#ifndef MQTT_MODULE_H
#define MQTT_MODULE_H

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes Wi-Fi and connects to the specified MQTT broker.
 *
 * This function connects the ESP32 to the given Wi-Fi network and starts the MQTT client
 * using the specified broker URI (e.g., "mqtt://192.168.0.123:1883").
 *
 * @param ssid Wi-Fi network SSID
 * @param password Wi-Fi password
 * @param broker_uri MQTT broker URI (including protocol and port)
 */
void wifi_init_and_start_mqtt(const char *ssid, const char *password, const char *broker_uri);

/**
 * @brief Publishes the result of exercise evaluation via MQTT.
 *
 * This function sends a message over MQTT indicating whether the exercise
 * was performed correctly or incorrectly. The message is published to a
 * predefined topic.
 *
 * @param correct A boolean indicating exercise correctness.
 *                - true: exercise performed correctly
 *                - false: exercise performed incorrectly
 *
 * @return
 *      - ESP_OK on success
 * @return
 *      - ESP_FAIL on publish error
 */
esp_err_t mqtt_publish_exercise_result(bool correct);

#ifdef __cplusplus
}
#endif

#endif
