/* Ejemplo basico con tareas. */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "init.h"

void app_main()
{
    nvs_flash_init();
    xTaskCreate(&tarea1, "tarea1", 1024, NULL, 1, NULL);
    xTaskCreate(&tarea2, "tarea2", 1024, NULL, 2, NULL);
    xTaskCreate(&tarea3, "tarea3", 1024, NULL, 3, NULL);
    xTaskCreate(&tarea4, "tarea4", 1024, NULL, 4, NULL);

    // vTaskStartScheduler();
}