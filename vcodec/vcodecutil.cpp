#include "vcodecutil.h"
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <cassert>

namespace Format
{

namespace vcodec
{

int getStartCodeLen(const uint8_t* frame)
{
    if(frame[0] == 0x00 && frame[1] == 0x00)
    {
        if(frame[2] == 0x01)
            return 3;
        else if(frame[2] == 0x00 && frame[3] == 0x01)
            return 4;
    }
    return 0;
}

const uint8_t* findStartCode(const uint8_t* frame,size_t size,int& scl,bool withStartCode)
{
    if(size < 3)
    {
        return nullptr;
    }

    for(size_t i = 0;i < size - 3; i++)
    {
        if(frame[i] == 0x00 && frame[i+1] == 0x00)
        {
            if(frame[i+2] == 0x01)
            {
                scl = 3;
                return withStartCode ? frame + i : frame + i + 3;
            }
            else if(frame[i+2] == 0x00 && frame[i+3] == 0x01)
            {
                scl = 4;
                return withStartCode ? frame + i : frame + i + 4;
            }
        }
    }
    return nullptr;
}


void splitStream(const uint8_t* start,
                 const uint8_t* end,
                 const std::function<int(const uint8_t*,size_t)>& func,
                 bool withStartCode)
{
    int scl = 0;
    const uint8_t* nalu = findStartCode(start,end - start,scl,withStartCode);
    while(nullptr != nalu)
    {
        const uint8_t* nextStart = withStartCode ?  nalu + scl : nalu;
        size_t leftSize = withStartCode ?  end - nalu - scl: end - nalu;
        const uint8_t* nalu2 = findStartCode(nextStart,leftSize,scl,withStartCode);
        if(func(nalu,nalu2 == nullptr ? end - nalu : nalu2 - nalu - (withStartCode ? 0 : scl)) == 1)
            break;
        nalu = nalu2;
    }
}


//based on https://stackoverflow.com/questions/24884827/possible-locations-for-sequence-picture-parameter-sets-for-h-264-stream#
int exportAVCDecoderConfigurationRecord(std::unordered_map<uint32_t,std::vector<uint8_t>>& spss, 
                                        std::unordered_map<uint32_t,std::vector<uint8_t>>& ppss, 
                                        std::vector<uint8_t>& extraData)
{
    std::size_t totalSize = spss.size() * 2;
    for(auto&& sps : spss)
    {
        totalSize += sps.second.size();
    }
    totalSize += ppss.size() * 2;
    for(auto&& pps : ppss)
    {
        totalSize += pps.second.size();
    }
    totalSize += 7;
    std::size_t startidx = extraData.size();
    extraData.reserve(startidx + totalSize);
    extraData.resize(startidx + totalSize);
    extraData[startidx++] = 0x01;                    //version
    extraData[startidx++] = (spss.begin()->second)[1];      //avc profile
    extraData[startidx++] = (spss.begin()->second)[2];      //avc compatibility
    extraData[startidx++] = (spss.begin()->second)[3];      //avc level
    extraData[startidx++] = 0xFF;                    //8 bit: 6 bit is reserved(always 0xFC), 2 bit is NALULengthSizeMinusOne
                                            //0xFF:4 bytes to stor length of nalu
    extraData[startidx++] = 0xE0 | static_cast<uint8_t>(spss.size());    //8 bit: 3 bit is reserved(always 0xE0), 5 bit is number of SPS NALUs
    
    for(auto&& sps : spss)
    {
        uint16_t spssize = static_cast<uint16_t>(sps.second.size());
        extraData[startidx++] = static_cast<uint8_t>((spssize >> 8) & 0xFF);
        extraData[startidx++] = static_cast<uint8_t>(spssize & 0xFF); 
        std::copy_n(sps.second.begin(),spssize,extraData.begin() + startidx);
        startidx += spssize;
    }
    extraData[startidx++] = static_cast<uint8_t>(spss.size());
    for(auto&& pps : ppss)
    {
        uint16_t ppssize = static_cast<uint16_t>(pps.second.size());
        extraData[startidx++] = static_cast<uint8_t>((ppssize >> 8) & 0xFF);
        extraData[startidx++] = static_cast<uint8_t>(ppssize & 0xFF); 
        std::copy_n(pps.second.begin(),ppssize,extraData.begin() + startidx);
        startidx += ppssize;
    }
    return 0;
}

int importAVCDecoderConfigurationRecord(std::unordered_map<uint32_t,std::vector<uint8_t>>& spss, 
                                        std::unordered_map<uint32_t,std::vector<uint8_t>>& ppss,
                                        const uint8_t* acvSequence,
                                        std::size_t len)
{
    assert(len > 7);
    if(len <= 7)
    {
        return -1;
    }

    uint8_t spsnum = acvSequence[5] & 0x1F;
    std::size_t offset = 6;
    for(uint8_t i = 0; i < spsnum; i++)
    {
        uint16_t spssize = ((uint16_t)(acvSequence[offset]) << 8) | (uint16_t)acvSequence[offset + 1];
        auto spsid = getSPSId(acvSequence + offset + 2, len - offset - 2);
        spss[spsid] = std::vector<uint8_t>(acvSequence + offset + 2,acvSequence + offset + 2 + spssize);
        offset += 2 + spssize;
        if(offset > len)
            return -1;
    }
    uint8_t ppsnum = acvSequence[offset++];
    for(uint8_t i = 0; i < ppsnum; i++)
    {
        uint16_t ppssize = ((uint16_t)(acvSequence[offset]) << 8) | (uint16_t)acvSequence[offset + 1];
        auto ppsid = getSPSId(acvSequence + offset + 2, len - offset - 2);
        ppss[ppsid] = std::vector<uint8_t>(acvSequence + offset + 2,acvSequence + offset + 2 + ppssize);
        offset += 2 + ppssize;
        if(offset > len)
            return -1;
    }
    return 0;
}

void h264AnnexB2Avcc(const uint8_t* nalu,size_t len,std::vector<uint8_t>& avcc)
{
    auto start = avcc.size();
    avcc.reserve(start + 4 + len);
    avcc.resize(start + 4);
    avcc[start]     = static_cast<uint8_t>(len >> 24);
    avcc[start + 1] = static_cast<uint8_t>(len >> 16);
    avcc[start + 2] = static_cast<uint8_t>(len >> 8);
    avcc[start + 3] = static_cast<uint8_t>(len);
    avcc.insert(avcc.end(),nalu , nalu + len);
}

uint32_t getSPSId(const uint8_t* sps, std::size_t size)
{
    const uint8_t* spsid = sps + 4; //4 means: 1 byte nalu hdr,1 byte profile_idc,1 byte constraint_set_flag, 1 byte level_id

    std::size_t offsetBits   = 0;
    std::size_t offsetBytes  = 0;
    return readue(spsid,size,offsetBytes,offsetBits);
}

uint32_t getPPSId(const uint8_t* pps, std::size_t size)
{
    const uint8_t* ppsid = pps + 1; //1 means: 1 byte nalu hdr 
    std::size_t offsetBits   = 0;
    std::size_t offsetBytes  = 0;
    return readue(ppsid,size,offsetBytes,offsetBits);
}

//ISO_IEC_14496-10 9.1 Parsing process for Exp-Golomb codes
uint32_t readue(const uint8_t* bits, std::size_t bytes,std::size_t& offsetBytes,std::size_t& offsetBits)
{
    int leadingZeroBits = -1;
    for(int b = 0; !b ; leadingZeroBits++)
    {
        if(offsetBits >= 8)
        {
            offsetBytes++;
            offsetBits = 0;
            if(offsetBytes >= bytes)
            {
                throw std::out_of_range("out_of_range readue offsetByte equal to bytes");
            }
        }
        b = (bits[offsetBytes] >> (7 - offsetBits)) & 0x01;
        offsetBits++;
    }

    uint32_t id = 0;
    for(int i = 0; i < leadingZeroBits; i++)
    {
        if(offsetBits >= 8)
        {
            offsetBytes++;
            offsetBits = 0;
            if(offsetBytes >= bytes)
            {
                throw std::out_of_range("out_of_range readue offsetByte equal to bytes");
            }
        }
        id = id << 1;
        id |= (bits[offsetBytes] >> (7 - offsetBits)) & 0x01;
        offsetBits++;
    }

    id = (1 << leadingZeroBits) + id - 1;
    return id;
}

int readse(const uint8_t* bits, std::size_t bitsnum, std::size_t& offsetBytes, std::size_t& offsetBits)
{
    auto v = readue(bits,bitsnum,offsetBytes,offsetBits);
    return (v % 2 == 0) ?  ( - 1 * (v / 2)) : ((v + 1) / 2);
}


}


}




