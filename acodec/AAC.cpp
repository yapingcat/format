#include "AAC.h"
#include "bits.h"
#include <cassert>

namespace Format
{
using namespace common;
namespace acodec
{

const uint8_t* findAACSyncword(const uint8_t* aacFrame,size_t size)
{
    for(std::size_t i = 0; i < size - 1; i++)
    {
        if(aacFrame[i] == 0xFF && (aacFrame[i + 1] & 0xF0) == 0xF0)
            return aacFrame + i;
    }
    return nullptr;
}

void splitAACFrame(const uint8_t* start,const uint8_t* end,const std::function<int(const uint8_t*,std::size_t)> func)
{
    start = findAACSyncword(start, end - start);
    while(start < end)
    {
        AdtsFrame adts;
        if(end - start < 7 || adts.decode(start,7) != 0)
            break;
        auto aacLen = adts.getAdtsVariableHeader().frameLength;
        if(func(start,aacLen) == 1)
            break;
        start = start + aacLen;
    }
    return;
}


int AdtsFrame::decode(const uint8_t* audioData, std::size_t len)
{
    if(len < ADTSHdrLen)
    {
        assert(0);
        return -1;
    }
    
    Bits bitsReader(audioData,len);
    adtsFixHdr_.syncword                = bitsReader.getBit(12);
    adtsFixHdr_.id                      = bitsReader.getBit(1);
    adtsFixHdr_.layer                   = bitsReader.getBit(2);
    adtsFixHdr_.protectionAbsent        = bitsReader.getBit(1);
    adtsFixHdr_.profile                 = bitsReader.getBit(2);
    adtsFixHdr_.samplingFrequencyIndex  = bitsReader.getBit(4);
    adtsFixHdr_.privateBit              = bitsReader.getBit(1);
    adtsFixHdr_.channelConfiguration    = bitsReader.getBit(3);
    adtsFixHdr_.original                = bitsReader.getBit(1);
    adtsFixHdr_.home                    = bitsReader.getBit(1);

    adtsVariableHdr_.copyrightIdentificationBit   =  bitsReader.getBit(1);
    adtsVariableHdr_.copyrightIdentificationStart =  bitsReader.getBit(1);
    adtsVariableHdr_.frameLength                  =  bitsReader.getBit(13);
    adtsVariableHdr_.adtsBufferFullness           =  bitsReader.getBit(11);   
    adtsVariableHdr_.numberOfRawDataBlocksInFrame =  bitsReader.getBit(2);
    
    if(adtsFixHdr_.syncword != 0xFFF)
    {
        assert(0);
        return -1;
    }
    aacLen_     = len;
    aacHdrLen_  = 7;    //program_config_element is not support  ISO_IEC_13818-7-AAC-2004 Page 46
    aacData_    = audioData;
    return 0;        
}


//   bits	Purpose	Value	           
//     5	audioObjectType	           
//     4	samplingFrequencyIndex	   
//     4	channelConfiguration	   
//     1	GA frameLengthFlag	       
//     1	GA dependsOnCoreCoder	   
//     1	GA extensionFlag	       

std::vector<uint8_t> AdtsFrame::getAudioSpecificConfig()
{
    std::vector<uint8_t> asc(2,0);
    asc[0] |= ((adtsFixHdr_.profile & 0x1F) + 1) << 3;              //audioObjectType
    asc[0] |= (adtsFixHdr_.samplingFrequencyIndex & 0x0F) >> 1;     //samplingFrequencyIndex
    asc[1] |= (adtsFixHdr_.samplingFrequencyIndex & 0x01) << 7;     //samplingFrequencyIndex
    asc[1] |= (adtsFixHdr_.channelConfiguration & 0x0F) << 3;       //channelConfiguration
    asc[1] |=  0;                                                   //GA frameLengthFlag
    asc[1] |=  0;                                                   //GA dependsOnCoreCoder
    asc[1] |=  0;                                                   //GA dependsOnCoreCoder

    return asc;
}

std::vector<uint8_t> AdtsFrame::takeAdtsHeaderFromASC(const std::vector<uint8_t>& asc, uint16_t bytes)
{
    assert(asc.size() >= 2);
    std::vector<uint8_t> hdr(7,0);
    hdr[0] = 0xFF;
    hdr[1] = 0xF0;
    hdr[1] |= 0x01;  //protection_absent Indicates whether error_check() data is present or not.
    hdr[2] = ((((asc[0] & 0xF8) >> 3) - 1 ) & 0x03) << 6;
    hdr[2] |= (((asc[0] & 0x07) << 1) | ( (asc[1] & 0x80) >> 7)) << 2;
    hdr[2] |= (asc[1] & 0x38) >> 5;
    hdr[3] |= (asc[1] & 0x38) << 3;
    hdr[3] |= (static_cast<uint8_t>(bytes >> 8) & 0x1F) >> 3;
    hdr[4] = ((static_cast<uint8_t>(bytes >> 8) & 0x1F) << 6) | (static_cast<uint8_t>(bytes & 0xFF) >> 3);
    hdr[5] = (static_cast<uint8_t>(bytes & 0xFF) << 5);
    hdr[6] = 0xFC; //i do not know why
    return hdr;
}


}
}







