/*
 *Exemplo que demonstra como enviar valores para uma fila
 *apartir de uma ISR (interrupcao);
 */

#include <stdio.h>

#include "driver/gpio.h"
#include "esp_mac.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define LED GPIO_NUM_2
#define BT GPIO_NUM_12

QueueHandle_t xFila;
TaskHandle_t xTask1Handle;
TaskHandle_t xTaskBlinkHandle;

void vTask1(void *pvParameters);
void trataISR_BT(void *arg);
void vTaskBlink(void *pvParameters);

void app_main(void)
{
    gpio_set_direction(BT, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BT, GPIO_PULLUP_ONLY);

    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);
    gpio_isr_handler_add(BT, trataISR_BT, (void *)BT);

    xFila = xQueueCreate(20, sizeof(int));

    xTaskCreate(vTask1, "task1", configMINIMAL_STACK_SIZE + 2048, NULL, 1, &xTask1Handle);
    xTaskCreate(vTaskBlink, "taskBlink", configMINIMAL_STACK_SIZE + 1024, (void *)LED, 1, &xTaskBlinkHandle);
}

void vTask1(void *pvParameters)
{
    int valorRecebido;
    while (1)
    {
        xQueueReceive(xFila, &valorRecebido, portMAX_DELAY);

        printf("Botão Pressionado %d\n", valorRecebido);
    }
}

void trataISR_BT(void *arg)
{
    static int valor;
    valor++;

    xQueueSendFromISR(xFila, &valor, NULL);
}

void vTaskBlink(void *pvParameters)
{
    int pin = (int)pvParameters;
    int pisca = 1;
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    while (1)
    {
        pisca = !pisca;
        gpio_set_level(LED, !pisca);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
