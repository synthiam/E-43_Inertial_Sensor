#ifndef __EZUtils
#define __EZUtils

#include <stm32f0xx_gpio.h>

bool IsBitSet(long b, int pos);

void DelayMS(uint32_t val);

void DelayCycles(uint32_t val);

uint8_t Convert8(uint64_t i, uint8_t pos);

int strlen(char s[]);

int atoi(char s[]);

void reverse(char s[]);

#endif