
#pragma once
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "fsb5.h"

typedef uint32_t off_t2;
typedef unsigned char STREAMFILE;

static inline int8_t   read_s8(off_t2 offset, STREAMFILE* sf) { return      (int8_t)(*(int8_t*)&sf[offset]); }
static inline uint8_t  read_u8(off_t2 offset, STREAMFILE* sf) { return     (uint8_t)(*(uint8_t*)&sf[offset]); }
static inline int16_t  read_s16le(off_t2 offset, STREAMFILE* sf) { return  (int16_t)(*(int16_t*)&sf[offset]); }
static inline uint16_t read_u16le(off_t2 offset, STREAMFILE* sf) { return (uint16_t)(*(uint16_t*)&sf[offset]); }
static inline int16_t  read_s16be(off_t2 offset, STREAMFILE* sf) { return  (int16_t)(*(int16_t*)&sf[offset]); }
static inline uint16_t read_u16be(off_t2 offset, STREAMFILE* sf) { return (uint16_t)(*(uint16_t*)&sf[offset]); }
static inline int32_t  read_s32le(off_t2 offset, STREAMFILE* sf) { return  (int32_t)(*(int32_t*)&sf[offset]); }
static inline uint32_t read_u32le(off_t2 offset, STREAMFILE* sf) { return (uint32_t)(*(uint32_t*)&sf[offset]); }
static inline int32_t  read_s32be(off_t2 offset, STREAMFILE* sf) { return  (int32_t)(*(int32_t*)&sf[offset]); }
static inline uint32_t read_u32be(off_t2 offset, STREAMFILE* sf) { return (uint32_t)(*(uint32_t*)&sf[offset]); }
static inline uint64_t read_u64le(off_t2 offset, STREAMFILE* sf) { return (uint64_t)(*(uint64_t*)&sf[offset]); }

/* ********************************************************************************** */
/* FSB5 - Firelight's FMOD Studio SoundBank format */
int parseFsb(unsigned char *sf, Fsb5Header** fsbSet) {

    Fsb5Header fsb5 = {0};
    uint32_t offset;
    int target_subsong = 2;
    int i;
    int counter=0;

    /* v0 is rare, seen in Tales from Space (Vita) */
    fsb5.version = read_u32le(0x04,sf);
    if (fsb5.version != 0x00 && fsb5.version != 0x01)
        goto fail;

    fsb5.total_subsongs     = read_u32le(0x08,sf);

    *fsbSet = new Fsb5Header[fsb5.total_subsongs];

    fsb5.sample_header_size = read_u32le(0x0C,sf);
    fsb5.name_table_size    = read_u32le(0x10,sf);
    fsb5.sample_data_size   = read_u32le(0x14,sf);
    fsb5.ss                 = (uint32_t *) & sf[0x14];
    fsb5.codec              = read_u32le(0x18,sf);
    /* 0x1c: zero */
    if (fsb5.version == 0x01) {
        fsb5.flags = read_u32le(0x20,sf); /* found by tests and assumed to be flags, no games known */
        /* 0x24: 128-bit hash */
        /* 0x34: unknown (64-bit sub-hash?) */
        fsb5.base_header_size = 0x3c;
    }
    else {
        /* 0x20: zero/flags? */
        /* 0x24: zero/flags? */
        /* 0x28: 128-bit hash */
        /* 0x38: unknown (64-bit sub-hash?) */
        fsb5.base_header_size = 0x40;
    }

    if (target_subsong == 0) target_subsong = 1;
    if (target_subsong > fsb5.total_subsongs || fsb5.total_subsongs <= 0) goto fail;

    /* find target stream header and data offset, and read all needed values for later use
     *  (reads one by one as the size of a single stream header is variable) */
    offset = fsb5.base_header_size;
    for (i = 0; i < fsb5.total_subsongs; i++) {
        uint32_t stream_header_size = 0;
        uint32_t data_offset = 0;
        uint64_t sample_mode;

        sample_mode = read_u64le(offset+0x00,sf);
        stream_header_size += 0x08;

        /* get samples */
        fsb5.num_samples  = ((sample_mode >> 34) & 0x3FFFFFFF); /* bits: 63..34 (30) */

        /* get offset inside data section (max 32b offset 0xFFFFFFE0) */
        data_offset   =  ((sample_mode >> 7) & 0x07FFFFFF) << 5; /* bits: 33..8 (25) */

        /* get channels */
        switch ((sample_mode >> 5) & 0x03) { /* bits: 7..6 (2) */
            case 0:  fsb5.channels = 1; break;
            case 1:  fsb5.channels = 2; break;
            case 2:  fsb5.channels = 6; break; /* some Dark Souls 2 MPEG; some IMA ADPCM */
            case 3:  fsb5.channels = 8; break; /* some IMA ADPCM */
            /* other channels (ex. 4/10/12ch) use 0 here + set extra flags */
            default: /* not possible */
                goto fail;
        }

        /* get sample rate  */
        switch ((sample_mode >> 1) & 0x0f) { /* bits: 5..1 (4) */
            case 0:  fsb5.sample_rate = 4000;  break;
            case 1:  fsb5.sample_rate = 8000;  break;
            case 2:  fsb5.sample_rate = 11000; break;
            case 3:  fsb5.sample_rate = 11025; break;
            case 4:  fsb5.sample_rate = 16000; break;
            case 5:  fsb5.sample_rate = 22050; break;
            case 6:  fsb5.sample_rate = 24000; break;
            case 7:  fsb5.sample_rate = 32000; break;
            case 8:  fsb5.sample_rate = 44100; break;
            case 9:  fsb5.sample_rate = 48000; break;
            case 10: fsb5.sample_rate = 96000; break;
            /* other sample rates (ex. 3000/64000/192000) use 0 here + set extra flags */
            default: /* 11-15: rejected (FMOD error) */
                goto fail;
        }

        /* get extra flags */
        if (sample_mode & 0x01) { /* bits: 0 (1) */
            uint32_t extraflag_offset = offset + 0x08;
            uint32_t extraflag, extraflag_type, extraflag_size, extraflag_end;

            do {
                extraflag = read_u32le(extraflag_offset,sf);
                extraflag_type = (extraflag >> 25) & 0x7F; /* bits 32..26 (7) */
                extraflag_size = (extraflag >> 1) & 0xFFFFFF; /* bits 25..1 (24)*/
                extraflag_end  = (extraflag & 0x01); /* bit 0 (1) */

                /* parse target only, as flags change between subsongs */
                //if (i + 1 == target_subsong) {
                if (1 == 1) {
                    switch(extraflag_type) {
                        case 0x01:  /* channels */
                            fsb5.channels = read_u8(extraflag_offset+0x04,sf);
                            break;
                        case 0x02:  /* sample rate */
                            fsb5.sample_rate = read_s32le(extraflag_offset+0x04,sf);
                            break;
                        case 0x03:  /* loop info */
                            fsb5.loop_start = read_s32le(extraflag_offset+0x04,sf);
                            if (extraflag_size > 0x04) { /* probably not needed */
                                fsb5.loop_end = read_s32le(extraflag_offset+0x08,sf);
                                fsb5.loop_end += 1; /* correct compared to FMOD's tools */
                            }
                            //;VGM_LOG("FSB5: stream %i loop start=%i, loop end=%i, samples=%i\n", i, fsb5.loop_start, fsb5.loop_end, fsb5.num_samples);

                            /* autodetect unwanted loops */
                            {
                                /* like FSB4 jingles/sfx/music do full loops for no reason, but happens a lot less.
                                 * Most songs loop normally now with proper values [ex. Shantae, FFX] */
                                int full_loop, ajurika_loops, is_small;

                                /* disable some jingles, it's even possible one jingle (StingerA Var1) to not have loops
                                 * and next one (StingerA Var2) do [Sonic Boom Fire & Ice (3DS)] */
                                full_loop = fsb5.loop_start == 0 && fsb5.loop_end + 1152 >= fsb5.num_samples; /* around ~15 samples less, ~1000 for MPEG */
                                /* a few longer Sonic songs shouldn't repeat */
                                is_small = 1; //fsb5.num_samples < 20 * fsb5.sample_rate;

                                /* wrong values in some files [Pac-Man CE2 Plus (Switch) pce2p_bgm_ajurika_*.fsb] */
                                ajurika_loops = fsb5.loop_start == 0x3c && fsb5.loop_end == (0x007F007F + 1) &&
                                        fsb5.num_samples > fsb5.loop_end + 10000; /* arbitrary test in case some game does have those */

                                fsb5.loop_flag = 1;
                                if ((full_loop && is_small) || ajurika_loops) {
                                    //VGM_LOG("FSB5: stream %i disabled unwanted loop ls=%i, le=%i, ns=%i\n", i, fsb5.loop_start, fsb5.loop_end, fsb5.num_samples);
                                    fsb5.loop_flag = 0;
                                }
                            }
                            break;
                        case 0x04:  /* free comment, or maybe SFX info */
                            break;
                        case 0x05:  /* unknown 32b */
                            /* rare, found in Tearaway (Vita) with value 0 in first stream and
                             * Shantae and the Seven Sirens (Mobile) with value 0x0003bd72 BE in #44 (Arena Town),
                             * also in SMT3 Remaster same as loop start (cue to jump to next segment?) */
                            //VGM_LOG("FSB5: stream %i flag %x with value %08x\n", i, extraflag_type, read_u32le(extraflag_offset+0x04,sf));
                            break;
                        case 0x06:  /* XMA seek table */
                            /* no need for it */
                            break;
                        case 0x07:  /* DSP coefs */
                            fsb5.extradata_offset = extraflag_offset + 0x04;
                            break;
                        case 0x09:  /* ATRAC9 config */
                            fsb5.extradata_offset = extraflag_offset + 0x04;
                            fsb5.extradata_size = extraflag_size;
                            break;
                        case 0x0a:  /* XWMA config */
                            fsb5.extradata_offset = extraflag_offset + 0x04;
                            break;
                        case 0x0b:  /* Vorbis setup ID and seek table */
                            fsb5.extradata_offset = extraflag_offset + 0x04;
                            /* seek table format:
                             * 0x08: table_size (total_entries = seek_table_size / (4+4)), not counting this value; can be 0
                             * 0x0C: sample number (only some samples are saved in the table)
                             * 0x10: offset within data, pointing to a FSB vorbis block (with the 16b block size header)
                             * (xN entries) */
                            break;
                        case 0x0d:  /* peak volume float (optional setting when making fsb) */
                            break;
                        case 0x0f:  /* OPUS data size not counting frames headers */
                            break;
                        case 0x0e:  /* Vorbis intra-layers (multichannel FMOD ~2021) [Invisible, Inc. (Switch), Just Cause 4 (PC)] */
                            fsb5.layers = read_u32le(extraflag_offset+0x04,sf);
                            /* info only as decoding is standard Vorbis that handles Nch multichannel (channels is 1 here) */
                            fsb5.channels = fsb5.channels * fsb5.layers;
                            break;
                        default:
                            //vgm_logi("FSB5: stream %i unknown flag 0x%x at %x + 0x04 + 0x%x (report)\n", i, extraflag_type, extraflag_offset, extraflag_size);
                            break;
                    }
                }

                extraflag_offset += 0x04 + extraflag_size;
                stream_header_size += 0x04 + extraflag_size;
            }
            while (extraflag_end != 0x00);
        }

        /* target found */
        fsb5.stream_offset = fsb5.base_header_size + fsb5.sample_header_size + fsb5.name_table_size + data_offset;

        /* get stream size from next stream offset or full size if there is only one */
        if (i + 1 == fsb5.total_subsongs) {
            fsb5.stream_size = fsb5.sample_data_size - data_offset;
            fsb5.ptrOffset = NULL;
        }
        else {
            uint32_t next_data_offset;
            uint64_t next_sample_mode;
            next_sample_mode = read_u64le(offset + stream_header_size + 0x00, sf);
            next_data_offset = ((next_sample_mode >> 7) & 0x07FFFFFF) << 5;
            fsb5.ptrOffset = (uint64_t*)&sf[offset + stream_header_size + 0x00];
            fsb5.stream_size = next_data_offset - data_offset;
        }
        fsb5.ptrStart = (uint64_t*)&sf[offset + 0x00];
        fsb5.cursong = i + 1;

        /* get stream name */
        if (fsb5.name_table_size > 0) {
            //off_t2 name_suboffset = fsb5.base_header_size + fsb5.sample_header_size + 0x04 * (target_subsong - 1);
            off_t2 name_suboffset = fsb5.base_header_size + fsb5.sample_header_size + 0x04 * (i);
            fsb5.name_offset = fsb5.base_header_size + fsb5.sample_header_size + read_u32le(name_suboffset, sf);
           /* auto dst = std::make_unique<char[]>(std::strlen(src) + 1);*/
            strcpy_s(fsb5.songName, sizeof(fsb5.songName), (char*)&sf[fsb5.name_offset]);
        }
        fsb5.bitrate = 8 * fsb5.stream_size * fsb5.sample_rate / fsb5.num_samples;
        (*fsbSet)[counter] = fsb5;
        counter++;

        /* continue searching target */
        offset += stream_header_size;
    }

    if (!fsb5.stream_offset || !fsb5.stream_size)
        goto fail;
    return 0;
fail:
    return 1;
}

void setOffset(uint64_t* ptr, uint32_t offset)
{
    offset = ((offset - 1) / 32 + 1) * 32;
    *ptr = (*ptr & 0xFFFFFFFC0000007F) | ((uint64_t)(offset >> 5) << 7);
}

void setNumSamples(uint64_t* ptr, uint32_t numSamples)
{
    *ptr = (*ptr & 0x00000003FFFFFFFF) | ((uint64_t)numSamples << 34);
}

uint32_t getOffset(uint64_t* ptr)
{
    return ((*ptr >> 7) & 0x07FFFFFF) << 5;
}