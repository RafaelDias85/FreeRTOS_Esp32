/*
 * Exemplo que demonstra como criar fila, adicionar e ler
 *dados da fila através de duas tasks
 */

#include <stdio.h>

/*Biblioteca dos Pinos gpio*/
#include "driver/gpio.h"

/*Biblioteca FreeRTOS*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"      /*biblioteca para Task*/
#include "freertos/queue.h"     /*biblioteca para queue*/

/*Mapeamento de pino*/
#define LED GPIO_NUM_25

/*Variaveis auxiliares para fila*/

QueueHandle_t xFila;

/*Variaveis auxiliares para Task*/
TaskHandle_t xTask1Handle;
TaskHandle_t xTask2Handle;
TaskHandle_t xTaskBlinkHandle;

/*prototipo de função*/

void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
void vTaskBlink(void *pvParameters);

void app_main(void)
{
    BaseType_t xReturned; // Para verificar se a task foi criada

    xFila = xQueueCreate(5, sizeof(int));

    if (xFila == NULL)
    {
        printf("Nao foi possivel criar a fila\n");
    }
    else
    {
        printf("A Fila foi Criada\n");
    }

    xReturned = xTaskCreate(vTask1, "Task1", configMINIMAL_STACK_SIZE + 1024, NULL, 1, &xTask1Handle);

    if (xReturned == pdFAIL)
    {
        printf("Nao foi possivel criar a Task 1\n");
    }
    else
    {
        printf("A Task 1 foi Criada\n");
    }

    xReturned = xTaskCreate(vTask2, "Task2", configMINIMAL_STACK_SIZE + 1024, NULL, 1, &xTask2Handle);

    if (xReturned == pdFAIL)
    {
        printf("Nao foi possivel criar a task1\n");
    }
    else
    {
        printf("A Task 2 foi Criada\n");
    }

    xReturned = xTaskCreate(vTaskBlink, "TaskBlink", configMINIMAL_STACK_SIZE + 1024, (void *)LED, 1, &xTaskBlinkHandle);

    if (xReturned == pdFAIL)
    {
        printf("Nao foi possivel criar a task1\n");
    }
    else
    {
        printf("A Task 3 foi Criada\n");
    }
}

void vTask1(void *pvParameters)
{

    int count = 0;

    while (1)
    {
        if (count < 10)
        {
            xQueueSend(xFila, &count, portMAX_DELAY);
            count++;
        }
        else
        {
            count = 0;

            vTaskDelay(pdMS_TO_TICKS(5000));
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void vTask2(void *pvParameters)
{
    int valorRecebido = 0;
    while (1)
    {
        if (xQueueReceive(xFila, &valorRecebido, pdMS_TO_TICKS(1000)) == pdTRUE)
        {
            printf("Valor Recebido %d\n", valorRecebido);
        } else {
            printf("TIMEOUT\n");
        }

    }
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