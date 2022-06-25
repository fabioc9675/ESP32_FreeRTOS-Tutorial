/* Este ejemplo lee el valor del ADC1 y lo muestra convertido en voltaje en un LCD
 */
// Incluimos los archivos de cabecera necesarios para el ejemplo
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "soc/uart_struct.h"

// Definimos las etiquetas que utilizaremos en el ejemplo
#define ADC1_TEST_CHANNEL (4)

#define TXD (4)
#define RXD (5)
#define RTS (18)
#define CTS (19)

#define BUF_SIZE (1024)

#define TAM_COLA_LECTURA 1 /*1 mensajes*/
#define TAM_MSG_LECTURA 2  /*Cada Mensaje 1 Entero (2 bytes)*/
#define TAM_COLA_VOLTAJE 1 /*1 mensajes*/
#define TAM_MSG_VOLTAJE 6  /*Cada Mensaje 5 caracteres de 1 byte cada uno*/

// creamos como variables globales los manejadores de las dos colas que utilizaremos para enviar datos entre tareas
xQueueHandle cola_Lectura;
xQueueHandle cola_Voltaje;

// tarea para leer el ACD1
void adc1task(void *arg)
{
    // inicializamos el convertidor Analógico/Digital (ADC)
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_TEST_CHANNEL, ADC_ATTEN_11db);
    int valor;
    while (1)
    {
        valor = adc1_get_raw(ADC1_TEST_CHANNEL);
        if (xQueueSendToBack(cola_Lectura, &valor, 1000 / portTICK_RATE_MS) != pdTRUE)
        { // 1seg--> Tiempo max. que la tarea está bloqueada si la cola está llena
            printf("error de escritura\n");
        }
        else
        {
            // printf("El valor de adc1 es:%d\n",valor);
        }
    }
}

// Tarea que recibe los datos de tipo entero del ACD1 los convierte a un tipo float y luego a una cadena de caracteres.
void convierteDatos(void *pvParameter)
{
    int dato;
    char String[6];
    float T = 0.00;
    while (1)
    {
        if (xQueueReceive(cola_Lectura, &dato, 1000 / portTICK_RATE_MS) == pdTRUE)
        { // 1s --> Tiempo max. que la tarea está bloqueada si la cola está vacía
            T = (3.3 * dato) / 4095;
            sprintf(String, "%1.2f\r", T);
        }
        else
        {
            printf("Fallo al leer la cola de lectura del ADC1");
        }

        // Enviamos los datos convertidos a la cola_Voltaje
        if (xQueueSendToBack(cola_Voltaje, &String, 1000 / portTICK_RATE_MS) != pdTRUE)
        { // 2seg--> Tiempo max. que la tarea está bloqueada si la cola está llena
            printf("error al escribir en la cola_Voltaje\n");
        }
        printf("El valor de adc1 es:%d\n", dato);
        // printf ("%1.2f\n",T);
        vTaskDelay(500 / portTICK_RATE_MS);
    }
}

// Tarea para enviar el valor del voltaje a través de la UART1 al microcontrolador que controla el LCD
static void uart_task()
{
    // Configuración de los parámetros de la UART1
    const int uart_num = UART_NUM_1;
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
    };
    uart_param_config(uart_num, &uart_config);
    // Set pines UART1 (TX: IO4, RX: I05, RTS: IO18, CTS: IO19)
    uart_set_pin(uart_num, TXD, RXD, RTS, CTS);
    // Instalamos el driver para la UART
    uart_driver_install(uart_num, BUF_SIZE * 2, 0, 0, NULL, 0);

    // Cadena constante para mostrar en la primera fila de caracteres del LCD
    char *data = "Lectura ADC1        V= ";
    char Rx[7];
    while (1)
    {
        if (xQueueReceive(cola_Voltaje, &Rx, 10000 / portTICK_RATE_MS) == pdTRUE)
        { // 10s --> Tiempo max. que la tarea está bloqueada si la cola está vacía
            // Escribimos los datos en la UART1
            uart_write_bytes(uart_num, (const char *)data, strlen(data));
            uart_write_bytes(uart_num, (char *)Rx, strlen(Rx));
        }
    }
}

void app_main()
{
    // Creamos las dos colas que utilizaremos en el programa para comunicar datos entre las tareas
    cola_Lectura = xQueueCreate(TAM_COLA_LECTURA, TAM_MSG_LECTURA);
    cola_Voltaje = xQueueCreate(TAM_COLA_VOLTAJE, TAM_MSG_VOLTAJE);

    // Creamos las tareas del programa
    xTaskCreate(adc1task, "adc1task", 1024 * 3, NULL, 5, NULL);
    xTaskCreate(convierteDatos, "convierteDatos", 1024 * 3, NULL, 10, NULL);
    xTaskCreate(uart_task, "uart_task", 1024, NULL, 5, NULL);
}
