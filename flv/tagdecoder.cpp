#include "tagdecoder.h"
#include "format.h"
#include "acodec/AAC.h"
#include "vcodec/vcodecutil.h"
#include <cassert>

namespace Format
{

using namespace acodec;
using namespace vcodec;

namespace Flv
{

std::array<uint8_t,4> StartCodeArray = {0x00,0x00,0x00,0x01};

TagDecoder::TagDecoder(DecodeTagCB cb)
    :cb_(cb)
{

}

int TagDecoder::inputAudioData(const uint8_t* audio,std::size_t len,uint32_t dts)
{
    AudioTagHeader ahdr = AudioTagHeader::decode(audio,len);
    if(ahdr.getSoundFormat() !=  SoundFormat::AAC)
        return -1;
    uint8_t aacPacketType = audio[1];
    assert(aacPacketType == 0 || aacPacketType == 1);
    if(aacPacketType == 0)
    {
        assert(len >= 4);
        if(len < 4)
            return -1; 
        asc_.insert(asc_.end(),audio + 2,audio + len);
        return 0;
    }
    else if(aacPacketType == 1)
    {
        std::vector<uint8_t> aacframe;
        aacframe = AdtsFrame::takeAdtsHeaderFromASC(asc_,len - 2 + 7);   //2 byte aac audio tag header,7 bytes aac header
        aacframe.reserve(len - 2 + 7);
        aacframe.insert(aacframe.end(),audio + 2,audio + len);
        cb_(TagType::Audio,(uint8_t)ahdr.getSoundFormat(),aacframe.data(),aacframe.size(),dts,dts);
    }
    else
    {
        return -1;
    }
    return 0;
}

int TagDecoder::inputVideoData(const uint8_t* video,std::size_t len,uint32_t dts)
{
    VideoTagHeader vhdr = VideoTagHeader::decode(video,len);
    if(vhdr.getCodecId() != 7) // other codec id is not support
        return -1;
    
    uint8_t avcPacketType = video[1];
    int cts = 0;
    if(avcPacketType == 1)
    {
        cts |= static_cast<uint32_t>(video[2]) << 16;
        cts |= static_cast<uint32_t>(video[3]) << 8;
        cts |= static_cast<uint32_t>(video[4]);
    }
    uint32_t pts = cts + dts;
    
    if(avcPacketType == (uint8_t)VideoPacketType::FlvSequenceHeader)
    {
        importAVCDecoderConfigurationRecord(spss_,ppss_,video + 5,len - 5);
    }
    else if(avcPacketType == (uint8_t)VideoPacketType::FlvNalu)
    {
        uint32_t offset = 5;
        while(offset < len)
        {
            std::vector<uint8_t> nalu;
            uint32_t naluSize = 0;
            naluSize |= static_cast<uint32_t>(video[offset++]) << 24;
            naluSize |= static_cast<uint32_t>(video[offset++]) << 16;
            naluSize |= static_cast<uint32_t>(video[offset++]) << 8;
            naluSize |= static_cast<uint32_t>(video[offset++]);
            assert(naluSize <= len - offset);
            if(naluSize > len - offset)
            {
                return -1;
            }

            if(h264NaluType(video + offset) == 5)
            {
                for(auto&& sps : spss_)
                {
                    nalu.insert(nalu.end(),StartCodeArray.begin(),StartCodeArray.end());
                    nalu.insert(nalu.end(),sps.second.begin(),sps.second.end());
                }

                for(auto&& pps : ppss_)
                {
                    nalu.insert(nalu.end(),StartCodeArray.begin(),StartCodeArray.end());
                    nalu.insert(nalu.end(),pps.second.begin(),pps.second.end());
                }
            }
            nalu.reserve(nalu.size() + 4 + naluSize);
            nalu.insert(nalu.end(),StartCodeArray.begin(),StartCodeArray.end());
            nalu.insert(nalu.end(),video + offset,video + offset + naluSize);
            cb_(TagType::Video,vhdr.getCodecId(),nalu.data(),nalu.size(),pts,dts);
            offset += naluSize;
        }
    }
    return 0;
}





}


}


