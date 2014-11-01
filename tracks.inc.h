extern uint32_t frame;

extern "C" void dotrack(uint32_t _track, uint32_t typesize, uint32_t _edi);
extern "C" void dorottrack(uint32_t _track, uint32_t _edi);
//extrn dotrack:near, slerp:near, dorottrack:near
extern "C" void doltrack(uint32_t _track, uint32_t typesize, uint32_t _edi);
extern "C" void dohidetrack(uint32_t _ebx, uint32_t _edi);
//extrn doltrack:near, dohidetrack:near
