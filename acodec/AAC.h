#ifndef AAC_H
#define AAC_H

#include <cstdint>
#include <vector>
#include <functional>

namespace  Format 
{

namespace acodec
{

enum AACProfile : uint8_t
{
    MainProfile                 = 0,
    LowComplexityProfile        = 1,
    ScalableSamplingRateProfile = 2,
    Reserved                    = 3,
};

enum SamplingFrequency : uint32_t
{
    AACSampleRate96000      = 0x00,
    AACSampleRate88200      = 0x01,
    AACSampleRate64000      = 0x02,
    AACSampleRate48000      = 0x03,
    AACSampleRate44100      = 0x04,
    AACSampleRate32000      = 0x05,
    AACSampleRate24000      = 0x06,
    AACSampleRate22050      = 0x07,
    AACSampleRate16000      = 0x08,
    AACSampleRate12000      = 0x09,
    AACSampleRate11025      = 0x0a,
    AACSampleRate8000       = 0x0b,
    AACSampleRateReserved0  = 0x0c,
    AACSampleRateReserved1  = 0x0d,
    AACSampleRateReserved2  = 0x0e,
    AACSampleRateReserved3  = 0x0f,
};

// adts_fixed_header()
// {
//     syncword;                   12 bslbf
//     ID;                         1 bslbf
//     layer;                      2 uimsbf
//     protection_absent;          1 bslbf
//     profile;                    2 uimsbf
//     sampling_frequency_index;   4 uimsbf
//     private_bit;                1 bslbf
//     channel_configuration;      3 uimsbf
//     original/copy;              1 bslbf
//     home;                       1 bslbf
// }
//  protection_absent Indicates whether error_check() data is present or not
//
//  profile:
//  index    profile
//  0        Main profile
//  1        Low Complexity profile (LC)
//  2        Scalable Sampling Rate profile (SSR)
//  3        (reserved)
//
// channel_configuration
// 0-pce
// 1-single_channel_element,center front speaker
// 2-channel_pair_element left, right front speakers
// 3-single_channel_element,channel_pair_element single_channel_element 
// 4-center front speaker left, right center front speakers, rear surround
// 5-center front speaker left, right front speakers left surround, right surround rear speakers
// 6-center front speaker left, right front speakers, left surround, right surround rear speakers, front low frequency effects speaker
// 7-center front speaker left, right center front speakers, left, right outside front speakers, left surround, right surround rear speakers, front low frequency effects speaker

struct AdtsFixHeader
{
    uint16_t syncword;
    uint8_t id;
    uint8_t layer;
    uint8_t protectionAbsent;
    uint8_t profile;
    uint8_t samplingFrequencyIndex;
    uint8_t privateBit;
    uint8_t channelConfiguration;
    uint8_t original;
    uint8_t home;
};

// adts_variable_header()
// {
//     copyright_identification_bit;           1 bslbf
//     copyright_identification_start;         1 bslbf
//     frame_length;                           13 bslbf
//     adts_buffer_fullness;                   11 bslbf
//     number_of_raw_data_blocks_in_frame;     2 uimsfb
// }

struct AdtsVariableHeader
{
    uint8_t copyrightIdentificationBit;
    uint8_t copyrightIdentificationStart;
    uint16_t frameLength;
    uint16_t adtsBufferFullness;
    uint8_t numberOfRawDataBlocksInFrame;
};

void splitAACFrame(const uint8_t* start,const uint8_t* end,const std::function<int(const uint8_t*,std::size_t)> func);

const uint8_t* findAACSyncword(const uint8_t* aacFrame,size_t size);

class AdtsFrame
{
public:
    enum  { ADTSHdrLen = 7 };
public:

    int decode(const uint8_t* audioData, std::size_t len);

    const uint8_t* rawAACData() const { return aacData_ + aacHdrLen_; }
    std::size_t rawAACDataSize() const { return aacLen_ - aacHdrLen_; }

    //int decodeOne(const uint8_t* audioData, std::size_t len);
public:
    const AdtsFixHeader& getAdtsFixHeader() { return adtsFixHdr_; }
    const AdtsVariableHeader& getAdtsVariableHeader() { return adtsVariableHdr_; }
    std::vector<uint8_t> getAudioSpecificConfig();

    static std::vector<uint8_t> takeAdtsHeaderFromASC(const std::vector<uint8_t>& asc, uint16_t bytes);
    
private:
    AdtsFixHeader adtsFixHdr_;
    AdtsVariableHeader adtsVariableHdr_;
    const uint8_t* aacData_ = nullptr;
    std::size_t aacLen_ = 0;
    std::size_t aacHdrLen_ = 0;
};


}

}







#endif

