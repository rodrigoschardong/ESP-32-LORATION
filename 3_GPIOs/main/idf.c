#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h" 
#include <esp_timer.h>

#include "esp_log.h"
#include "esp_err.h"

#define TAG "GPIO"

/**
 * @brief  Configura o pino como saida
 * @note   Pode controlar o nivel logico do pino
 * @param  pinNum: Numeracao do pino a ser configurado
 * @retval None
 */
void Pin_As_Output(uint8_t pinNum){
   gpio_set_direction(pinNum, GPIO_MODE_OUTPUT);
}

/**
 * @brief  Configura o nivel logico do pino de saida
 * @note   Pode configurar como nivel alto ou baixo
 * @param  pinNum: Numero do pino a ser configurado
 * @param  level: Boleano true = alto, false = baixo
 * @retval None
 */
void Set_Pin_Level(uint8_t pinNum, bool level){
    gpio_set_level(pinNum, level);
}
/**
 * @brief  Le o nivel logico no pino de entrada
 * @note   Pode ler se o nivel esta alto ou baixo
 * @param  pinNum: Numero do pino a ser lido
 * @retval Boleano se o nivel esta alto ou baixo
 */
bool Get_Pin_Level(uint8_t pinNum){
    return gpio_get_level(pinNum);
}

/**
 * @brief  Interrupcao que é acionada no instante em que o evento acontece
 * @note   Nao sobrecarregar esta função, termine-a o mais rapido possivel
 * @param  *interrupt_flag: O endereço da variavel que vai avisar que a interrupcao foi acionada
 * @retval 
 */
void IRAM_ATTR Pin_ISR_Handler(void* *interrupt_flag){
    *interrupt_flag = *interrupt_flag + 1;
}

/**
 * @brief  Configura um pino para acionar uma interrupcao
 * @note   Definir o numero do pino, o tipo de interrupcao, o modo do botao
 * @param  pinNum: Numero do pino a ser configurado
 * @param  *interrupt_flag: Endereco da variavel a ser passada para a interrupcao interagir
 * @retval None
 */
void Pin_As_IRAM_Input(uint8_t pinNum, uint8_t *interrupt_flag){
    gpio_config_t io_config = {
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL<< pinNum),
        .intr_type = GPIO_INTR_POSEDGE,
        .pull_up_en = 1,
        .pull_down_en = 0
    };
    gpio_config(&io_config);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(pinNum, Pin_ISR_Handler, (void*) interrupt_flag);
}

uint64_t get_time(){
    return esp_timer_get_time();
}

void delay(uint16_t pdMs){
    vTaskDelay(pdMS_TO_TICKS(pdMs));
}