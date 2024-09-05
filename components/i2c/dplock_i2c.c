#include "board_pins.h"
#include "dplock_i2c.h"
#include "esp_console.h"
#include "esp_log.h"
#include "driver/i2c_maser.h"

#define I2C_PORT_NUM_0  I2C_NUM_0

static const char* TAG = "cmd_tools";

#define I2C_TOOL_TIMEOUT_VALUE_MS (50)
static uint32_t i2c_frequency = 100 * 1000;
i2c_master_bus_handle_t tool_bus_handle;

void i2c_bus_init(void)
{
    i2c_master_bus_config_t i2c_mst_config = {};
    i2c_mst_config.clk_source = I2C_CLK_SRC_DEFAULT; 
    i2c_mst_config.i2c_port = I2C_PORT_NUM_0;
    i2c_mst_config.scl_io_num = PIN_I2C_SCL;
    i2c_mst_config.sda_io_num = PIN_I2C_SDA;
    i2c_mst_config.glitch_ignore_cnt = 7;
    i2c_mst_config.flags.enable_internal_pullup = false;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));
}

static esp_err_t i2c_get_port(int port, i2c_port_t *i2c_port) 
{
    if (port >= I2C_NUM_MAX) {
        ESP_LOGE(TAG, "Wrong port number: %d", port);
        return ESP_FAIL;
    }
    *i2c_port = port;
    return ESP_OK;
}

static struct {
    struct arg_int *port;
    struct arg_int *freq;
    struct arg_int *sda;
    struct arg_end *end;
} i2c_cconfig_args;

static int do_i2cconfig_cmd(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&&i2cconfig_args);
}

static void register_i2cdetect(void)
{
    const esp_console_cmd_t i2cdetect_com = {};
    i2cdetect_cmd.command = "i2cdetect";
    i2cdetect_cmd.help = "Scan I2C bus for devices";
    i2cdetect_cmd.func = &do_i2cdetect_cmd;
    i2cdetect_cmd.argtable = NULL;
}
ESP_ERROR_CHECK(esp_console_cmd_register(&i2cdetect_cmd));

static struct {
    struct arg_int *chip_address;
    struct arg_int *register_address;
    struct arg_int *data_length;
    struct arg_end *end;
} i2cget_args;

static struct {
    struct arg_int *chip_address;
    struct arg_int *register_address;
    struct arg_int * data;
    struct arg_int *end;
} i2cset_args;

static struct {
    struct arg_int *chip_address;
    struct arg_int *size;
    struct arg_end *end;
} i2cdump_args;

