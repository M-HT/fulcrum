extern uint32_t frame;

extern "C" void dotrack(void *_track, uint32_t typesize, float *edi);
extern "C" void dorottrack(void *_track, float *edi);
//extrn dotrack:near, slerp:near, dorottrack:near
extern "C" void doltrack(void *_track, uint32_t typesize, float *edi);
extern "C" void dohidetrack(void *ebx, int32_t *edi);
//extrn doltrack:near, dohidetrack:near
