#ifndef FLVTAGDECODER_H
#define FLVTAGDECODER_H

#include <cstdint>
#include <functional>
#include <unordered_map>
#include <vector>

#include "typeenum.h"

namespace Format
{

namespace Flv
{


using DecodeTagCB = std::function<void(TagType,uint8_t,uint8_t*,std::size_t,uint32_t,uint32_t)>;

class TagDecoder
{
public:
    TagDecoder(DecodeTagCB cb);
    ~TagDecoder() = default;

public:
    
    int inputAudioData(const uint8_t* audio,std::size_t len,uint32_t dts);

    int inputVideoData(const uint8_t* video,std::size_t len,uint32_t dts);

private:
    DecodeTagCB cb_;
    std::unordered_map<uint32_t,std::vector<uint8_t>> spss_;
    std::unordered_map<uint32_t,std::vector<uint8_t>> ppss_;
    std::vector<uint8_t> asc_;
};




}



}









#endif


