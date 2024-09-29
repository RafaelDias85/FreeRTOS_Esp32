#include <stdio.h>
#include "driver/gpio.h"
#include "driver/adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define LED GPIO_NUM_25
#define ADC1_CHANNEL ADC1_CHANNEL_6 // ADC1_CHANNEL_6 corresponde a GPIO_NUM_34

TaskHandle_t xTaskADCHandle; // ADC - Conversor Analogico para Digital
TaskHandle_t xTaskBlinkHandle;

SemaphoreHandle_t xSemaphore;

void vTaskBlink(void *pvParameters);
void vTaskADC(void *pvParameters);

void app_main(void)
{
    // Configuração do LED
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    // Criação do semáforo
    xSemaphore = xSemaphoreCreateBinary();
    if (xSemaphore == NULL)
    {
        printf("Nao foi Possivel Criar o Semaforo");
    }

    // Criação das tarefas
    xTaskCreate(vTaskBlink, "vTaskBlink", configMINIMAL_STACK_SIZE + 2048, NULL, 1, &xTaskBlinkHandle);
    xTaskCreate(vTaskADC, "vTaskADC", configMINIMAL_STACK_SIZE + 2048, NULL, 1, &xTaskADCHandle);
}

void vTaskBlink(void *pvParameters)
{
    int pisca = 1;

    while (1)
    {
        pisca = !pisca;
        gpio_set_level(LED, pisca);
        printf("O estado do Led e %d \n", pisca);

        // Dar semáforo antes de atrasar, para evitar bloqueio
        xSemaphoreGive(xSemaphore);
        vTaskDelay(pdMS_TO_TICKS(500)); // Delay de 1 segundo para piscar o LED
    }
}

void vTaskADC(void *pvParameters)
{
    // Configuração do ADC
    adc1_config_width(ADC_WIDTH_BIT_12);                      // Define a resolução do ADC (12 bits)
    adc1_config_channel_atten(ADC1_CHANNEL, ADC_ATTEN_DB_12); // Define a atenuação (12 dB)

    int adcValue;
    int numSamples = 10; // Número de amostras para calcular a média
    while (1)
    {
        xSemaphoreTake(xSemaphore, portMAX_DELAY);

        adcValue = 0;

        // Captura várias amostras e calcula a média
        for (int i = 0; i < numSamples; i++)
        {
            adcValue += adc1_get_raw(ADC1_CHANNEL);
            vTaskDelay(pdMS_TO_TICKS(10)); // Pequeno delay para permitir estabilização
        }
        adcValue /= numSamples; // Média das amostras
        printf("Valor do ADC %d\n\n", adcValue);
    }
}
