#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h" 
#include <esp_timer.h>

#include "esp_log.h"
#include "esp_err.h"

#define TAG "GPIO"
#define BLUE_LED    GPIO_NUM_2
#define BUTTON      GPIO_NUM_15

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

/**
 * @brief  
 * @note   
 * @param  *interrupt_flag: 
 * @retval None
 */
void Run_States(uint8_t *interrupt_flag){
    #define MAX_STATE   3
    #define DELAY_US    500000

    bool level = false;
    bool blinking = false;
    uint64_t start = 0;
    *interrupt_flag = 0;  // 0 Led Desligado // 1 Led Ligado // 2 Led Piscando
    Set_Pin_Level(BLUE_LED, level);
    while(1){
        if(*interrupt_flag > MAX_STATE){
            *interrupt_flag = 0;
            blinking = false;
        }
        switch (*interrupt_flag)
        {
        case 0:
            if(level != false){
                ESP_LOGI(TAG, "Turning Led Off");
                level = false;
                Set_Pin_Level(BLUE_LED, level);
            }
            break;
        case 1:
            if(level != true){
                ESP_LOGI(TAG, "Turning Led On");
                level = true;
                Set_Pin_Level(BLUE_LED, level);
            }
            break;
        case 2:
            if(!blinking){
                ESP_LOGI(TAG, "Blinking Led");
                start = esp_timer_get_time();
                blinking = true;
                level = !level;
                Set_Pin_Level(BLUE_LED, level);
            }
            if(Timeout_Expired(start, DELAY_US)){
                blinking = false;
            }            
            break;
        default:
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void GPIO_Config(){
    uint8_t button_interrupt_flag = 0;

    ESP_LOGI(TAG, "Starting");
    Pin_As_Output(BLUE_LED);
    Pin_As_IRAM_Input(BUTTON, &button_interrupt_flag);

    Run_States(&interrupt_flag);
}