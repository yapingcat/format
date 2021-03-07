#ifndef FLVFILE_H
#define FLVFILE_H

#include <string>
#include <cstdint>
#include <fstream>
#include <memory>

#include "tagencoder.h"
#include "tagdecoder.h"

namespace Format
{
namespace Flv
{
    
class FlvFile
{
public:
    FlvFile(const std::string& filename);
    virtual ~FlvFile() = default;

protected:
    std::string filename_ = "";
};


class FlvFileWriter : public FlvFile
{
public:
    FlvFileWriter(const std::string& filename,CodecID cid, SoundFormat sf);
    ~FlvFileWriter();

public:

    int writeVideo(const uint8_t* videoFrame,const std::size_t size,uint32_t pts,uint32_t dts);

    int writeAudio(const uint8_t* audioFrame,const std::size_t size,uint32_t pts,uint32_t dts);

private:
    std::ofstream flvout_;
    std::shared_ptr<TagEncoder> vtagEncoder_;
    std::shared_ptr<TagEncoder> atagEncoder_;
};

//std::function<void(int,int,uint8_t*,std::size_t,uint32_t ,uint32_t)>
//callback param 
//int          - frame type 8-audio 9-video 18-script data
//int          - codec id, for video,7-h264,12-h265  for audio 10-aac 7-g711a 8-g711u
//uint8_t*     - frame buf
//std::size_t  - frame length
//uint32_t     - pts
//uint32_t     - dts
using OnFlvFileReader = std::function<void(int,int,uint8_t*,std::size_t,uint32_t ,uint32_t)>;
 
class FlvFileReader : public FlvFile
{
public:
    FlvFileReader(const std::string& filename);
    ~FlvFileReader();

public:

    void startRead(OnFlvFileReader onReader);

private:
    std::ifstream flvin_;
    std::shared_ptr<TagDecoder> tagDecoder_;
};


}
}







#endif
