#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "esp_log.h"

#define pdUS_TO_TICKS(us) ((TickType_t) ((us) / (TickType_t) 1000 / portTICK_PERIOD_MS))

typedef struct {
    int id;
    char* text;
    TickType_t interval_ticks;  // Interval in ticks
    TimerHandle_t timer_handle; // Initialize to NULL
} task_params_t;

static const char *TAG = "TimerExample";

task_params_t timer1_params = {1, "Timer 1 Text", pdUS_TO_TICKS(2000000), NULL}; // 2000000 us = 2 seconds
task_params_t timer2_params = {2, "Timer 2 Text", pdUS_TO_TICKS(3000000), NULL}; // 3000000 us = 3 seconds
task_params_t timer3_params = {3, "Timer 3 Text", pdUS_TO_TICKS(5000000), NULL}; // 5000000 us = 5 seconds

// Callback function to be executed by the timer
void myCallback(TimerHandle_t xTimer) {
    task_params_t *params = (task_params_t *)pvTimerGetTimerID(xTimer);
    ESP_LOGI(TAG, "Timer %d - ID: %d, Text: %s", params->id, params->id, params->text);
}

void editTimerParameters(task_params_t *params, char *newText, TickType_t newInterval_ticks) {
    // Suspend the timer to prevent race conditions during parameter update
    xTimerStop(params->timer_handle, portMAX_DELAY);
    
    // Update the parameters
    params->text = newText;
    params->interval_ticks = newInterval_ticks;
    
    // Restart the timer with the updated parameters
    xTimerChangePeriod(params->timer_handle, newInterval_ticks, 0);
    xTimerStart(params->timer_handle, 0);
}

void app_main(void) {
    TimerHandle_t myTimer1 = xTimerCreate("MyTimer1", timer1_params.interval_ticks, pdTRUE, &timer1_params, myCallback);
    TimerHandle_t myTimer2 = xTimerCreate("MyTimer2", timer2_params.interval_ticks, pdTRUE, &timer2_params, myCallback);
    TimerHandle_t myTimer3 = xTimerCreate("MyTimer3", timer3_params.interval_ticks, pdTRUE, &timer3_params, myCallback);

    if (myTimer1 != NULL && myTimer2 != NULL && myTimer3 != NULL) {
        timer1_params.timer_handle = myTimer1;
        timer2_params.timer_handle = myTimer2;
        timer3_params.timer_handle = myTimer3;

        xTimerStart(myTimer1, 0);
        xTimerStart(myTimer2, 0);
        xTimerStart(myTimer3, 0);
    }

    // Simulate editing timer parameters after 10 seconds
    vTaskDelay(pdMS_TO_TICKS(10000));
    editTimerParameters(&timer1_params, "Updated Timer 1 Text", pdUS_TO_TICKS(3000000)); // 3000000 us = 3 seconds
    
    // Simulate canceling a timer after 20 seconds
    vTaskDelay(pdMS_TO_TICKS(20000));
    xTimerStop(myTimer2, 0);
    xTimerDelete(myTimer2, 0);

    // Continue with your main application code
}
