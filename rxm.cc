#include <SDL/SDL.h>
#include <malloc.h>
#include "bass/include/bass.h"
#include "rxm.h"

static HMUSIC current_song;

#pragma pack(1)

typedef struct __attribute__ ((__packed__)) {
 uint32_t hdSonglen;                //Song length (in patten order table)
 uint32_t hdRestart;                //Restart position
 uint32_t hdChannels;               //Number of channels (2,4,6,8,10,...,32)
 uint32_t hdPatterns;               //Number of patterns (max 256)
 uint32_t hdInstruments;            //Number of instruments (max 128)
 uint32_t hdSpeed;                  //Default tempo
 uint32_t hdBPM;                    //Default BPM
 uint8_t  hdFlags;                  //Flags: bit 0: 0 = Amiga, 1 = Linear
 uint8_t  hdPtable[256];            //Pattern order table
} _rxmheader;

typedef struct __attribute__ ((__packed__)) {
 char     ID_text[17];
 char     Module_name[20];
 uint8_t  x1A;
 char     Tracker_name[20];
 uint16_t Version_number;
 uint32_t Header_size;
 uint16_t Song_length;              //Song length (in patten order table)
 uint16_t Restart_position;         //Restart position
 uint16_t Number_of_channels;       //Number of channels (2,4,6,8,10,...,32)
 uint16_t Number_of_patterns;       //Number of patterns (max 256)
 uint16_t Number_of_instruments;    //Number of instruments (max 128)
 uint16_t Flags;                    //Flags: bit 0: 0 = Amiga, 1 = Linear
 uint16_t Default_tempo;            //Default tempo
 uint16_t Default_BPM;              //Default BPM
 uint8_t  Patern_order_table[256];  //Pattern order table
} _xmheader;

typedef struct __attribute__ ((__packed__)) {
 uint32_t Pattern_header_length;
 uint8_t  Packing_type;
 uint16_t Number_of_rows_in_pattern;
 uint16_t Packed_pattern_data_size;
} _xmpatternheader;

typedef struct __attribute__ ((__packed__)) {
 uint32_t Instrument_size;
 char     Instrument_name[22];
 uint8_t  Instrument_type;
 uint16_t Number_of_samples;
} _xminstrumentheader1;

typedef struct __attribute__ ((__packed__)) {
 uint32_t Sample_header_size;
 uint8_t  Sample_keymap_assignments[96];
 uint16_t Points_for_volume_envelope[24];
 uint16_t Points_for_panning_envelope[24];
 uint8_t  Number_of_volume_points;
 uint8_t  Number_of_panning_points;
 uint8_t  Volume_sustain_point;
 uint8_t  Volume_loop_start_point;
 uint8_t  Volume_loop_end_point;
 uint8_t  Panning_sustain_point;
 uint8_t  Panning_loop_start_point;
 uint8_t  Panning_loop_end_point;
 uint8_t  Volume_type;
 uint8_t  Panning_type;
 uint8_t  Vibrato_type;
 uint8_t  Vibrato_sweep;
 uint8_t  Vibrato_depth;
 uint8_t  Vibrato_rate;
 uint16_t Volume_fadeout;
 uint16_t Reserved[11];
} _xminstrumentheader2;

typedef struct __attribute__ ((__packed__)) {
 uint32_t Sample_length;
 uint32_t Sample_loop_start;
 uint32_t Sample_loop_length;
 uint8_t  Volume;
 int8_t   Finetune;
 uint8_t  Type;
 uint8_t  Panning;
 int8_t   Relative_note_number;
 uint8_t  Reserved;
 char     Sample_name[22];
} _xmsampleheader;

#pragma pack()


static void convert_pattern(uint8_t *pattern, int number_of_notes, int packed_size, uint8_t *out, int *out_size)
{
    uint8_t data[6];
    uintptr_t orig_out;

    *out_size = 0;
    orig_out = (uintptr_t) out;

    for (; number_of_notes > 0 && packed_size > 0; number_of_notes--)
    {
        int num_effects;

        data[0] = 0;
        // silence warning:
        data[1] = 0;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;

        num_effects = *pattern;
        pattern++;
        packed_size--;
        for (; num_effects != 0; num_effects--, pattern+=2, packed_size-=2)
        {
            if (pattern[0] & 0x80)
            {
                // note & instrument
                if (data[0] & 3) return;

                data[0] |= 3;
                data[1] = pattern[0] & 0x7f; // note
                data[2] = pattern[1]; // instrument
            }
            else if (pattern[0] == 36)
            {
                // Key off
                if ((data[0] & 3) || (pattern[1] != 0)) return;

                data[0] |= 1;
                data[1] = 97; // key-off note
            }

            // hopefully this is right
            // used only few times
            //else if ((pattern[0] == 7) && ((pattern[1] & 0x0f) == 0) && !(data[0] & 4)) // Tone porta
            //{
            //    data[0] |= 4;
            //    data[3] = 0xf0 + (pattern[1] >> 4); // volume
            //}
            // unusable without
            else if ((pattern[0] == 12) && (pattern[1] <= 0x40) && !(data[0] & 4)) // Set volume
            {
                data[0] |= 4;
                data[3] = pattern[1] + 0x10; // volume
            }
            // unusable without
            else if ((pattern[0] == 13) && (((int8_t *)pattern)[1] < 0) && (((int8_t *)pattern)[1] >= -15) && !(data[0] & 4)) // Volume slide down
            {
                data[0] |= 4;
                data[3] = 0x60 - ((int8_t *)pattern)[1]; // volume
            }
            // unusable without
            else if ((pattern[0] == 13) && (pattern[1] > 0) && (pattern[1] <= 15) && !(data[0] & 4)) // Volume slide up
            {
                data[0] |= 4;
                data[3] = 0x70 + pattern[1]; // volume
            }

            // NOT used by fulcrum xm's
            //else if ((pattern[0] == 14) && (((int8_t *)pattern)[1] < 0) && (((int8_t *)pattern)[1] >= -15) && !(data[0] & 4)) // Volume slide down
            //{
            //    data[0] |= 4;
            //    data[3] = 0x60 - ((int8_t *)pattern)[1]; // volume
            //}
            // NOT used by fulcrum xm's
            //else if ((pattern[0] == 14) && (pattern[1] >= 0) && (pattern[1] <= 15) && !(data[0] & 4)) // Volume slide up
            //{
            //    data[0] |= 4;
            //    data[3] = 0x70 + pattern[1]; // volume
            //}
            // unusable without
            else if ((pattern[0] == 15) && (((int8_t *)pattern)[1] < 0) && (((int8_t *)pattern)[1] >= -15) && !(data[0] & 4)) // Fine volume slide down
            {
                data[0] |= 4;
                data[3] = 0x80 - ((int8_t *)pattern)[1]; // volume
            }
            // unusable without
            else if ((pattern[0] == 15) && (pattern[1] > 0) && (pattern[1] <= 15) && !(data[0] & 4)) // Fine volume slide up
            {
                data[0] |= 4;
                data[3] = 0x90 + pattern[1]; // volume
            }
            // NOT used by fulcrum xm's
            //else if ((pattern[0] == 16) && (pattern[1] > 0) && (pattern[1] <= 15) && !(data[0] & 4)) // Fine volume slide up
            //{
            //    data[0] |= 4;
            //    data[3] = 0x90 + pattern[1]; // volume
            //}
            // NOT used by fulcrum xm's
            //else if ((pattern[0] == 17) && (pattern[1] > 0) && (pattern[1] <= 15) && !(data[0] & 4)) // Fine volume slide down
            //{
            //    data[0] |= 4;
            //    data[3] = 0x80 + pattern[1]; // volume
            //}
            // unusable without
            else if ((pattern[0] == 25) && ((pattern[1] & 0x0f) == (pattern[1] >> 4)) && !(data[0] & 4)) // Set panning
            {
                data[0] |= 4;
                data[3] = 0xc0 + (pattern[1] >> 4); // volume
            }

            else if ((pattern[0] == 1) && !(data[0] & 0x18)) // Porta up
            {
                if (pattern[1] == 0)
                {
                    data[0] |= 0x08;
                    data[4] = 0x01; // effect type
                }
                else
                {
                    data[0] |= 0x18;
                    data[4] = 0x01; // effect type
                    data[5] = pattern[1]; // effect parameter
                }
            }
            else if ((pattern[0] == 2) && !(data[0] & 0x18)) // Porta down
            {
                if (pattern[1] == 0)
                {
                    data[0] |= 0x08;
                    data[4] = 0x02; // effect type
                }
                else
                {
                    data[0] |= 0x18;
                    data[4] = 0x02; // effect type
                    data[5] = pattern[1]; // effect parameter
                }
            }
            else if ((pattern[0] == 3) && (pattern[1] <= 0x10) && !(data[0] & 0x18)) // Fine porta up
            {
                data[0] |= 0x18;
                data[4] = 0x0e; // effect type
                data[5] = 0x10 | pattern[1]; // effect parameter
            }
            else if ((pattern[0] == 7) && !(data[0] & 0x18)) // Tone porta
            {
                if (pattern[1] == 0)
                {
                    data[0] |= 0x08;
                    data[4] = 0x03; // effect type
                }
                else
                {
                    data[0] |= 0x18;
                    data[4] = 0x03; // effect type
                    data[5] = pattern[1]; // effect parameter
                }
            }
            else if ((pattern[0] == 12) && (pattern[1] <= 0x40) && !(data[0] & 0x18)) // Set volume
            {
                if (pattern[1] == 0)
                {
                    data[0] |= 0x08;
                    data[4] = 0x0c; // effect type
                }
                else
                {
                    data[0] |= 0x18;
                    data[4] = 0x0c; // effect type
                    data[5] = pattern[1]; // effect parameter
                }
            }
            else if ((pattern[0] == 13) && (((int8_t *)pattern)[1] < 0) && (((int8_t *)pattern)[1] >= -15) && !(data[0] & 0x18)) // Volume slide down
            {
                data[0] |= 0x18;
                data[4] = 0x0a; // effect type
                data[5] = -((int8_t *)pattern)[1]; // effect parameter
            }
            else if ((pattern[0] == 13) && (pattern[1] >= 1) && (pattern[1] <= 15) && !(data[0] & 0x18)) // Volume slide up
            {
                data[0] |= 0x18;
                data[4] = 0x0a; // effect type
                data[5] = pattern[1] << 4; // effect parameter
            }
            else if ((pattern[0] == 14) && (((int8_t *)pattern)[1] < 0) && (((int8_t *)pattern)[1] >= -15) && !(data[0] & 0x18)) // Volume slide down
            {
                data[0] |= 0x18;
                data[4] = 0x0a; // effect type
                data[5] = -((int8_t *)pattern)[1]; // effect parameter
            }
            else if ((pattern[0] == 14) /*&& (pattern[1] >= 0)*/ && (pattern[1] <= 15) && !(data[0] & 0x18)) // Volume slide up
            {
                if (pattern[1] == 0)
                {
                    data[0] |= 0x08;
                    data[4] = 0x0a; // effect type
                }
                else
                {
                    data[0] |= 0x18;
                    data[4] = 0x0a; // effect type
                    data[5] = pattern[1] << 4; // effect parameter
                }
            }
            else if ((pattern[0] == 15) && (((int8_t *)pattern)[1] < 0) && (((int8_t *)pattern)[1] >= -15) && !(data[0] & 0x18)) // Fine volume slide down
            {
                data[0] |= 0x18;
                data[4] = 0x0e; // effect type
                data[5] = 0xb0 | -((int8_t *)pattern)[1]; // effect parameter
            }
            else if ((pattern[0] == 15) && (pattern[1] > 0) && (pattern[1] <= 15) && !(data[0] & 0x18)) // Fine volume slide up
            {
                data[0] |= 0x18;
                data[4] = 0x0e; // effect type
                data[5] = 0xa0 | pattern[1]; // effect parameter
            }
            else if ((pattern[0] == 16) && (pattern[1] > 0) && (pattern[1] <= 15) && !(data[0] & 0x18)) // Fine volume slide up
            {
                data[0] |= 0x18;
                data[4] = 0x0e; // effect type
                data[5] = 0xa0 | pattern[1]; // effect parameter
            }
            else if ((pattern[0] == 17) && (pattern[1] > 0) && (pattern[1] <= 15) && !(data[0] & 0x18)) // Fine volume slide down
            {
                data[0] |= 0x18;
                data[4] = 0x0e; // effect type
                data[5] = 0xb0 | pattern[1]; // effect parameter
            }
            else if ((pattern[0] == 25) && !(data[0] & 0x18)) // Set panning
            {
                data[0] |= 0x18;
                data[4] = 0x08; // effect type
                data[5] = pattern[1]; // effect parameter
            }
            else if ((pattern[0] == 28) && !(data[0] & 0x18)) // Position jump
            {
                data[0] |= 0x18;
                data[4] = 0x0b; // effect type
                data[5] = pattern[1]; // effect parameter
            }
            else if ((pattern[0] == 29) && !(data[0] & 0x18)) // Pattern break
            {
                if (pattern[1] == 0)
                {
                    data[0] |= 0x08;
                    data[4] = 0x0d; // effect type
                }
                else
                {
                    data[0] |= 0x18;
                    data[4] = 0x0d; // effect type
                    data[5] = pattern[1]; // effect parameter
                }
            }
            else if ((pattern[0] == 32) && (pattern[1] < 0x20) && !(data[0] & 0x18)) // Set tempo
            {
                data[0] |= 0x18;
                data[4] = 0x0f; // effect type
                data[5] = pattern[1]; // effect parameter
            }
            else if ((pattern[0] == 34) && !(data[0] & 0x18)) // Sample offset
            {
                data[0] |= 0x18;
                data[4] = 0x09; // effect type
                data[5] = pattern[1]; // effect parameter
            }
            else if ((pattern[0] == 38) && (pattern[1] < 0x10) && !(data[0] & 0x18)) // Retrig
            {
                data[0] |= 0x18;
                data[4] = 0x0e; // effect type
                data[5] = 0x90 | pattern[1]; // effect parameter
            }
            else if ((pattern[0] == 35) && (pattern[1] < 0x10) && !(data[0] & 0x18)) // Note delay
            {
                data[0] |= 0x18;
                data[4] = 0x0e; // effect type
                data[5] = 0xd0 | pattern[1]; // effect parameter
            }
            else
            {
                return;
            }
        }

        // better mimics original fulcrum xm's
        if ((data[0] == 0x0f) && (data[4] == 0x03))
        {
            data[0] = 0x1f;
            data[5] = 0;
        }

        if (data[0] == 0x1f)
        {
            out[0] = data[1];
            out[1] = data[2];
            out[2] = data[3];
            out[3] = data[4];
            out[4] = data[5];
            out += 5;
        }
        else
        {
            *(out++) = 0x80 | data[0];
            if (data[0] &  1) *(out++) = data[1];
            if (data[0] &  2) *(out++) = data[2];
            if (data[0] &  4) *(out++) = data[3];
            if (data[0] &  8) *(out++) = data[4];
            if (data[0] & 16) *(out++) = data[5];
        }
    }

    *out_size = (uintptr_t)out - orig_out;
}

static void *rxm2xm(void *rxm, int len, uint32_t *xmsize)
{
    void *xm = malloc((int)(len * 1.01f)); // the allocated size should be higher than the resulting xm needs
    if (xm == NULL) return NULL;

    _rxmheader *rxmheader = (_rxmheader *)rxm;
    uint32_t rxmheadersize = sizeof(_rxmheader) - 256 + rxmheader->hdSonglen;

    // prepare standard xm header
    _xmheader *sxmheader = (_xmheader *)xm;

    memcpy(sxmheader->ID_text, "Extended Module: ", 17);
    memset(sxmheader->Module_name, ' ', 20); // pad with spaces
    //memcpy(sxmheader->Module_name, rxmname, strlen(rxmname));
    sxmheader->x1A = 0x1A;
    memcpy(sxmheader->Tracker_name, "FastTracker v2.00   ", 20);
    sxmheader->Version_number = 0x0104;
    sxmheader->Header_size = 20 + 256;
    sxmheader->Song_length = rxmheader->hdSonglen;
    sxmheader->Restart_position = rxmheader->hdRestart;
    sxmheader->Number_of_channels = rxmheader->hdChannels;
    sxmheader->Number_of_patterns = rxmheader->hdPatterns;
    sxmheader->Number_of_instruments = rxmheader->hdInstruments;
    sxmheader->Flags = rxmheader->hdFlags;
    sxmheader->Default_tempo = rxmheader->hdSpeed;
    sxmheader->Default_BPM = rxmheader->hdBPM;
    memset(sxmheader->Patern_order_table, 0, 256); // pad with zeroes
    memcpy(sxmheader->Patern_order_table, rxmheader->hdPtable, sxmheader->Song_length);

    uint8_t *rxmcurptr = (uint8_t *) (rxmheadersize + (uintptr_t)rxm);
    uint8_t *sxmcurptr = (uint8_t *) (sizeof(_xmheader) + (uintptr_t)xm);

    // process patterns
    for (int pattern = 0; pattern < sxmheader->Number_of_patterns; pattern++)
    {
        uint32_t pattern_size = *( (uint32_t *)rxmcurptr );
        uint32_t number_of_rows = *( (uint32_t *)(rxmcurptr + 4) );

        _xmpatternheader *xmpatternheader = (_xmpatternheader *)sxmcurptr;
        sxmcurptr += sizeof(_xmpatternheader);

        xmpatternheader->Pattern_header_length = 9;
        xmpatternheader->Packing_type = 0;
        xmpatternheader->Number_of_rows_in_pattern = number_of_rows;

        if (pattern_size > 8)
        {
            int xmpatternsize;

            convert_pattern(rxmcurptr + 8, number_of_rows * sxmheader->Number_of_channels, pattern_size - 8, sxmcurptr, &xmpatternsize);

            xmpatternheader->Packed_pattern_data_size = xmpatternsize;
            sxmcurptr += xmpatternsize;
        }
        else
        {
            xmpatternheader->Packed_pattern_data_size = 0;
        }

        rxmcurptr += pattern_size;
    }

    // process instruments
    for (int instrument = 0; instrument < sxmheader->Number_of_instruments; instrument++)
    {
        uint32_t number_of_samples = rxmcurptr[0];
        uint32_t instrument_flags = rxmcurptr[1];
        rxmcurptr += 2;

        _xminstrumentheader1 *xminstrumentheader1 = (_xminstrumentheader1 *)sxmcurptr;
        sxmcurptr += sizeof(_xminstrumentheader1);
        memset(xminstrumentheader1->Instrument_name, ' ', 22); // pad with spaces
        xminstrumentheader1->Instrument_type = 0;
        xminstrumentheader1->Number_of_samples = number_of_samples;

        if (number_of_samples > 0)
        {
            xminstrumentheader1->Instrument_size = sizeof(_xminstrumentheader1) + sizeof(_xminstrumentheader2);

            _xminstrumentheader2 *xminstrumentheader2 = (_xminstrumentheader2 *)sxmcurptr;
            sxmcurptr += sizeof(_xminstrumentheader2);
            xminstrumentheader2->Sample_header_size = sizeof(_xmsampleheader);
            memset(xminstrumentheader2->Reserved, 0, 22);

            if (instrument_flags & 1)
            {
                memcpy(xminstrumentheader2->Sample_keymap_assignments, rxmcurptr, 96);
                rxmcurptr += 96;
            }
            else
            {
                memset(xminstrumentheader2->Sample_keymap_assignments, 0, 96);
            }

            memset(xminstrumentheader2->Points_for_volume_envelope, 0, 48); // padd with zeroes
            if (instrument_flags & 2)
            {
                xminstrumentheader2->Number_of_volume_points = rxmcurptr[0] + 1;
                xminstrumentheader2->Volume_type = 1; // on
                if (((int8_t *)rxmcurptr)[1] != -1)
                {
                    xminstrumentheader2->Volume_sustain_point = rxmcurptr[1];
                    xminstrumentheader2->Volume_type |= 2; // sustain
                }
                else
                {
                    xminstrumentheader2->Volume_sustain_point = 0;
                }
                if (((int8_t *)rxmcurptr)[3] != -1)
                {
                    xminstrumentheader2->Volume_loop_start_point = rxmcurptr[2];
                    xminstrumentheader2->Volume_loop_end_point = rxmcurptr[3];
                    xminstrumentheader2->Volume_type |= 4; // loop
                }
                else
                {
                    xminstrumentheader2->Volume_loop_start_point = 0;
                    xminstrumentheader2->Volume_loop_end_point = 0;
                }
                rxmcurptr += 4;

                uint16_t lastvalue = 0;
                for (int point = 0; point < xminstrumentheader2->Number_of_volume_points; point++)
                {
                    uint16_t value;
                    value = *((uint16_t *)&(rxmcurptr[4*point]));

                    lastvalue += value;

                    xminstrumentheader2->Points_for_volume_envelope[2 * point] = lastvalue;
                    xminstrumentheader2->Points_for_volume_envelope[2 * point + 1] = *((uint16_t *)&(rxmcurptr[4*point + 2])) >> 2;
                }

                rxmcurptr += xminstrumentheader2->Number_of_volume_points * 4;

                xminstrumentheader2->Volume_fadeout = *( (uint16_t *)rxmcurptr );
                rxmcurptr += 2;
            }
            else
            {
                xminstrumentheader2->Number_of_volume_points = 0;
                xminstrumentheader2->Volume_sustain_point = 0;
                xminstrumentheader2->Volume_loop_start_point = 0;
                xminstrumentheader2->Volume_loop_end_point = 0;
                xminstrumentheader2->Volume_type = 0;
                xminstrumentheader2->Volume_fadeout = 0x80;
            }

            memset(xminstrumentheader2->Points_for_panning_envelope, 0, 48); // padd with zeroes
            if (instrument_flags & 4)
            {
                xminstrumentheader2->Number_of_panning_points = rxmcurptr[0] + 1;
                xminstrumentheader2->Panning_type = 1; // on
                if (((int8_t *)rxmcurptr)[1] != -1)
                {
                    xminstrumentheader2->Panning_sustain_point = rxmcurptr[1];
                    xminstrumentheader2->Panning_type |= 2; // sustain
                }
                else
                {
                    xminstrumentheader2->Panning_sustain_point = 0;
                }
                if (((int8_t *)rxmcurptr)[3] != -1)
                {
                    xminstrumentheader2->Panning_loop_start_point = rxmcurptr[2];
                    xminstrumentheader2->Panning_loop_end_point = rxmcurptr[3];
                    xminstrumentheader2->Panning_type |= 4; // loop
                }
                else
                {
                    xminstrumentheader2->Panning_loop_start_point = 0;
                    xminstrumentheader2->Panning_loop_end_point = 0;
                }
                rxmcurptr += 4;

                uint16_t lastvalue = 0;
                for (int point = 0; point < xminstrumentheader2->Number_of_panning_points; point++)
                {
                    uint16_t value;
                    value = *((uint16_t *)&(rxmcurptr[4*point]));

                    lastvalue += value;

                    xminstrumentheader2->Points_for_panning_envelope[2 * point] = lastvalue;
                    xminstrumentheader2->Points_for_panning_envelope[2 * point + 1] = *((uint16_t *)&(rxmcurptr[4*point + 2])) >> 2;
                }

                rxmcurptr += xminstrumentheader2->Number_of_panning_points * 4;
            }
            else
            {
                xminstrumentheader2->Number_of_panning_points = 0;
                xminstrumentheader2->Panning_sustain_point = 0;
                xminstrumentheader2->Panning_loop_start_point = 0;
                xminstrumentheader2->Panning_loop_end_point = 0;
                xminstrumentheader2->Panning_type = 0;
            }

            if (instrument_flags & 8)
            {
                xminstrumentheader2->Vibrato_type = rxmcurptr[0];
                xminstrumentheader2->Vibrato_sweep = rxmcurptr[1];
                xminstrumentheader2->Vibrato_depth = rxmcurptr[2];
                xminstrumentheader2->Vibrato_rate = rxmcurptr[3];
                rxmcurptr += 4;
            }
            else
            {
                xminstrumentheader2->Vibrato_type = 0;
                xminstrumentheader2->Vibrato_sweep = 0;
                xminstrumentheader2->Vibrato_depth = 0;
                xminstrumentheader2->Vibrato_rate = 0;
            }

            uint8_t *samplecurptr = rxmcurptr;
            for (unsigned int sample = 0; sample < number_of_samples; sample++)
            {
                uint32_t sample_loop_start = *( (uint32_t *)&(samplecurptr[4]) );
                uint32_t sample_loop_end = *( (uint32_t *)&(samplecurptr[8]) );

                uint8_t sample_type = samplecurptr[12];
                uint8_t xm_sample_type;

                _xmsampleheader *xmsampleheader = (_xmsampleheader *)sxmcurptr;
                sxmcurptr += sizeof(_xmsampleheader);

                if (sample_type & 24) // loop ? (forward or pingpong)
                {
                    xm_sample_type = (sample_type & 16)?2:1; // forward or pingpong ?
                    xmsampleheader->Sample_length = sample_loop_end + 3;
                    xmsampleheader->Sample_loop_start = sample_loop_start;
                    xmsampleheader->Sample_loop_length = sample_loop_end - sample_loop_start;
                }
                else
                {
                    xm_sample_type = 0;
                    xmsampleheader->Sample_length = sample_loop_end;
                    xmsampleheader->Sample_loop_start = 0;
                    xmsampleheader->Sample_loop_length = 0;
                }

                if (sample_type & 4) // 16-bit ?
                {
                    xm_sample_type |= 16;
                    xmsampleheader->Sample_length *= 2;
                    xmsampleheader->Sample_loop_start *= 2;
                    xmsampleheader->Sample_loop_length *= 2;
                }

                xmsampleheader->Volume = samplecurptr[15];
                xmsampleheader->Finetune = 2 * ((int8_t *)samplecurptr)[13];
                xmsampleheader->Type = xm_sample_type;
                xmsampleheader->Panning = samplecurptr[16];
                xmsampleheader->Relative_note_number = ((int8_t *)samplecurptr)[14];
                xmsampleheader->Reserved = 0;
                memset(xmsampleheader->Sample_name, ' ', 22); // Pad with spaces

                samplecurptr += 22;
            }

            samplecurptr = rxmcurptr;
            for (unsigned int sample = 0; sample < number_of_samples; sample++)
            {
                uint8_t *sample_data_ptr = samplecurptr + *( (uint32_t *)samplecurptr );
                uint32_t sample_length = *( (uint32_t *)&(samplecurptr[8]) );
                uint8_t sample_type = samplecurptr[12];

                if (sample_type & 24) // loop ? (forward or pingpong)
                {
                    sample_length += 3;
                }

                memcpy(sxmcurptr, sample_data_ptr, sample_length * ((sample_type & 4)?2:1));
                sxmcurptr += sample_length * ((sample_type & 4)?2:1);

                samplecurptr += 22;
            }

            rxmcurptr = samplecurptr;
        }
        else
        {
            xminstrumentheader1->Instrument_size = sizeof(_xminstrumentheader1);
        }
    }

    *xmsize = (uintptr_t)sxmcurptr - (uintptr_t)xm;

    return xm;
}

extern "C" void rxminit(void)
{
    current_song = 0;
}

extern "C" int rxmdevinit(tdinfo *dinfo, void *)
{
    DWORD flags = 0;
    if (!(dinfo->flags & df16bit )) flags |= BASS_DEVICE_8BITS;
    if (!(dinfo->flags & dfStereo)) flags |= BASS_DEVICE_MONO;
    if (  dinfo->rate != 0        ) flags |= BASS_DEVICE_FREQ;

    if ( BASS_Init(-1, dinfo->rate, flags, 0, NULL) )
    {
        return 0;
    }
    else
    {
        return 3;
    }
}

extern "C" void rxmdevdone(void)
{
    if (current_song) rxmstop(xmStop);
    BASS_Free();
}

extern "C" void rxmsetvol(int vol)
{
    //BASS_SetVolume(vol / 256.0f);
}

extern "C" void rxmplay(void *rxmmem, int len, int pos)
{
    if (current_song) rxmstop(xmStop);

    uint32_t xmsize;
    void *xm = rxm2xm(rxmmem, len, &xmsize);
    if (xm == NULL) return;

    current_song = BASS_MusicLoad(
        TRUE,
        xm,
        0,
        xmsize,
        BASS_MUSIC_RAMP | BASS_MUSIC_POSRESETEX,
        0
    );

    free(xm);

    if (current_song == 0) return;

    if (pos)
    {
        BASS_ChannelSetPosition(current_song, MAKELONG(pos, 0), BASS_POS_MUSIC_ORDER);
        BASS_ChannelPlay(current_song, FALSE);
    }
    else
    {
        BASS_ChannelPlay(current_song, TRUE);
    }
}

extern "C" void rxmstop(int method)
{
    if (current_song)
    {
        if (method == xmFade)
        {
            BASS_ChannelSlideAttribute(current_song, BASS_ATTRIB_VOL, 0.0f, 1000);
            SDL_Delay(1100);

            BASS_ChannelStop(current_song);
            BASS_MusicFree(current_song);
            current_song = 0;
        }
        else
        {
            BASS_ChannelStop(current_song);
            BASS_MusicFree(current_song);
            current_song = 0;
        }
    }
}

