/*
 * Exemplo para demonstrar o uso de semáforo binário dentro
 * de uma ISR (interrupção)
 */

#include <stdio.h>

/* Bibliotecas para manipulação de GPIOs */
#include "driver/gpio.h"

/* Bibliotecas do FreeRTOS para multitarefas e comunicação */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

/* Definição do LED e do Botão como GPIOs */
#define LED GPIO_NUM_25   // Define o pino 25 como o LED
#define BT GPIO_NUM_12    // Define o pino 12 como o botão

/* Declaração do semáforo */
SemaphoreHandle_t semaforo;

/* Handles para as tarefas */
TaskHandle_t TaskTrataBTHandle;
TaskHandle_t TaskBlinkHandle;

/* Declaração das funções das tarefas e da função de interrupção */
void vTaskTrataBt(void *pvParameters);
void vTaskBlink(void *pvParameters);
void ISR_CallBack();

/* Função principal que inicializa o aplicativo */
void app_main(void)
{
    /* Configura o pino do LED como saída */
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    /* Configura o pino do botão como entrada */
    gpio_set_direction(BT, GPIO_MODE_INPUT);

    /* Habilita o modo PullUP para o botão (coloca o pino em nível lógico alto quando não pressionado) */
    gpio_set_pull_mode(BT, GPIO_PULLUP_ONLY);

    /* Configura o tipo de interrupção para detectar borda de descida (quando o botão é pressionado) */
    gpio_set_intr_type(BT, GPIO_INTR_NEGEDGE);

    /* Instala o serviço de interrupção para o GPIO */
    gpio_install_isr_service(ESP_INTR_FLAG_LOWMED);
    
    /* Adiciona o manipulador da interrupção ao botão */
    gpio_isr_handler_add(BT, ISR_CallBack, (void *)BT);

    /* Cria o semáforo binário */
    semaforo = xSemaphoreCreateBinary();

    /* Cria a tarefa para tratar o botão */
    xTaskCreate(vTaskTrataBt, "Task BT", configMINIMAL_STACK_SIZE + 1024, NULL, 3, &TaskTrataBTHandle);

    /* Cria a tarefa para controlar o piscar do LED */
    xTaskCreate(vTaskBlink, "Task Blink", configMINIMAL_STACK_SIZE + 1024, NULL, 1, &TaskBlinkHandle);
}

/* Função que pisca o LED */
void vTaskBlink(void *pvParameters)
{
    int pisca = 0;  // Variável para alternar entre aceso e apagado
    while (1)
    {
        pisca = !pisca;  // Alterna o estado (0 ou 1)
        gpio_set_level(LED, pisca);  // Define o nível lógico do LED (ligado ou desligado)
        vTaskDelay(pdMS_TO_TICKS(1000));  // Espera 1 segundo (1000 milissegundos)
    }
}

/* Função de callback da ISR (Interrupção) */
void ISR_CallBack()
{
    BaseType_t xHighPriorityTaskWoken = 0;  // Flag para verificar se uma tarefa de alta prioridade foi acordada

    /* Libera o semáforo de dentro da ISR */
    xSemaphoreGiveFromISR(semaforo, &xHighPriorityTaskWoken);

    /* Se uma tarefa de alta prioridade foi acordada, troca o contexto para executar a tarefa imediatamente */
    if (xHighPriorityTaskWoken == pdTRUE)
    {
        portYIELD_FROM_ISR();  // Muda o contexto de tarefa se necessário
    }
}

/* Tarefa que trata o evento de pressionar o botão */
void vTaskTrataBt(void *pvParameters)
{
    int contador = 0;  // Contador para mostrar quantas vezes o botão foi pressionado

    while (1)
    {
        /* Espera pelo semáforo que será liberado pela interrupção do botão */
        xSemaphoreTake(semaforo, portMAX_DELAY);

        /* Imprime o valor do contador no console */
        printf("Contador : %d \n", contador++);
    }
}
