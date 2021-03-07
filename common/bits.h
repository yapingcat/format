#ifndef FORMAT_BITS_H
#define FORMAT_BITS_H

#include <cstdint>
#include <string>

namespace Format
{
namespace common
{

class Bits
{
public:

    Bits(const uint8_t* bits,std::size_t len);
    ~Bits() = default;

public:
    
    uint32_t getBit(uint32_t n);
    
private:
    const uint8_t* start_;
    const uint8_t* end_;
    std::size_t bytesOffset_;
    std::size_t bitsOffset_;
};




}
}

#endif
