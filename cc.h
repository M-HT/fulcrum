#include <stdint.h>
#include <cmath>
using namespace std;

#define set_high_byte(reg, value) ( ((reg) & 0xffff00ff) | (((uint32_t)(uint8_t)(value)) << 8) )

#if !defined(UINT8_MAX)
    #define UINT8_MAX (255)
#endif
#if !defined(UINT32_MAX)
    #define UINT32_MAX (4294967295U)
#endif

#if defined(USE_FLOATS)
    typedef float realnum;
#else
    typedef double realnum;
#endif

