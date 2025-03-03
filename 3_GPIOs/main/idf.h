#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

void Pin_As_Output(uint8_t pinNum);
void Set_Pin_Level(uint8_t pinNum, bool level);
bool Get_Pin_Level(uint8_t pinNum);
void Pin_As_IRAM_Input(uint8_t pinNum, uint8_t *interrupt_flag);
uint64_t get_time();
void delay(uint16_t pdMs);
