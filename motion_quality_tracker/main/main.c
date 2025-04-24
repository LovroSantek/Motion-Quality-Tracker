#include "mqtt_module.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Adjust based on the Wi-Fi the computer is connected to.*/
//--------------------------------------------------------
#define WIFI_SSID "Batonga Wi-Fi"
#define WIFI_PASS "77953162"
#define MQTT_BROKER_URI "mqtt://192.168.0.101:1883"
//--------------------------------------------------------

void publish_task(void *pvParameters)
{
    while (1)
    {
        mqtt_publish_exercise_result(true);
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

void app_main(void) {
    wifi_init_and_start_mqtt(WIFI_SSID, WIFI_PASS, MQTT_BROKER_URI);

    xTaskCreate(&publish_task, "publish_task", 4096, NULL, 5, NULL);
}
