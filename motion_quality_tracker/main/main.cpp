/* Edge Impulse Espressif ESP32 Standalone Inference ESP IDF Example
 * Copyright (c) 2022 EdgeImpulse Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* Include ----------------------------------------------------------------- */
#include <stdio.h>
#include "IMU_driver.h"
#include "mqtt_module.h"

#include "edge-impulse-sdk/classifier/ei_run_classifier.h"

/* Adjust based on the Wi-Fi the computer is connected to.*/
//--------------------------------------------------------
#define WIFI_SSID "Batonga Wi-Fi"
#define WIFI_PASS "77953162"
#define MQTT_BROKER_URI "mqtt://192.168.0.101:1883"
//--------------------------------------------------------

#define DECISION_THRESHOLD (0.45f)

int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
    for(int i = 0; i < EI_CLASSIFIER_RAW_SAMPLE_COUNT; i++)
    {
        float ax = IMU_read_accel(ACCEL_XOUT_H);
        //float ay = IMU_read_accel(ACCEL_YOUT_H); //  Not used for this model
        float az = IMU_read_accel(ACCEL_ZOUT_H);

        float gx = IMU_read_gyro(GYRO_XOUT_H);
        float gy = IMU_read_gyro(GYRO_YOUT_H);
        //float gz = IMU_read_gyro(GYRO_ZOUT_H); //  Not used for this model

        out_ptr[EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME * i + 0] = ax;
        out_ptr[EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME * i + 1] = az;
        out_ptr[EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME * i + 2] = gx;
        out_ptr[EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME * i + 3] = gy;

        ei_sleep((int)(1000 / EI_CLASSIFIER_FREQUENCY));
    }

    return 0;
}

void print_inference_result(ei_impulse_result_t result) {

    // Print how long it took to perform inference
    /*
    ei_printf("Timing: DSP %d ms, inference %d ms, anomaly %d ms\r\n",
            result.timing.dsp,
            result.timing.classification,
            result.timing.anomaly);
    */

    ei_printf("Predictions:\r\n");
    for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++)
    {
        ei_printf("  %s: ", ei_classifier_inferencing_categories[i]);
        ei_printf("%.5f\r\n", result.classification[i].value);
        if(result.classification[i].value > DECISION_THRESHOLD)
        {
            mqtt_publish_exercise_result(ei_classifier_inferencing_categories[i]);
        }
    }

    // Print anomaly result (if it exists)
#if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("Anomaly prediction: %.3f\r\n", result.anomaly);
#endif

}

extern "C" int app_main()
{
    IMU_init();
    ei_sleep(100);
    wifi_init_and_start_mqtt(WIFI_SSID, WIFI_PASS, MQTT_BROKER_URI);
    ei_sleep(2000);

    ei_impulse_result_t result = {nullptr};

    ei_printf("Application satrted!\n");

    while (true)
    {
        signal_t features_signal;
        features_signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
        features_signal.get_data = &raw_feature_get_data;

        // invoke the impulse
        EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false /* debug */);
        if (res != EI_IMPULSE_OK) {
            ei_printf("ERR: Failed to run classifier (%d)\n", res);
            return res;
        }

        print_inference_result(result);
    }
}

