#ifndef VCODECUTIL_H
#define VCODECUTIL_H

#include <cstdint>
#include <functional>
#include <vector>
#include <unordered_map>

namespace Format
{
namespace vcodec
{

enum CodeId
{
    H264 = 0,
    H265 = 1,
};

int getStartCodeLen(const uint8_t* frame);

//withStartCode: frame with startcode(0x00000001)
inline int h264NaluType(const uint8_t* frame, bool withStartCode = true)
{
    if(!withStartCode)
        return frame[0] & 0x1f;
    return frame[getStartCodeLen(frame)] & 0x1f;
}

const uint8_t* findStartCode(const uint8_t* frame,size_t size,int& scl,bool withStartCode = true);

/// @param  withStartCode: true- splited nalus that start with 0x00000001 false- splited nalus that start without 0x00000001
void splitStream(const uint8_t* start,
                 const uint8_t* end, 
                 const std::function<int(const uint8_t*,size_t)>& func,
                 bool withStartCode = true);


int exportAVCDecoderConfigurationRecord(std::unordered_map<uint32_t,std::vector<uint8_t>>& spss, 
                                        std::unordered_map<uint32_t,std::vector<uint8_t>>& ppss, 
                                        std::vector<uint8_t>& extraData);

int importAVCDecoderConfigurationRecord(std::unordered_map<uint32_t,std::vector<uint8_t>>& spss, 
                                        std::unordered_map<uint32_t,std::vector<uint8_t>>& ppss,
                                        const uint8_t* acvSequence,
                                        std::size_t len);

void h264AnnexB2Avcc(const uint8_t* nalu,size_t len,std::vector<uint8_t>& avcc);

uint32_t getSPSId(const uint8_t* sps, std::size_t size);

uint32_t getPPSId(const uint8_t* pps, std::size_t size);

uint32_t readue(const uint8_t* bits, std::size_t bitsnum,std::size_t& offsetBytes,std::size_t& offsetBits);

int readse(const uint8_t* bits, std::size_t bitsnum, std::size_t& offsetBytes, std::size_t& offsetBits);




}


}




#endif
