/* 1º Ejemplo para compartir recursos con MUTEX.
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "freertos/semphr.h"

xSemaphoreHandle xMutex;

void tarea1(void *pvParameter)
{
    while (1)
    {
        if (xMutex != NULL)
        {
            if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE)
            {

                printf("*****************************************************************************************************\n");
                printf("Ejecutando tarea 1 \n");
                printf("**************************************************************************************************** \n");
            }
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
void tarea2(void *pvParameter)
{
    while (1)
    {
        if (xMutex != NULL)
        {
            if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE)
            {
                /* Se toma el Mutex si está disponible*/
                printf("#####################################################################################################\n");
                printf("Ejecutando tarea 2 \n");
                printf("#####################################################################################################\n");
            }
            /* Se da el Mutex para que otra tarea lo pueda usar*/
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void tarea3(void *pvParameter)
{
    while (1)
    {
        if (xMutex != NULL)
        {
            if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE)
            {

                printf("=====================================================================================================\n");
                printf("Ejecutando tarea 3 \n");
                printf("=====================================================================================================\n");
            }
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
void tarea4(void *pvParameter)
{
    while (1)
    {
        if (xMutex != NULL)
        {
            if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE)
            {
                printf("-----------------------------------------------------------------------------------------------------\n");
                printf("Ejecutando tarea 4 \n");
                printf("------------------------------------------------------------------------------------------------------\n");
            }
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main()
{

    // Se crea el Mutex
    xMutex = xSemaphoreCreateMutex();

    // Se crean las tareas
    xTaskCreate(&tarea1, "tarea1", 1024, NULL, 5, NULL);
    xTaskCreate(&tarea2, "tarea2", 1024, NULL, 5, NULL);
    xTaskCreate(&tarea3, "tarea3", 1024, NULL, 5, NULL);
    xTaskCreate(&tarea4, "tarea4", 1024, NULL, 5, NULL);
}
