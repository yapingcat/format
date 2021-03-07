#ifndef FORMAT_PES_H
#define FORMAT_PES_H


namespace Format
{

namespace TS
{

struct PesPacket
{
uint8_t packet_start_code_prefix[3];
uint8_t stream_id;
uint16_t pes_packet_length;


};







}

}
