#ifndef IMU_DRIVER_H
#define IMU_DRIVER_H

#include <string.h>
#include "driver/spi_master.h"
#include "esp_log.h"

// Pin definitions for SPI
#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK 18
#define PIN_NUM_CS 5

#define MAX_TRANSFER_SIZE 512
#define SPI_WRITE_MASK 0X7F
#define SPI_READ_MASK 0X80
 
#define USER_BANK_REG_ADDRESS 0X7F
#define PWR_MGMT_REG_ADDRESS 0X06
#define WAKE_UP_MASK 0X01
#define SLEEP_MASK 0X41

// Accelerometer result registers
#define ACCEL_XOUT_H 0x2D
#define ACCEL_YOUT_H 0X2F
#define ACCEL_ZOUT_H 0X31

// Gyroscope result registers
#define GYRO_XOUT_H 0X33
#define GYRO_YOUT_H 0X35
#define GYRO_ZOUT_H 0X37

#define ACCEL_SENSITIVITY 16384.0  // Sensitivity setting for the accelerometer (+-2g option)
#define GYRO_SENSITIVITY 131.0  // Sensitivity setting for the gyroscope ()

extern spi_device_handle_t spi_handle;  // Declaration of the SPI device handle

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    IMU_SUCCESS = 1,        // Success
    IMU_INIT_SUCCESS = 0,   // Initialization success
    IMU_ERROR = -1,         // Error
} IMU_status_t;

/**
 * @brief Initializes the IMU sensor and SPI communication.
 *
 * This function configures the SPI bus and adds the IMU device to it. 
 * It also wakes up the sensor by writing to the power management register.
 *
 * @return IMU_status_t Returns IMU_INIT_SUCCESS if initialization is successful, otherwise IMU_ERROR.
 */

IMU_status_t IMU_init();

/**
 * @brief Reads a register from the sensor via SPI.
 *
 * This function sends a read command to the specified register address
 * and retrieves the corresponding value from the sensor.
 *
 * @param address The address of the register to read.
 * @return uint8_t The value read from the register.
 */
uint8_t read_register(uint8_t address);

/**
 * @brief Writes a value to a register on the sensor via SPI.
 *
 * @param address The address of the register to write to.
 * @param value The value to write.
 * @return IMU_status_t Returns IMU_SUCCESS if the operation was successful, otherwise IMU_ERROR.
 */
IMU_status_t write_register(uint8_t address, uint8_t value);

/**
 * @brief Reads the accelerometer value from a given register address.
 *
 * @param accel_out_address The address of the accelerometer output register.
 * @return float The acceleration value in g units.
 */
float IMU_read_accel(uint8_t accel_out_address);

/**
 * @brief Reads the gyroscope value from a given register address.
 *
 * @param gyro_out_address The address of the gyroscope output register.
 * @return float The angular velocity in degrees per second (dps).
 */
float IMU_read_gyro(uint8_t gyro_out_address);

#ifdef __cplusplus
}
#endif

#endif