/*
 *Exemplo de Passar Parametros para Task
 *Esse exemplo exibe como criar tarefas no freeRTOS
 */

#include <stdio.h>

/*Biblioteca dos Pinos gpio*/
#include "driver/gpio.h"

/*Biblioteca FreeRTOS*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*Mapeamento de pinos*/
#define LED1 GPIO_NUM_2
#define LED2 GPIO_NUM_25

/*Variaveis para armazenamento do handle das tasks*/

TaskHandle_t task1Handle = NULL;
TaskHandle_t task2Handle = NULL;
TaskHandle_t task3Handle = NULL;

/* Variaveis auxiliares */

int valor = 500;

/*prototipos das tasks*/

void vTaskBlink(void *pvParameters);
void vTask2(void *pvParameters);

void app_main(void)
{
    /*Cria a task com Função, "identificador", tamanho da stack que srá usada
    inicialização (null), valor de prioridade, Endereço da Variavel onde será armazendo a task */

    xTaskCreatePinnedToCore(vTaskBlink, "TASK1", configMINIMAL_STACK_SIZE, (void *)LED1, 1, &task1Handle, APP_CPU_NUM);
    xTaskCreatePinnedToCore(vTask2, "TASK2", configMINIMAL_STACK_SIZE + 1024, (void *)valor, 2, &task2Handle, PRO_CPU_NUM);
    xTaskCreatePinnedToCore(vTaskBlink, "TASK3", configMINIMAL_STACK_SIZE, (void *)LED2, 1, &task3Handle, APP_CPU_NUM);
}

void vTaskBlink(void *pvParameters)
{
    int pin = (int)pvParameters;
    int state = 0;

    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    while (1)
    {
        state = !state;
        gpio_set_level(pin, !state);

        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

void vTask2(void *pvParameters)
{
    int contador = (int)pvParameters;

    while (1)
    {
        printf("Contador = %d \n", contador++);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
