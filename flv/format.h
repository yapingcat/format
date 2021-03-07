#ifndef FLVFORMAT_H
#define FLVFORMAT_H

#include "typeenum.h"
#include <vector>
#include <cstdint>

namespace Format
{

namespace Flv
{


struct FlvHeader
{
    uint8_t signature[3] = {'F','L','V'};
    uint8_t version = 0x01;
    uint8_t typeflags = 0;  //|TypeFlagsReserved|TypeFlagsAudio|TypeFlagsReserved|TypeFlagsVideo
    uint32_t dataoffset = 9;

    void setAudioFlag(int flag);
    void setVideoFlag(int flag);

    std::vector<uint8_t> encode();
    int decode(uint8_t* data, std::size_t len);
};

inline void FlvHeader::setAudioFlag(int ispresent)
{
    typeflags &= 0xFB;
    typeflags |= (ispresent ? 0x04 : 0x00);
}

inline void FlvHeader::setVideoFlag(int ispresent)
{
    typeflags &= 0xFE;
    typeflags |= (ispresent ? 0x01 : 0x00);
}

struct FlvTagCommonHeader
{
    uint8_t tagType = 0;  //|Reserved|Filter|TagType  Reserved Filter should be 0;
    uint8_t dataSize[3] = {0};
    uint8_t  timestamp[3] = {0};
    uint8_t timestampExtended = 0;
    uint8_t streamId[3] = {0};    //always be 0

    //8-audio 9-video 18-scribe data
    void setTagType(uint8_t type);
    void setDataSize(uint32_t dataSize);
    uint32_t getDataSize();
    void setTimestamp(uint32_t timestamp);
    uint32_t getTimeStamp();

    std::vector<uint8_t> encode();
    int decode(uint8_t* data, std::size_t len);
};

inline void FlvTagCommonHeader::setDataSize(uint32_t size)
{
    dataSize[0] = static_cast<uint8_t>(size >> 16);
    dataSize[1] = static_cast<uint8_t>(size >> 8);
    dataSize[2] = static_cast<uint8_t>(size);
}

inline uint32_t FlvTagCommonHeader::getDataSize()
{
    uint32_t size = 0;
    size |= static_cast<uint32_t>(dataSize[0]) << 16;
    size |= static_cast<uint32_t>(dataSize[1]) << 8;
    size |= static_cast<uint32_t>(dataSize[2]);
    return size;
}

inline void FlvTagCommonHeader::setTagType(uint8_t type)
{
    tagType = type & 0x1F;
}

inline void FlvTagCommonHeader::setTimestamp(uint32_t ts)
{
    timestamp[0] = (uint8_t)((ts >> 16) & 0xFF);
    timestamp[1] = (uint8_t)((ts >> 8) & 0xFF);
    timestamp[2] = (uint8_t)(ts  & 0xFF);
    if(ts >> 24)
    {
        timestampExtended = (uint8_t)((ts >> 24) & 0xFF);
    }
}

inline uint32_t FlvTagCommonHeader::getTimeStamp()
{
    uint32_t ts = 0;
    ts |= static_cast<uint32_t>(timestamp[0]) << 16;
    ts |= static_cast<uint32_t>(timestamp[1]) << 8;
    ts |= static_cast<uint32_t>(timestamp[2]);
    if(timestampExtended)
    {
        ts |= static_cast<uint32_t>(timestampExtended) << 24;
    }
    return ts;
}

struct AudioTagHeader 
{
    uint8_t soundParam;   // |SoundFormat(UB[4])|SoundRate(UB[2])|SoundSize(UB[1])|SoundType(UB[1])|

    // 0 = Linear PCM, platform endian
    // 1 = ADPCM
    // 2 = MP3
    // 3 = Linear PCM, little endian
    // 4 = Nellymoser 16 kHz mono
    // 5 = Nellymoser 8 kHz mono
    // 6 = Nellymoser
    // 7 = G.711 A-law logarithmic PCM
    // 8 = G.711 mu-law logarithmic PCM
    // 9 = reserved
    // 10 = AAC
    // 11 = Speex
    // 14 = MP3 8 kHz
    // 15 = Device-specific sound
    void setSoundFormat(SoundFormat sf);
    SoundFormat getSoundFormat();

    // 0 = 5.5 kHz
    // 1 = 11 kHz
    // 2 = 22 kHz
    // 3 = 44 kHz
    void setSoundRate(SoundRate sr);
    SoundRate getSoundRate();

    // 0 = 8-bit samples
    // 1 = 16-bit samples
    void setSoundSize(uint8_t ss);
    uint8_t getSoundSize();
    
    // 0 = Mono sound
    // 1 = Stereo sound
    void setSoundType(uint8_t st);
    uint8_t getSoundType();

    static AudioTagHeader decode(const uint8_t* atag,std::size_t len);
};


inline void AudioTagHeader::setSoundFormat(SoundFormat sf)
{
    soundParam |= ((uint8_t)sf & 0x0F) << 4;
}

inline SoundFormat AudioTagHeader::getSoundFormat()
{
    return (SoundFormat)((soundParam & 0xF0) >> 4);
}

inline void AudioTagHeader::setSoundRate(SoundRate sr)
{
    soundParam |= ((uint8_t)sr & 0x03) << 2;
}

inline SoundRate AudioTagHeader::getSoundRate()
{
    return  (SoundRate)((soundParam & 0xC0) >> 2);
}

inline void AudioTagHeader::setSoundSize(uint8_t ss)
{
    soundParam |= ((uint8_t)ss & 0x01) << 1;
}

inline uint8_t AudioTagHeader::getSoundSize()
{
    return soundParam & 0x02 >> 1;
}

inline void AudioTagHeader::setSoundType(uint8_t st)
{
    soundParam |= (uint8_t)st & 0x01;
}

inline uint8_t AudioTagHeader::getSoundType()
{
    return soundParam & 0x01;
}

struct AACTagHeader : public AudioTagHeader
{
    uint8_t packetType;  //0 = AAC sequence header,1 = AAC raw
};

struct VideoTagHeader
{
    uint8_t frameParam; // |Frame Type|CodecID|

    // 1 = key frame (for AVC, a seekable frame)
    // 2 = inter frame (for AVC, a non-seekable frame)
    // 3 = disposable inter frame (H.263 only)
    // 4 = generated key frame (reserved for server use only)
    // 5 = video info/command frame
    void setFrameType(uint8_t type);
    uint8_t getFrameType();

    // 2 = Sorenson H.263
    // 3 = Screen video
    // 4 = On2 VP6
    // 5 = On2 VP6 with alpha channel
    // 6 = Screen video version 2
    // 7 = AVC
    void setCodecId(uint8_t cid);
    uint8_t getCodecId();

    static VideoTagHeader decode(const uint8_t* vtag,std::size_t len);
};

inline void VideoTagHeader::setFrameType(uint8_t type)
{
    frameParam &= 0x0F;
    frameParam |= (type & 0x0F) << 4;
}

inline uint8_t VideoTagHeader::getFrameType()
{
    return (frameParam & 0xF0) >> 4;
}

inline void VideoTagHeader::setCodecId(uint8_t cid)
{
    frameParam &= 0xF0;
    frameParam |= cid & 0x0F;
}

inline uint8_t VideoTagHeader::getCodecId()
{
    return frameParam & 0x0F;
}

struct AVCTagHeader : public VideoTagHeader
{
    // 0 = AVC sequence header
    // 1 = AVC NALU
    // 2 = AVC end of sequence (lower level NALU sequence ender is not required or supported)
    uint8_t avcPacketType;
    int8_t compositionTime[3];

    //ISO 14496-12, 8.15.3 cts = pts -dts
    void setCompositionTime(int32_t diffTime);
};

inline void AVCTagHeader::setCompositionTime(int32_t diffTime)
{
    compositionTime[0] = (uint8_t)((diffTime >> 16) & 0xFF);
    compositionTime[1] = (uint8_t)((diffTime >> 8) & 0xFF);
    compositionTime[2] = (uint8_t)(diffTime  & 0xFF);
}





}

}






#endif

