#include "bits.h"
#include <bitset>
#include <array>
#include <exception>
#include <stdexcept>

namespace Format
{
namespace common
{

std::array<uint8_t,8> BitMasks = {0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0xFF};

Bits::Bits(const uint8_t* bits,std::size_t len)
    :start_(bits)
    ,end_(bits + len)
    ,bytesOffset_(0)
    ,bitsOffset_(0)
{
    
}

uint32_t Bits::getBit(uint32_t n)
{
    if(n > 32)
    {
        throw std::invalid_argument("argument n is bigger than 32");
    }
    
    if(start_ + bytesOffset_ >= end_)
    {
        throw std::out_of_range("out_of_range end of stream");
    }

    uint32_t ret = 0;
    if(8 - bitsOffset_ >= n)
    {    
        ret = (start_[bytesOffset_] >> (8 - bitsOffset_ - n)) & BitMasks[n - 1];
        bitsOffset_ += n;
        if(bitsOffset_ == 8)
        {
            bytesOffset_ ++;
            bitsOffset_ = 0;
        }
    }
    else
    {
        ret = start_[bytesOffset_++] & BitMasks[8 - bitsOffset_ - 1];
        n -= 8 - bitsOffset_;
        do {

            if(start_ + bytesOffset_ >= end_)
            {
                throw std::out_of_range("out_of_range end of stream");
            }
            if(n >= 8)
            {
                ret = ret << 8 | start_[bytesOffset_++];
                n -= 8;
            }
            else
            {
                ret = (ret << n) | (start_[bytesOffset_] >> (8 - n) & BitMasks[n - 1]);
                bitsOffset_ = n;
                break;
            }
        }while(1);
    }
    return ret;
}


}

}


