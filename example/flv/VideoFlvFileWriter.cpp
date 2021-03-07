#include <iostream>
#include <fstream>
#include "flv/file.h"
#include "vcodec/vcodecutil.h"

using namespace Format::vcodec;
using namespace Format::Flv;

int main(int argc,char* argv[])
{
    if(argc < 2)
    {
        std::cout<<"Usage: example <h264file>"<<std::endl;
        return 0;
    }

    std::ifstream in(argv[1]);
    in.seekg(0,in.end);
    int length = in.tellg();
    in.seekg(0,in.beg);

    char * buffer = new char [length];
    in.read (buffer,length);
    in.close();

    FlvFileWriter ffw("test.flv",CodecID::AVC,SoundFormat::NONE);
    
    uint32_t pts = 0;
    splitStream((uint8_t*)buffer,(uint8_t*)buffer + length,[&](const uint8_t* nalu,size_t size){
        std::cout<<"nalu size"<<size<<std::endl;
        ffw.writeVideo(nalu,size,pts,pts);
        if( (nalu[getStartCodeLen(nalu)] & 0x1F) == 7 || (nalu[getStartCodeLen(nalu)] &0x1f) == 8)
            return 0;
        pts += 40;
        return 0;
    });
    
    return 0;
}




