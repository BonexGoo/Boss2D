#include <boss.h>

#if !defined(BOSS_NEED_ADDON_OGG) || (BOSS_NEED_ADDON_OGG != 0 && BOSS_NEED_ADDON_OGG != 1)
    #error BOSS_NEED_ADDON_OGG macro is invalid use
#endif
bool __LINK_ADDON_OGG__() {return true;} // 링크옵션 /OPT:NOREF가 안되서 임시코드

#if BOSS_NEED_ADDON_OGG

#include "boss_addon_ogg.hpp"

#include <boss.hpp>
#include <platform/boss_platform.hpp>

// 등록과정
namespace BOSS
{
    BOSS_DECLARE_ADDON_FUNCTION(Ogg, ToPcmStream, void, id_file_read, AddOn::Ogg::PcmCreateCB, AddOn::Ogg::PcmWriteCB, payload)

    static autorun Bind_AddOn_Ogg()
    {
        Core_AddOn_Ogg_ToPcmStream() = Customized_AddOn_Ogg_ToPcmStream;
        return true;
    }
    static autorun _ = Bind_AddOn_Ogg();
}

// 구현부
namespace BOSS
{
    void Customized_AddOn_Ogg_ToPcmStream(id_file_read oggfile, AddOn::Ogg::PcmCreateCB ccb, AddOn::Ogg::PcmWriteCB wcb, payload data)
    {
        ogg_int16_t convbuffer[4096];
        int convsize = 4096;
        ogg_sync_state   oy;
        ogg_stream_state os;
        ogg_page         og;
        ogg_packet       op;
        vorbis_info      vi;
        vorbis_comment   vc;
        vorbis_dsp_state vd;
        vorbis_block     vb;
        char* _buffer = nullptr;
        int _bytes = 0;

        ogg_sync_init(&oy);
        while(true)
        {
            int eos = 0;
            _buffer = ogg_sync_buffer(&oy, 4096);
            _bytes = Platform::File::Read(oggfile, (uint08*) _buffer, 4096);
            ogg_sync_wrote(&oy, _bytes);

            if(ogg_sync_pageout(&oy, &og) != 1)
            {
                if(_bytes < 4096) break;
                return;
            }

            ogg_stream_init(&os, ogg_page_serialno(&og));
            vorbis_info_init(&vi);
            vorbis_comment_init(&vc);

            if(ogg_stream_pagein(&os, &og) < 0) return;
            if(ogg_stream_packetout(&os, &op) != 1) return;
            if(vorbis_synthesis_headerin(&vi, &vc, &op) < 0) return;
            ccb(vi.channels, vi.rate, 16, data);

            int i = 0;
            while(i < 2)
            {
                while(i < 2)
                {
                    int result = ogg_sync_pageout(&oy, &og);
                    if(result == 0) break;
                    if(result == 1)
                    {
                        ogg_stream_pagein(&os, &og);
                        while(i < 2)
                        {
                            result = ogg_stream_packetout(&os, &op);
                            if(result == 0) break;
                            if(result < 0) return;
                            result = vorbis_synthesis_headerin(&vi, &vc, &op);
                            if(result < 0) return;
                            i++;
                        }
                    }
                }
                _buffer = ogg_sync_buffer(&oy, 4096);
                _bytes = Platform::File::Read(oggfile, (uint08*) _buffer, 4096);
                if(_bytes == 0 && i < 2) return;
                ogg_sync_wrote(&oy, _bytes);
            }

            int volume = 0;
            convsize = 4096 / vi.channels;
            if(vorbis_synthesis_init(&vd, &vi) == 0)
            {
                vorbis_block_init(&vd, &vb);
                while(!eos)
                {
                    while (!eos)
                    {
                        int result = ogg_sync_pageout(&oy, &og);
                        if(result == 0) break;
                        if(result < 0) return;
                        else
                        {
                            ogg_stream_pagein(&os, &og);
                            while(true)
                            {
                                result = ogg_stream_packetout(&os, &op);
                                if(result == 0) break;
                                if(0 <= result)
                                {
                                    float **pcm;
                                    int samples;
                                    if(vorbis_synthesis(&vb, &op) == 0)
                                        vorbis_synthesis_blockin(&vd, &vb);
                                    while((samples = vorbis_synthesis_pcmout(&vd, &pcm)) > 0)
                                    {
                                        int j;
                                        int clipflag = 0;
                                        int bout = (samples < convsize ? samples : convsize);
                                        for(i = 0; i < vi.channels; i++)
                                        {
                                            ogg_int16_t *ptr = convbuffer + i;
                                            float *mono = pcm[i];
                                            for(j = 0; j < bout; j++)
                                            {
                                                int val = (((int) Math::Floor(mono[j] * 32767.f + .5f)) * volume) >> 8;
                                                if(val > 32767)
                                                {
                                                    val = 32767;
                                                    clipflag = 1;
                                                }
                                                if(val < -32768)
                                                {
                                                    val = -32768;
                                                    clipflag = 1;
                                                }
                                                *ptr = val;
                                                ptr += vi.channels;
                                            }
                                        }
                                        volume = wcb(convbuffer, 2 * vi.channels * bout, data);
                                        if(volume < 0)
                                        {
                                            ogg_sync_clear(&oy);
                                            return;
                                        }
                                        vorbis_synthesis_read(&vd, bout);
                                    }
                                }
                            }
                            if(ogg_page_eos(&og))
                                eos = 1;
                        }
                    }
                    if(!eos)
                    {
                        _buffer = ogg_sync_buffer(&oy, 4096);
                        _bytes = Platform::File::Read(oggfile, (uint08*) _buffer, 4096);
                        ogg_sync_wrote(&oy, _bytes);
                        if(_bytes == 0) eos = 1;
                    }
                }
                vorbis_block_clear(&vb);
                vorbis_dsp_clear(&vd);
            }
            ogg_stream_clear(&os);
            vorbis_comment_clear(&vc);
            vorbis_info_clear(&vi);
        }
        ogg_sync_clear(&oy);
    }
}

#endif
