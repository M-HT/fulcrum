
xmLinear    = 1
xmConverted = 2

theader struc
 hdSonglen      dd ?                ;Song length (in patten order table)
 hdRestart      dd ?                ;Restart position
 hdChannels     dd ?                ;Number of channels (2,4,6,8,10,...,32)
 hdPatterns     dd ?                ;Number of patterns (max 256)
 hdInstruments  dd ?                ;Number of instruments (max 128)
 hdSpeed        dd ?                ;Default tempo
 hdBPM          dd ?                ;Default BPM
 hdFlags        db ?                ;Flags: bit 0: 0 = Amiga, 1 = Linear
 hdPtable       db 256 dup(?)       ;Pattern order table
ends

tpatternh struc
 patSize        dd ?
 patRows        dd ?
ends

tinstrh struc
 iSamples       db ?
 iFollows       db ?
ends

tnotesmp struc
 Notesmp        db 96 dup(?)
ends

tenvelope struc
 envLastpoint   db ?                ;Number of envelope points-1
 envSustain     db ?                ;Sustain point       (-1 if none)
 envLoops       db ?                ;Loop start point
 envLoope       db ?                ;Loop end point      (-1 if none)
 envPoint       dw 12*2 dup(?)      ;Points for envelope
ends

tvibrato struc
 vibType        db ?                ;Vibrato type
 vibSweep       db ?                ;Vibrato sweep
 vibDepth       db ?                ;Vibrato depth
 vibRate        db ?                ;Vibrato rate
ends

Xtra = 3                            ;Extra values at end of sample

;sample-flags
sfLoop      =  8h
sfPingpong  = 10h
sf16bit     =  4h

tsampleh struc
 sDataptr       dd ?                ;Data pointer
 sLoops         dd ?                ;Sample loop start
 sLoope         dd ?                ;Sample loop length  ;end
 sType          db ?                ;Type (sample-flags)
 sFinetune      db ?                ;Finetune (signed byte -16..+15)
 sRelnote       db ?                ;Relative note number (signed byte)
 sVol           db ?                ;Volume
 sPan           db ?                ;Panning (0-255)
 sResample      db ?                ;Resampling-factor (GUS, AWE)
 sOffset        dd ?                ;Sample offset (GUS, AWE)
ends


;active-flags
afSustain = 1            ;Bit 0: sustain (note gehalten)
afStop    = 2
afPlay    = 4            ;kanal spielt (start- und stopflag sind gelîscht)
afStart   = 8
afAktiv   = afStart + afPlay
afVol     = 80h

tchannel struc
 caktiv         db ?          ;siehe aktiv-flags

 cnote          db ?
 cinstr         db ?

 ctune          dd ?          ;tuning-anteil (fÅr tone porta)
 crnote         dd ?          ;real-note in 1/64 schritten
 crelnote       dd ?          ;appregio, vibrato, autovibrato

 cvol           dd ?
 crelvol        dd ?          ;tremolo, tremor (alle lautstÑrke-befehle)
 cfadeoutvol    dd ?

 cpan           dd ?

 cmulti         dd ?          ;multi retrig counter

 ctick0         db ?          ;tick 0 indicator

 cvolc          dw ?          ;envelope segment counter
 cvolp          dd ?          ;punkt
 cpanc          dw ?
 cpanp          dd ?

 cportad        dd ?
 cvibofs        db ?          ;0-255
 ctreofs        db ?          ;0-255
 cavofs         db ?          ;0-255 fÅr autovibrato-tabelle
 cswcnt         dd ?          ;tick-zÑhler fÅr sweep

 cinstrh        dd ?          ;pointer auf intstrument
 csampleh       dd ?          ;pointer auf sample-header

 ctype          db ?          ;siehe sample-flags
 csample        dd ?          ;anfang bezogen auf ds:[0] oder GUS-ram
 cloops         dd ?          ;loop-start
 cloope         dd ?          ;loop-ende
 cincval        dd ?          ;frequenz

 cfinalvol      dd ?          ;0-256
 cfinalpan      dd ?          ;0-255
 clivol         dd ?
 crevol         dd ?

 cofinalvol     dd ?          ;0-256
 colivol        dd ?
 corevol        dd ?

 cmixtype       db ?          ;siehe sample-flags
 cmixloops      dd ?          ;loop-start
 cmixloope      dd ?          ;loop-ende
 cmixdir        db ?          ;0: forward, 1: reverse
 cmixpos        dd ?          ;position im spample
 cmixposn       dd ?          ;pos-nachkommastelle
 cmixincval     dd ?          ;frequenz

 cpatvol        dd ?
 cpatpan        dd ?
 cenvvol        dd ?
 cenvpan        dd ?
 coenvvol       dd ?
 coenvpan       dd ?


 cnofx          db ?          ;effekte
 cfx            db 8 dup(?)
 cparams        db 8 dup(?)
 coldparams     db 49 dup(?)

 cappmul        dd ?
 cresample      db ?
 align 4
ends

sbuflen = 16
tsound struc
 sinstrnote     dw ?
 svol           db ?
ends

df16bit   = 1       ;play 16 bit
dfStereo  = 2       ;play stereo
dfReverse = 4       ;reverse stereo
dfWave    = 8       ;wave output
dfNosound = 16
sb_dsp1   = 32      ;play in DSP 1.xx mode
sb_awe    = 64      ;use AWE32 if present
gus_iw    = 128     ;play in extended IW mode
wav_lin   = 256     ;wav linear
wav_32bit = 512

tdinfo struc
 d_base         dd ?
 d_irq          db ?
 d_dma1         db ?
 d_dma2         db ?
 d_rate         dd ?
 d_flags        dd ?
 d_ver          db ?
 d_vol          db ?
 d_mastervol    db ?
 d_mem          dd ?
 d_dmabuf       dd ?
 d_vmt          dd ?
ends

tsvmt struc
; v_test         dd ?
 v_init         dd ?
 v_done         dd ?
 v_setvol       dd ?
 v_play         dd ?
 v_bpm          dd ?
; v_waveplay     dd ?
ends

xmOff = 0
xmStop = 2
xmFade = 3
xmPlay = 4

trxmdata struc
 s_vmt          tsvmt ?

 s_drvmem       dd ?

 xmstatus       db ?
; stopcnt        db ?

 globalvol      dd ?            ;0-64

 tempvar        db ?

;tick-processor
 t_tick         dd ?            ;aktueller player tick
 t_extra        dd ?            ;extra ticks fÅr pattern delay
 t_usec         dd ?            ;tickdauer in mikrosekunden

;pattern-processor
 p_break        db ?            ;1: neues pattern
 p_breakrow     dd ?
 p_pos          dd ?            ;song-position
 p_rowcnt       dd ?
 p_rows         dd ?
 p_exist        db ?            ;leere pattern am ende haben keine daten
 p_offs         dd ?
 p_buf          db ?
 p_bufc         db ?


 p_looppos      dd ?            ;sprungziel
 p_loopcnt      dd ?            ;wiederhol-zÑhler

 s_freqmul      dd ?

 align 4
 head           theader ?
 pattern        dd 256 dup(?)
 instr          dd 128 dup(?)
 sample         dd 128 dup(?)

 freq           dd 768 dup(?)   ;frequenztabelle
 vibrato        db 1024 dup(?)

 align 4
 chdata         tchannel 32 dup(?)

ifndef noAmiga
 apptab         dd 16 dup(?)
endif

 rflag          db ?

if gamesound gt 0
 schannels      dd ?  ;sound-channels
 pchannels      dd ?  ;pattern-channels
 nextchan       dd ?
 soundhead      dd ?
 soundtail      dd ?
 sounds         tsound sbuflen dup(?)
endif

 align 4
ends


tmix struc
 m_inclo        dd ?
 m_inchi        dd ?
 m_samples      dd ?
 m_todo         dd ?
 m_livol        dd ?
 m_revol        dd ?

 m_tsamples     dd ?            ;samples pro mixeinheit (unterteilter tick)
; m_tile         dd ?
; m_tilecnt      dd ?

 align 4
 union

 struc ;8 bit
 m_voltab       db 33*256 dup(?) ; 8k
 m_mixbuf       dd 3750 dup(?)   ;15k   1024 dup(?)   ;4k
 m_clipstart    db 128*32 dup(?) ; 4k
 m_clip         db 128*32 dup(?) ; 4k
 ends

 struc ;16 bit
 m_liadd        dd ?
 m_readd        dd ?
 m_vol          dd ?
 m_mixbuf16     dd 7500 dup(?)   ;30k   2048 dup(?)   ; 8k
 ends

 ends

ends

