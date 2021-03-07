#ifndef FLVTYPEENUM_H
#define FLVTYPEENUM_H

#include <cstdint>

namespace Format
{

namespace Flv
{

enum class TagType : uint8_t
{
    Audio       = 8,
    Video       = 9,
    ScriptData  = 18,
};

enum class SoundFormat : uint8_t
{
    NONE                    = 0xFF,
    LinearPCM               = 0 ,   //inear PCM, platform endian
    ADPCM                   = 1 , 
    MP3                     = 2 ,      
    LinearPCMLittleEndian   = 3 ,      
    Nellymoser16kHzmono     = 4 ,      
    Nellymoser8kHzmono      = 5 ,      
    Nellymoser              = 6 ,      
    G711A                   = 7 ,      
    G711U                   = 8 ,  
    reserved                = 9 ,  
    AAC                     = 10,      
    Speex                   = 11,      
    MP38kHz                 = 14,      
    DeviceSpecificSound     = 15,          
};

// 0 = 5.5 kHz
// 1 = 11 kHz
// 2 = 22 kHz
// 3 = 44 kHz
enum class SoundRate : uint8_t
{
    FLVSoundRate5500  = 0x00,
    FLVSoundRate11000 = 0x01,
    FLVSoundRate22000 = 0x02,
    FLVSoundRate44000 = 0x03,
};

enum class FrameFlag : uint8_t
{
    KeyFrame   = 1,
    InterFrame = 2,
};

enum class VideoPacketType : uint8_t
{
    FlvSequenceHeader = 0,
    FlvNalu           = 1,
    FlvEndSequence    = 2,
};

enum class CodecID : uint8_t
{
    NONE    = 0,
    AVC     = 7,   //h264
    HEVC    = 12,   //h265
};






}



}




#endif
