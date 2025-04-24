#include "IMU_driver.h"

spi_device_handle_t spi_handle;

IMU_status_t IMU_init()
{
    spi_bus_config_t buscfg = {};
    buscfg.mosi_io_num = PIN_NUM_MOSI;
    buscfg.miso_io_num = PIN_NUM_MISO;
    buscfg.sclk_io_num = PIN_NUM_CLK;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
    buscfg.max_transfer_sz = MAX_TRANSFER_SIZE;

    esp_err_t result = spi_bus_initialize(VSPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (result != ESP_OK)
    {
        ESP_LOGE("SPI", "Failed to initialize bus: %s", esp_err_to_name(result));
        return IMU_ERROR;
    }

    spi_device_interface_config_t devcfg = {};
    devcfg.clock_speed_hz = 1000000;  // Clock 1 MHz
    devcfg.mode = 0;  // SPI mode 0
    devcfg.spics_io_num = PIN_NUM_CS;
    devcfg.queue_size = 1;

    result = spi_bus_add_device(VSPI_HOST, &devcfg, &spi_handle);
    if (result != ESP_OK)
    {
        ESP_LOGE("SPI", "Failed to setup SPI: %s", esp_err_to_name(result));
        spi_bus_free(VSPI_HOST);
        return IMU_ERROR;
    }
    else
    {
        ESP_LOGI("SPI", "SPI setup successful");
    }

    if (write_register(PWR_MGMT_REG_ADDRESS, WAKE_UP_MASK) == IMU_SUCCESS)
    {
        ESP_LOGI("SPI: ", "Sensor is no longer in sleep mode");
        return IMU_INIT_SUCCESS;
    }
    else
    {
        return IMU_ERROR;
    }
}

uint8_t read_register(uint8_t address)
{
    uint8_t send_buf[2] = {(uint8_t)(SPI_READ_MASK | address), (uint8_t)0x00};
    uint8_t recv_buf[2] = {0};

    spi_transaction_t t = {};
    t.length = 16;             // Total bits to transfer
    t.tx_buffer = send_buf;    // Transmit buffer
    t.rx_buffer = recv_buf;    // Receive buffer

    esp_err_t ret = spi_device_transmit(spi_handle, &t);
    if (ret != ESP_OK) {
        ESP_LOGE("SPI", "Failed to transmit: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI("SPI", "Received: 0x%02X", recv_buf[1]);
    }

return recv_buf[1];

}

IMU_status_t write_register(uint8_t address, uint8_t value)
{
    uint8_t tx_data[2];
    tx_data[0] = address & 0x7F;  // Postavljamo MSB na 0 za pisanje
    tx_data[1] = value;

    spi_transaction_t transaction = {
        .length = 16,  // 2 bajta = 16 bitova
        .tx_buffer = tx_data,
        .rx_buffer = NULL,
    };

    esp_err_t result = spi_device_transmit(spi_handle, &transaction);
    if (result != ESP_OK)
    {
        ESP_LOGE("SPI", "Failed to write to sensor: %s", esp_err_to_name(result));
        return IMU_ERROR;
    }
    else
    {
        ESP_LOGI("SPI", "Data 0x%X written to register 0x%X successfully", value, address);
        return IMU_SUCCESS;
    }
}

float IMU_read_accel(uint8_t accel_out_address)
{
    uint8_t tx_data_high[2] = {(uint8_t)(SPI_READ_MASK | accel_out_address), (uint8_t)0x00};
    uint8_t rx_data_high[2] = {0};
    
    spi_transaction_t trans_high = {};
    trans_high.length = 16;
    trans_high.tx_buffer = tx_data_high;
    trans_high.rx_buffer = rx_data_high;
    spi_device_transmit(spi_handle, &trans_high);

    uint8_t tx_data_low[2] = {(uint8_t)(SPI_READ_MASK | (accel_out_address + 1)), (uint8_t)0x00};
    uint8_t rx_data_low[2] = {0};

    spi_transaction_t trans_low = {};
    trans_low.length = 16;
    trans_low.tx_buffer = tx_data_low;
    trans_low.rx_buffer = rx_data_low;
    spi_device_transmit(spi_handle, &trans_low);

    int16_t accel = ((int16_t)rx_data_high[1] << 8) | rx_data_low[1];

    return accel / ACCEL_SENSITIVITY;
}

float IMU_read_gyro(uint8_t gyro_out_address)
{
    uint8_t tx_data_high[2] = {(uint8_t)(SPI_READ_MASK | gyro_out_address), (uint8_t)0x00};
    uint8_t rx_data_high[2] = {0};
    
    spi_transaction_t trans_high = {};
    trans_high.length = 16;
    trans_high.tx_buffer = tx_data_high;
    trans_high.rx_buffer = rx_data_high;
    spi_device_transmit(spi_handle, &trans_high);

    uint8_t tx_data_low[2] = {(uint8_t)(SPI_READ_MASK | (gyro_out_address + 1)), (uint8_t)0x00};
    uint8_t rx_data_low[2] = {0};

    spi_transaction_t trans_low = {};
    trans_low.length = 16;
    trans_low.tx_buffer = tx_data_low;
    trans_low.rx_buffer = rx_data_low;
    spi_device_transmit(spi_handle, &trans_low);

    int16_t gyro_raw = ((int16_t)rx_data_high[1] << 8) | rx_data_low[1];

    return gyro_raw / GYRO_SENSITIVITY;
}