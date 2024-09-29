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
    // Configura��o do LED
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    // Cria��o do sem�foro
    xSemaphore = xSemaphoreCreateBinary();
    if (xSemaphore == NULL)
    {
        printf("Nao foi Possivel Criar o Semaforo");
    }

    // Cria��o das tarefas
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

        // Dar sem�foro antes de atrasar, para evitar bloqueio
        xSemaphoreGive(xSemaphore);
        vTaskDelay(pdMS_TO_TICKS(500)); // Delay de 1 segundo para piscar o LED
    }
}

void vTaskADC(void *pvParameters)
{
    // Configura��o do ADC
    adc1_config_width(ADC_WIDTH_BIT_12);                      // Define a resolu��o do ADC (12 bits)
    adc1_config_channel_atten(ADC1_CHANNEL, ADC_ATTEN_DB_12); // Define a atenua��o (12 dB)

    int adcValue;
    int numSamples = 10; // N�mero de amostras para calcular a m�dia
    while (1)
    {
        xSemaphoreTake(xSemaphore, portMAX_DELAY);

        adcValue = 0;

        // Captura v�rias amostras e calcula a m�dia
        for (int i = 0; i < numSamples; i++)
        {
            adcValue += adc1_get_raw(ADC1_CHANNEL);
            vTaskDelay(pdMS_TO_TICKS(10)); // Pequeno delay para permitir estabiliza��o
        }
        adcValue /= numSamples; // M�dia das amostras
        printf("Valor do ADC %d\n\n", adcValue);
    }
}
