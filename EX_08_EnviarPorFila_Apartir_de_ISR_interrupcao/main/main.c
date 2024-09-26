/*
 * Exemplo que demonstra como enviar valores para uma fila
 * a partir de uma ISR (interrup��o);
 */

#include <stdio.h>

#include "driver/gpio.h"
#include "esp_mac.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define LED GPIO_NUM_25     // LED principal
#define BT GPIO_NUM_12      // Bot�o
#define LED_ERRO GPIO_NUM_2 // LED para indicar falha

QueueHandle_t xFila;
TaskHandle_t xTask1Handle;
TaskHandle_t xTaskBlinkHandle;

void vTask1(void *pvParameters);
void trataISR_BT(void *arg);
void vTaskBlink(void *pvParameters);

void app_main(void)
{
    // Configura��es do bot�o
    gpio_set_direction(BT, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BT, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(BT, GPIO_INTR_NEGEDGE); // Detectar borda de descida

    // Configura��o do LED de erro
    gpio_set_direction(LED_ERRO, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_ERRO, 0); // Inicialmente desligado

    // Instala��o da interrup��o
    gpio_install_isr_service(ESP_INTR_FLAG_LOWMED);
    gpio_isr_handler_add(BT, trataISR_BT, (void *)BT);

    // Cria��o da fila
    xFila = xQueueCreate(1, sizeof(int));

    // Cria��o das tarefas
    xTaskCreate(vTask1, "task1", configMINIMAL_STACK_SIZE + 2048, NULL, 1, &xTask1Handle);
    xTaskCreate(vTaskBlink, "taskBlink", configMINIMAL_STACK_SIZE + 1024, (void *)LED, 1, &xTaskBlinkHandle);
}

void vTask1(void *pvParameters)
{
    int valorRecebido;
    while (1)
    {
        xQueueReceive(xFila, &valorRecebido, portMAX_DELAY);
        printf("Botao Pressionado %d\n", valorRecebido);
    }
}

void trataISR_BT(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken;
    static int valor; // inicializar valor se necess�rio
    valor++;

    // Enviar o valor incrementado para a fila
    BaseType_t resultado = xQueueSendFromISR(xFila, &valor, &xHigherPriorityTaskWoken);
}

void vTaskBlink(void *pvParameters)
{
    int pin = (int)pvParameters;
    int pisca = 1;
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    while (1)
    {
        pisca = !pisca;
        gpio_set_level(pin, pisca); // Piscar LED

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
