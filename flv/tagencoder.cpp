#include "tagencoder.h"
#include "vcodec/vcodecutil.h"
#include "format.h"
#include "acodec/AAC.h"
#include <iostream>

namespace Format
{

using namespace vcodec;
using namespace acodec;

namespace Flv
{

TagEncoder::TagEncoder(EncodeTagCB tagCB)
    :tagCB_(std::move(tagCB))
{

}

AVCVideoTagEncoder::AVCVideoTagEncoder(EncodeTagCB tagCB)
    :TagEncoder(tagCB)
{
    tags_.reserve(1024);
}

int AVCVideoTagEncoder::encode(const uint8_t* frames,std::size_t size,uint32_t pts,uint32_t dts)
{
    splitStream(frames,frames + size,[&](const uint8_t* nalu,std::size_t len){
        return muxerNalu(nalu,len,pts,dts);
    },false);
    return 0;
}

int AVCVideoTagEncoder::encodeOne(const uint8_t* frame,std::size_t size,uint32_t pts,uint32_t dts)
{
    int slen = getStartCodeLen(frame);
    auto nalu = frame + slen;
    muxerNalu(nalu,size - slen,pts,dts);
    return 0;
}

int AVCVideoTagEncoder::muxerNalu(const uint8_t* nalu,std::size_t len,uint32_t pts, uint32_t dts)
{
    int type = nalu[0] & 0x1F;
    int spsid = 0;
    int ppsid = 0;
    AVCTagHeader vtag;
    std::vector<uint8_t> tagData;
    std::cout<<"nalu type "<<type <<" nalu size: "<< len<<std::endl;
    switch(type)
    {
    case 7:
        spsid = getSPSId(nalu,len);
        if(spss_.count(spsid) == 0 
            ||  spss_[spsid].size() != len 
                || !std::equal(spss_[spsid].begin(),spss_[spsid].end(),nalu))
        {
            spss_[spsid] = std::vector<uint8_t>(nalu,nalu + len);
            updateSequence_ = true;
        }
        break;
    case 8:
        ppsid = getPPSId(nalu,len);
        if(ppss_.count(ppsid) == 0 
            || ppss_[ppsid].size() != len 
                || !std::equal(ppss_[ppsid].begin(),ppss_[ppsid].end(),nalu))
        {
            ppss_[ppsid] = std::vector<uint8_t>(nalu,nalu + len);
            updateSequence_ = true;
        }
        break;
    default:
        if(updateSequence_ && ppss_.size() > 0 && spss_.size() > 0)
        {
            vtag.setFrameType((uint8_t)FrameFlag::KeyFrame);
            vtag.setCodecId((uint8_t)CodecID::AVC);
            vtag.avcPacketType = (uint8_t)VideoPacketType::FlvSequenceHeader;
            vtag.setCompositionTime(pts - dts);
            tagData.reserve(sizeof(AVCTagHeader));
            tagData.insert(tagData.end(),(uint8_t*)&vtag,(uint8_t*)&vtag + sizeof(AVCTagHeader));
            exportAVCDecoderConfigurationRecord(spss_,ppss_,tagData);
            updateSequence_ = false;
            std::cout<<"update sequence "<<std::endl;
            if(tagCB_(tagData.data(),tagData.size(),pts,dts) == 1)
                return 1;
        }
        tagData.resize(0);
        vtag.setFrameType(type == 5 ? (uint8_t)FrameFlag::KeyFrame : (uint8_t)FrameFlag::InterFrame);
        vtag.setCodecId((uint8_t)CodecID::AVC);
        vtag.avcPacketType = (uint8_t)VideoPacketType::FlvNalu;
        vtag.setCompositionTime(pts - dts);
        tagData.insert(tagData.end(),(uint8_t*)&vtag,(uint8_t*)&vtag + sizeof(AVCTagHeader)); 
        h264AnnexB2Avcc(nalu,len,tagData);
        return tagCB_(tagData.data(),tagData.size(),pts,dts);
    };
    return 0;
}


AACAudioTagEncoder::AACAudioTagEncoder(EncodeTagCB tagCB)
    :TagEncoder(tagCB)
{

}

int AACAudioTagEncoder::encode(const uint8_t* frames,std::size_t size,uint32_t pts,uint32_t dts)
{
    
    return 0;
}

int AACAudioTagEncoder::encodeOne(const uint8_t* frame,std::size_t size,uint32_t pts,uint32_t dts)
{
    AACTagHeader atag;
    atag.setSoundFormat(SoundFormat::AAC);
    atag.setSoundRate(SoundRate::FLVSoundRate44000);
    atag.setSoundSize(1);
    atag.setSoundType(1);
    AdtsFrame adts;
    if(adts.decode(frame,size) < 0)
    {
        std::cerr<<"decode aac frame failed"<<std::endl;
        return -1;
    }
    std::cout<<"aac raw size"<<adts.rawAACDataSize()<<std::endl;
    std::vector<uint8_t> tagData;
    tagData.reserve(size);
    if(updateAACSequence_)
    {
        atag.packetType = 0;
        auto asc = adts.getAudioSpecificConfig();
        tagData.insert(tagData.end(),(uint8_t*)(&atag),(uint8_t*)(&atag) + sizeof(AACTagHeader));
        tagData.insert(tagData.end(),asc.begin(),asc.end());
        tagCB_(tagData.data(), tagData.size(), pts, dts);
        updateAACSequence_ = false;
    }
    atag.packetType = 1;
    tagData.resize(0);
    tagData.insert(tagData.end(),(uint8_t*)(&atag),(uint8_t*)(&atag) + sizeof(AACTagHeader));
    tagData.insert(tagData.end(),adts.rawAACData(),adts.rawAACData() + adts.rawAACDataSize());
    tagCB_(tagData.data(), tagData.size(), pts, dts);
    return 0;
}







}



}


