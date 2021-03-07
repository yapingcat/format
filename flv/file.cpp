#include "file.h"
#include "format.h"

namespace Format
{

namespace Flv
{

FlvFile::FlvFile(const std::string& filename)
    :filename_(filename)
{

}

FlvFileWriter::FlvFileWriter(const std::string& filename,CodecID cid, SoundFormat sf)
    :FlvFile(filename)
    ,flvout_(filename_,std::ofstream::out | std::ofstream::binary)
{
    FlvHeader hdr;
    hdr.setVideoFlag(1);
    hdr.setAudioFlag(1);
    auto hdrData = hdr.encode();
    flvout_.write(reinterpret_cast<char*>(hdrData.data()),hdrData.size());
    char previousTagSize0[4] = {0x00,0x00,0x00,0x00};
    flvout_.write(previousTagSize0,4); //PreviousTagSize0,Always 0

    auto vtagWriter = [this](const uint8_t* tag, std::uint32_t size,uint32_t pts,uint32_t dts) {
            FlvTagCommonHeader ftchdr;
            ftchdr.setTagType(9);
            ftchdr.setDataSize(size);
            ftchdr.setTimestamp(pts);
            auto flvtaghdr = ftchdr.encode();
            flvout_.write(reinterpret_cast<char*>(flvtaghdr.data()),flvtaghdr.size());
            flvout_.write(reinterpret_cast<const char*>(tag),size);
            std::vector<uint8_t> previousTagSize(4,0);
            size += 11;
            previousTagSize[0] = static_cast<uint8_t>(size >> 24 & 0xFF);
            previousTagSize[1] = static_cast<uint8_t>(size >> 16 & 0xFF);
            previousTagSize[2] = static_cast<uint8_t>(size >> 8 & 0xFF);
            previousTagSize[3] = static_cast<uint8_t>(size & 0xFF);
            flvout_.write(reinterpret_cast<char*>(previousTagSize.data()),4);
            return 0;
    };

    auto atagWriter = [this](const uint8_t* tag, std::uint32_t size, uint32_t pts, uint32_t dts){
            FlvTagCommonHeader ftchdr;
            ftchdr.setTagType(8);
            ftchdr.setDataSize(size);
            ftchdr.setTimestamp(pts);
            auto flvtaghdr = ftchdr.encode();
            flvout_.write(reinterpret_cast<char*>(flvtaghdr.data()),flvtaghdr.size());
            flvout_.write(reinterpret_cast<const char*>(tag),size);
            std::vector<uint8_t> previousTagSize(4,0);
            size += 11;
            previousTagSize[0] = static_cast<uint8_t>(size >> 24 & 0xFF);
            previousTagSize[1] = static_cast<uint8_t>(size >> 16 & 0xFF);
            previousTagSize[2] = static_cast<uint8_t>(size >> 8 & 0xFF);
            previousTagSize[3] = static_cast<uint8_t>(size & 0xFF);
            flvout_.write(reinterpret_cast<char*>(previousTagSize.data()),4);
            return 0;
    };

    if(sf == SoundFormat::AAC)
    {
        atagEncoder_ = std::make_shared<AACAudioTagEncoder>(atagWriter);
    }
    if(cid == CodecID::AVC)
    {
        vtagEncoder_ = std::make_shared<AVCVideoTagEncoder>(vtagWriter);
    }
}

FlvFileWriter::~FlvFileWriter()
{
    flvout_.close();
}

int FlvFileWriter::writeVideo(const uint8_t* videoFrame,const std::size_t size,uint32_t pts,uint32_t dts)
{
    if(!vtagEncoder_)
    {
        return -1;
    }

    return vtagEncoder_->encode(videoFrame,size,pts,dts);
}

int FlvFileWriter::writeAudio(const uint8_t* audioFrame,const std::size_t size,uint32_t pts,uint32_t dts)
{
    if(!atagEncoder_)
    {
        return -1;
    }

    return atagEncoder_->encodeOne(audioFrame,size,pts,dts);
}


FlvFileReader::FlvFileReader(const std::string& filename)
    :FlvFile(filename)
    ,flvin_(filename_,std::ifstream::in | std::ifstream::binary )
{

}

FlvFileReader::~FlvFileReader()
{
    if(flvin_.is_open())
        flvin_.close();
}

void FlvFileReader::startRead(OnFlvFileReader onReader)
{
    if(!tagDecoder_)
    {
        auto tagcb = [onReader](TagType ttype,uint8_t codecid,uint8_t* frame,std::size_t len,uint32_t pts,uint32_t dts){
            onReader((int)ttype,codecid,frame,len,pts,dts);
        };                    
        tagDecoder_ = std::make_shared<TagDecoder>(tagcb);
    }
    std::vector<uint8_t> flvHeader;
    flvHeader.resize(9);
    flvin_.read(reinterpret_cast<char*>(flvHeader.data()),flvHeader.size());
    FlvHeader flvhdr;
    if(flvhdr.decode(flvHeader.data(),flvHeader.size()) == -1)
        return;
    uint32_t previousTagSize0 = 0;
    flvin_.read(reinterpret_cast<char*>(&previousTagSize0), 4);
    while(!flvin_.eof())
    {
        std::vector<uint8_t> flvTag;
        flvTag.resize(11);
        flvin_.read(reinterpret_cast<char*>(flvTag.data()),flvTag.size());
        FlvTagCommonHeader commHdr;
        commHdr.decode(flvTag.data(),flvTag.size());
        auto tagSize = commHdr.getDataSize();
        auto dts     = commHdr.getTimeStamp();
        std::vector<uint8_t> tag(tagSize,0);
        flvin_.read(reinterpret_cast<char*>(tag.data()),tag.size());
        if(commHdr.tagType == 8)
        {
            tagDecoder_->inputAudioData(tag.data(),tag.size(),dts);
        }
        else if(commHdr.tagType == 9)
        {
            tagDecoder_->inputVideoData(tag.data(),tag.size(),dts);
        }
        uint32_t previousTagSize = 0;
        flvin_.read(reinterpret_cast<char*>(&previousTagSize), 4);
    }
    return;
}








}

}

















