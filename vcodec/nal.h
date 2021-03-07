#ifndef NAL_H
#define NAL_H

namespace Format
{
namespace vcodec
{

enum H264_NAL_Type : uint8_t
{
    H264_Unspecified         = 0,
    H264_SliceNoPartition    = 1,
    H264_SlicePartitionA     = 2,
    H264_SlicePartitionB     = 3,
    H264_SlicePartitionC     = 4,
    H264_SliceIDR            = 5,
    H264_SEI                 = 6,
    H264_SPS                 = 7,
    H264_PPS                 = 8,
    H264_AUD                 = 9,
    H264_EndSequence         = 10,
    H264_EndStream           = 11,
    H264_FillerData          = 12,
};

enum H265_NAL_Type : uint8_t
{
    //non IDR 
    H265_Slice_TRAIL_N = 0,
    H265_SLICE_TRAIL_R,
    H265_SLICE_TSA_N,
    H265_SLICE_TSA_R,
    H265_SLICE_STSA_N,
    H265_SLICE_STSA_R,
    H265_SLICE_RADL_N,
    H265_SLICE_RADL_R,
    H265_SLICE_RASL_N,
    H265_SLICE_RASL_R,

    //IDR
    H265_SLICE_BLA_W_LP = 16,
    H265_SLICE_BLA_W_RADL,
    H265_SLICE_BLA_N_LP,
    H265_SLICE_IDR_W_RADL,
    H265_SLICE_IDR_N_LP,
    H265_SLICE_CRA,

    //vps pps sps
    H265_VPS = 32,
    H265_SPS = 33,
    H265_PPS = 34,
    H265_AUD = 35,

    //SEI
    H265_SEI = 39,
    H265_SEI_SUFFIX,
};
















}

}







#endif

