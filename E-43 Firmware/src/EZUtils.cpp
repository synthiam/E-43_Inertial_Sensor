#include "EZUtils.h"

bool IsBitSet(long b, int pos) {

  return (b & (1 << pos)) != 0;
}

void DelayMS(uint32_t val) {

#ifdef DEBUG

  for (uint32_t p = 0; p < val * 1000; p++)
    asm("nop");

#else

  for (uint32_t p = 0; p < val * 8000; p++)
    asm("nop");

#endif // DEBUG

}

void DelayCycles(uint32_t val) {

  for (uint32_t p = 0; p < val; p++)
    asm("nop");
}

uint8_t Convert8(uint64_t i, uint8_t pos) {

  // pos is 56, 48, 40, 32, 24, 16, 8 or 0

  return ((i >> pos) & 0xff);
}

int strlen(char s[]) {

  int i = 0;

  while (s[i] != '\0')
    ++i;

  return i;
}

int atoi(char s[]) {

  int i, n;

  n = 0;

  for (i = 0; s[i] >= '0' && s[i] <= '9'; ++i)
    n = 10 * n + (s[i] - '0');

  return n;
}

void reverse(char s[]) {

  int i, j;
  char c;

  for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {

    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}
