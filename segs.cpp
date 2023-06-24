
#include <TM1637Display.h>

const uint8_t SEG_ON[] = {
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,  // O
  SEG_C | SEG_E | SEG_G,                          // n
  0,0
};
const uint8_t SEG_OFF[] = {
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,  // O
  SEG_A | SEG_E | SEG_F | SEG_G,                  // F
  SEG_A | SEG_E | SEG_F | SEG_G,                  // F
  0,
};
const uint8_t SEG_BRIGHTNESS[] = {
  0,
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,
  0,
};
// SEG_GOOD
const uint8_t SEG_GOOD[] = {
  // G
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G,
  // O
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,
  // O
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,
  // D
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,
};