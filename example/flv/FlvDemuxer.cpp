#include <iostream>
#include <fstream>
#include "flv/file.h"

using namespace Format::Flv;

int main(int argc,char* argv[])
{
    if(argc < 2)
    {
        std::cout<<"Usage: exampleFlvDemuxer <Flv File>"<<std::endl;
        return 0;
    }

    std::ofstream vfile("v.h264",std::ofstream::binary);
    std::ofstream afile("a.aac",std::ofstream::binary);

    FlvFileReader ffr(argv[1]);
    ffr.startRead([&](int type,int codecid,uint8_t* frame,std::size_t len,uint32_t pts, uint32_t dts){
        
        std::cout<<"type:["<<type<<"] "<<"codec id:["<<codecid<<"] frame len:["<<len<<"] pts:["<<pts<<"] dts:["<<dts<<"]"<<std::endl; 
        if(type == 9)
        {
            vfile.write((char*)frame,len);
        }
        else if(type == 8)
        {
            afile.write((char*)frame,len);
        }
    }); 
    return 0;
}
