/*
 *Exemplo de Parar e Reiniciar uma task
 *Esse exemplo exibe como criar tarefas no freeRTOS
 *
 */

#include <stdio.h>

/*Biblioteca dos Pinos gpio*/
#include "driver/gpio.h"

/*Biblioteca FreeRTOS*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*Mapeamento de pinos*/
#define LED GPIO_NUM_2

/*Variaveis para armazenamento do handle das tasks*/

TaskHandle_t task1Handle = NULL;
TaskHandle_t task2Handle = NULL;

int state = 0;

/*prototipos das tasks*/

void vTask1(void *pvParameters);
void vTask2(void *pvParameters);

void app_main(void)
{
    /*Cria a task com Fun��o, "identificador", tamanho da stack que sr� usada
    inicializa��o (null), valor de prioridade, Endere�o da Variavel onde ser� armazendo a task */
    xTaskCreate(vTask1, "TASK1", configMINIMAL_STACK_SIZE, NULL, 1, &task1Handle);
    xTaskCreate(vTask2, "TASK2", configMINIMAL_STACK_SIZE + 1024, NULL, 2, &task2Handle);
}

void vTask1(void *pvParameters)
{
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    while (1)
    {
        state = !state;
        gpio_set_level(LED, !state);

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void vTask2(void *pvParameters)
{
    int contador = 1;

    while (1)
    {
        printf("Contador = %d \n", contador++);

        if (contador == 11)
        {
            printf("Suspendendo a task 1\n");
            gpio_set_level(LED, 0);
            vTaskSuspend(task1Handle);
        }
        else if (contador == 16)
        {
            printf("Retornando a task 1\n");
            vTaskResume(task1Handle);
            contador = 0;
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
