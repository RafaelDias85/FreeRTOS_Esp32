/*
 * Exemplo para demonstrar o uso de sem�foro bin�rio dentro
 * de uma ISR (interrup��o)
 */

#include <stdio.h>

/* Bibliotecas para manipula��o de GPIOs */
#include "driver/gpio.h"

/* Bibliotecas do FreeRTOS para multitarefas e comunica��o */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

/* Defini��o do LED e do Bot�o como GPIOs */
#define LED GPIO_NUM_25   // Define o pino 25 como o LED
#define BT GPIO_NUM_12    // Define o pino 12 como o bot�o

/* Declara��o do sem�foro */
SemaphoreHandle_t semaforo;

/* Handles para as tarefas */
TaskHandle_t TaskTrataBTHandle;
TaskHandle_t TaskBlinkHandle;

/* Declara��o das fun��es das tarefas e da fun��o de interrup��o */
void vTaskTrataBt(void *pvParameters);
void vTaskBlink(void *pvParameters);
void ISR_CallBack();

/* Fun��o principal que inicializa o aplicativo */
void app_main(void)
{
    /* Configura o pino do LED como sa�da */
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    /* Configura o pino do bot�o como entrada */
    gpio_set_direction(BT, GPIO_MODE_INPUT);

    /* Habilita o modo PullUP para o bot�o (coloca o pino em n�vel l�gico alto quando n�o pressionado) */
    gpio_set_pull_mode(BT, GPIO_PULLUP_ONLY);

    /* Configura o tipo de interrup��o para detectar borda de descida (quando o bot�o � pressionado) */
    gpio_set_intr_type(BT, GPIO_INTR_NEGEDGE);

    /* Instala o servi�o de interrup��o para o GPIO */
    gpio_install_isr_service(ESP_INTR_FLAG_LOWMED);
    
    /* Adiciona o manipulador da interrup��o ao bot�o */
    gpio_isr_handler_add(BT, ISR_CallBack, (void *)BT);

    /* Cria o sem�foro bin�rio */
    semaforo = xSemaphoreCreateBinary();

    /* Cria a tarefa para tratar o bot�o */
    xTaskCreate(vTaskTrataBt, "Task BT", configMINIMAL_STACK_SIZE + 1024, NULL, 3, &TaskTrataBTHandle);

    /* Cria a tarefa para controlar o piscar do LED */
    xTaskCreate(vTaskBlink, "Task Blink", configMINIMAL_STACK_SIZE + 1024, NULL, 1, &TaskBlinkHandle);
}

/* Fun��o que pisca o LED */
void vTaskBlink(void *pvParameters)
{
    int pisca = 0;  // Vari�vel para alternar entre aceso e apagado
    while (1)
    {
        pisca = !pisca;  // Alterna o estado (0 ou 1)
        gpio_set_level(LED, pisca);  // Define o n�vel l�gico do LED (ligado ou desligado)
        vTaskDelay(pdMS_TO_TICKS(1000));  // Espera 1 segundo (1000 milissegundos)
    }
}

/* Fun��o de callback da ISR (Interrup��o) */
void ISR_CallBack()
{
    BaseType_t xHighPriorityTaskWoken = 0;  // Flag para verificar se uma tarefa de alta prioridade foi acordada

    /* Libera o sem�foro de dentro da ISR */
    xSemaphoreGiveFromISR(semaforo, &xHighPriorityTaskWoken);

    /* Se uma tarefa de alta prioridade foi acordada, troca o contexto para executar a tarefa imediatamente */
    if (xHighPriorityTaskWoken == pdTRUE)
    {
        portYIELD_FROM_ISR();  // Muda o contexto de tarefa se necess�rio
    }
}

/* Tarefa que trata o evento de pressionar o bot�o */
void vTaskTrataBt(void *pvParameters)
{
    int contador = 0;  // Contador para mostrar quantas vezes o bot�o foi pressionado

    while (1)
    {
        /* Espera pelo sem�foro que ser� liberado pela interrup��o do bot�o */
        xSemaphoreTake(semaforo, portMAX_DELAY);

        /* Imprime o valor do contador no console */
        printf("Contador : %d \n", contador++);
    }
}
