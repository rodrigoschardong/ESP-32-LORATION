#include "driver/ledc.h"

void PWMConfig();
void PWMReConfig(uint8_t freq);
void PWMSetDuty(uint16_t duty);
void PWMSetDutyWithFade(uint32_t duty, uint16_t fadeTime);