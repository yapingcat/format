#include <iostream>
#include <fstream>
#include "flv/file.h"
#include "AAC.h"

uint32_t detal[3] = {23,23,24};

using namespace Format::Flv;
using namespace Format::acodec;

int main(int argc,char* argv[])
{
    if(argc < 2)
    {
        std::cout<<"Usage: example <AAC File>"<<std::endl;
        return 0;
    }

    std::ifstream in(argv[1]);
    in.seekg(0,in.end);
    int length = in.tellg();
    in.seekg(0,in.beg);

    char * buffer = new char [length];
    in.read (buffer,length);
    in.close();

    FlvFileWriter ffw("testaudio.flv",CodecID::NONE,SoundFormat::AAC);
    
    uint32_t pts = 0;
    splitAACFrame((uint8_t*)buffer,(uint8_t*)buffer + length,[&](const uint8_t* nalu,size_t size){
        static int idx = 0;
        std::cout<<"aac frame size "<<size <<" i:"<< idx <<std::endl;
        ffw.writeAudio(nalu,size,pts,pts);
        pts += detal[idx++ % 3];
        return 0;
    });
    
    return 0;
}
