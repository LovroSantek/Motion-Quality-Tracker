#include "mqtt_module.h"

#include <string.h>
#include <stdio.h>

#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "mqtt_client.h"
#include "esp_event.h"

#define RESULT_TOPIC "motion_tracker/exercise_result"

static const char *TAG_WIFI = "WIFI(mqtt_module)";
static const char *TAG_MQTT = "MQTT(mqtt_module)";
static const char *TAG = "MQTT";


static esp_mqtt_client_handle_t mqtt_client;

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        ESP_LOGI(TAG_WIFI, "Disconnected from WiFi, retrying...");
        esp_wifi_connect();
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG_WIFI, "Got IP:" IPSTR, IP2STR(&event->ip_info.ip));
        ESP_LOGI(TAG_WIFI, "Successfully connected to Wi-Fi!");
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    //esp_mqtt_event_handle_t event = event_data;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_CONNECTED");
        //esp_mqtt_client_publish(event->client, "test/topic", "Success!", 0, 1, 0);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_DISCONNECTED");
        break;
    default:
        break;
    }
}

void wifi_init_and_start_mqtt(const char *ssid, const char *password, const char *broker_uri)
{
    ESP_LOGI(TAG, "Started Wi-Fi and MQTT initalization.");

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        nvs_flash_erase();
        nvs_flash_init();
    }

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    
    wifi_config_t wifi_config = {0};
    strcpy((char *)wifi_config.sta.ssid, ssid);
    strcpy((char *)wifi_config.sta.password, password);

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL);

    esp_wifi_start();

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = broker_uri,
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, mqtt_client);
    esp_mqtt_client_start(mqtt_client);
}

esp_err_t mqtt_publish_exercise_result(const char* category) {
    if (mqtt_client == NULL) {
        ESP_LOGE(TAG, "MQTT client is not initialized.");
        return ESP_FAIL;
    }

    const char* ei_classifier_inferencing_categories[] = { "Correct Squat", "Fast Squat", "Idle", "Incorrect Squat" };

    bool is_valid = false;
    for (int i = 0; i < sizeof(ei_classifier_inferencing_categories) / sizeof(ei_classifier_inferencing_categories[0]); i++) {
        if (strcmp(category, ei_classifier_inferencing_categories[i]) == 0) {
            is_valid = true;
            break;
        }
    }

    if (!is_valid) {
        esp_mqtt_client_publish(mqtt_client, RESULT_TOPIC, "unknown category", 0, 1, 0);
        return ESP_FAIL;
    }

    int msg_id = esp_mqtt_client_publish(mqtt_client, RESULT_TOPIC, category, 0, 1, 0);

    if (msg_id < 0) {
        ESP_LOGE(TAG, "Failed to publish message: %s", category);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Published message: %s (msg_id=%d)", category, msg_id);
    return ESP_OK;
}
