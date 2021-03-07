#ifndef FLVTAGENCODER_H
#define FLVTAGENCODER_H


#include <cstdint>
#include <functional>
#include <vector>
#include <unordered_map>

#include "typeenum.h"
#include <thread>

namespace Format
{

namespace Flv
{

///encode tag callback function, return value is 0 or 1. 0 means to continue to encode,1 terminate the encode process
using EncodeTagCB = std::function<int(const uint8_t* tag, std::uint32_t size,uint32_t pts,uint32_t dts)>;

class TagEncoder
{
public:
    TagEncoder(EncodeTagCB tagCB);
    virtual ~TagEncoder() = default;

public:
    
    virtual int encode(const uint8_t* frames,std::size_t size,uint32_t pts,uint32_t dts) = 0;
    virtual int encodeOne(const uint8_t* frame,std::size_t size,uint32_t pts,uint32_t dts) = 0;

protected:
    EncodeTagCB tagCB_;
    std::vector<uint8_t> tags_;
};

class AVCVideoTagEncoder : public TagEncoder
{
public:
    AVCVideoTagEncoder(EncodeTagCB tagCB);
    ~AVCVideoTagEncoder() = default;

public:

    /// @param[in] frames annexb nalu byte stream,start with 0x00000001/0x000001
    /// @return 0-ok,-1-failed
    int encode(const uint8_t* frames,std::size_t size,uint32_t pts,uint32_t dts);
    int encodeOne(const uint8_t* frame,std::size_t size,uint32_t pts,uint32_t dts);

private:
    int muxerNalu(const uint8_t* nalu,std::size_t len,uint32_t pts,uint32_t dts);

private:

    std::unordered_map<uint32_t,std::vector<uint8_t>> spss_;
    std::unordered_map<uint32_t,std::vector<uint8_t>> ppss_;
    bool updateSequence_ = false;
};


class AACAudioTagEncoder : public TagEncoder
{
public:
    AACAudioTagEncoder(EncodeTagCB tagCB);
    ~AACAudioTagEncoder() = default;
public:
    int encode(const uint8_t* frames,std::size_t size,uint32_t pts,uint32_t dts);
    int encodeOne(const uint8_t* frame,std::size_t size,uint32_t pts,uint32_t dts);

private:
    bool updateAACSequence_ = true;
};






}



}









#endif


