/*
 *Exemplo de Criação de tasks
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

/*prototipos das tasks*/

void vTask1(void *pvParameters);
void vTask2(void *pvParameters);

void app_main(void)
{
    /*Cria a task com Função, "identificador", tamanho da stack que srá usada
    inicialização (null), valor de prioridade, Endereço da Variavel onde será armazendo a task */
    xTaskCreate(vTask1, "TASK1", configMINIMAL_STACK_SIZE, NULL, 1, &task1Handle);
    xTaskCreate(vTask2, "TASK2", configMINIMAL_STACK_SIZE + 1024, NULL, 2, &task2Handle);
}

void vTask1(void *pvParameters)
{
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    int state = 0;
    while (1)
    {
        state = !state;
        gpio_set_level(LED, !state);

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void vTask2(void *pvParameters)
{
    int contador = 0;
    while (1)
    {
        printf("Task 2 = %d \n", contador);
        contador++;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}