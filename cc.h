#define set_high_byte(reg, value) ( ((reg) & 0xffff00ff) | (((uint32_t)(uint8_t)(value)) << 8) )
