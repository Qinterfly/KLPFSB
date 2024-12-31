#pragma once

#include <string>
#include <stdint.h>

struct Fsb5Header {
    int total_subsongs;
    int version;
    int codec;
    int flags;

    int channels;
    int layers;
    int sample_rate;
    int32_t num_samples;
    int32_t loop_start;
    int32_t loop_end;
    int loop_flag;

    uint32_t sample_header_size;
    uint32_t name_table_size;
    uint32_t sample_data_size;
    uint32_t base_header_size;

    uint32_t extradata_offset;
    uint32_t extradata_size;

    uint32_t stream_offset;
    uint32_t stream_size;
    uint32_t name_offset;

    int cursong;
    uint64_t* ptrOffset;
    uint64_t* ptrStart;
    char songName[256];
    uint32_t* ss;
    uint32_t bitrate;
};

int parseFsb(unsigned char* sf, Fsb5Header** fsbSet);
void setOffset(uint64_t* ptr, uint32_t offset);
void setNumSamples(uint64_t* ptr, uint32_t numSamples);
uint32_t getOffset(uint64_t* ptr);