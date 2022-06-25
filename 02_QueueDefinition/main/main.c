/* Ejemplo de uso de colas */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_system.h"
#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "init.h"
#include "read.h"

#define TAM_COLA 20 /* 20 mensajes */
#define TAM_MSG 8   /* Cada Mensaje 7 caracteres */

xQueueHandle colaMensaje;

void escribe1(void *pvParameter)
{
    char cadena[8];
    while (1)
    {
        strcpy(cadena, "Tarea1\n");

        if (xQueueSendToBack(colaMensaje, &cadena, 2000 / portTICK_PERIOD_MS) != pdTRUE)
        { // 2seg--> Tiempo max. que la tarea está bloqueada si la cola está llena
            printf("error escribe1\n");
        }
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    nvs_flash_init();

    colaMensaje = xQueueCreate(TAM_COLA, TAM_MSG);

    xTaskCreate(&lee1, "lee1", 1024, NULL, 5, NULL);
    xTaskCreate(&escribe1, "escribe1", 1024, NULL, 1, NULL);
    xTaskCreate(&escribe2, "escribe2", 1024, NULL, 1, NULL);
    xTaskCreate(&escribe3, "escribe3", 1024, NULL, 1, NULL);
}