#include "mqtt_module.h"

/* Adjust based on the Wi-Fi computer is connected to.*/
#define WIFI_SSID "Batonga Wi-Fi"
#define WIFI_PASS "77953162"
#define MQTT_BROKER_URI "mqtt://192.168.0.101:1883"

void app_main(void) {
    wifi_init_and_start_mqtt(WIFI_SSID, WIFI_PASS, MQTT_BROKER_URI);
    //mqtt_publish_exercise_result(true);
}
