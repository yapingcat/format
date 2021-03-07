#include "format.h"
#include <stdexcept>
#include <cassert>

namespace Format
{
namespace Flv
{

std::vector<uint8_t> FlvHeader::encode()
{
    std::vector<uint8_t> flvhdr;
    flvhdr.reserve(9);
    flvhdr.resize(9);
    flvhdr[0] = signature[0];
    flvhdr[1] = signature[1];
    flvhdr[2] = signature[2];
    flvhdr[3] = version;
    flvhdr[4] = typeflags;
    flvhdr[5] = 0x00;
    flvhdr[6] = 0x00;
    flvhdr[7] = 0x00;
    flvhdr[8] = 0x09;
    return flvhdr;
}

int FlvHeader::decode(uint8_t* data, std::size_t len)
{
    if(len < 9)
        return -1;
    signature[0] = data[0];
    signature[1] = data[1];
    signature[2] = data[2];
    version      = data[3];
    typeflags    = data[4];
    dataoffset   |= static_cast<uint32_t>(data[5]) << 24;
    dataoffset   |= static_cast<uint32_t>(data[6]) << 16;
    dataoffset   |= static_cast<uint32_t>(data[7]) << 8;
    dataoffset   |= static_cast<uint32_t>(data[8]);
    if(signature[0] != 'F' || signature[1] != 'L' || signature[2] != 'V' || dataoffset != 9)
        return -1;
    return 0;
}

std::vector<uint8_t> FlvTagCommonHeader::encode()
{
    std::vector<uint8_t> commonHeader;
    commonHeader.reserve(11);
    commonHeader.resize(11);
    commonHeader[0]  = tagType;
    commonHeader[1]  = dataSize[0];
    commonHeader[2]  = dataSize[1];
    commonHeader[3]  = dataSize[2];
    commonHeader[4]  = timestamp[0];
    commonHeader[5]  = timestamp[1];
    commonHeader[6]  = timestamp[2];
    commonHeader[7]  = timestampExtended;
    commonHeader[8]  = streamId[0];
    commonHeader[9]  = streamId[1];
    commonHeader[10] = streamId[2];
    return commonHeader;
}

int FlvTagCommonHeader::decode(uint8_t* data, std::size_t len)
{
    if(len < 11)
        return -1;
    tagType             = data[0];
    dataSize[0]         = data[1];
    dataSize[1]         = data[2];
    dataSize[2]         = data[3];
    timestamp[0]        = data[4];
    timestamp[1]        = data[5];
    timestamp[2]        = data[6];
    timestampExtended   = data[7];
    streamId[0]         = data[8];
    streamId[1]         = data[9];
    streamId[2]         = data[10];
    return 0;
}

AudioTagHeader AudioTagHeader::decode(const uint8_t* atag,std::size_t len)
{
    assert(len > 1);
    if(len <= 1)
    {
        throw std::invalid_argument("len <= 1");
    }

    AudioTagHeader ataghdr;
    ataghdr.soundParam = atag[0];
    return ataghdr;
}


VideoTagHeader VideoTagHeader::decode(const uint8_t* vtag,std::size_t len)
{
    assert(len > 1);
    if(len <= 1)
    {
        throw std::invalid_argument("len <= 1");
    }
    VideoTagHeader vtaghdr;
    vtaghdr.frameParam = vtag[0];
    return vtaghdr;
}

}
}


