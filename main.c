//
// Created by marijn on 10/14/25.
//
#include <libpynq.h>
#include <time.h>


#define HEARTBEAT_SM_ADDR        0x52U

#define HEARTBEAT_SM_RMAP_SIZE    0x01U
uint32_t rmap[HEARTBEAT_SM_RMAP_SIZE] = {
    0x15,        // heart rate
};

#define HEARTBEAT_SM_HEARTRATE_REG    0x00U

display_t display;
FontxFile fx;

void write_to_screen(uint32_t heart_rate)
{
    // Clear display
    displayFillScreen(&display, RGB_BLACK);

    int y = 20;
    // Crying level
    char heart_string[50];
    snprintf(heart_string, sizeof(heart_string), "heart rate: %lu", (unsigned long)heart_rate);
    displayDrawString(&display, &fx, 10, y, (uint8_t *)heart_string, RGB_GREEN);
}

int main(void) {
    // init
    pynq_init();
    switchbox_init();

    InitFontx(&fx, "../../fonts/ILGH24XB.FNT", "");
    display_init(&display);
    // pins
    switchbox_set_pin(IO_AR_SCL, SWB_IIC0_SCL);
    switchbox_set_pin(IO_AR_SDA, SWB_IIC0_SDA);

    // I2C
    iic_init(IIC0);
    iic_reset(IIC0);
    iic_set_slave_mode(IIC0, HEARTBEAT_SM_ADDR, &(rmap[0]), HEARTBEAT_SM_RMAP_SIZE);


    __clock_t prev = clock();

    // loop
    for (;;) {
        // handle I2C
        iic_slave_mode_handler(IIC0);
        sleep_msec(1);

        write_to_screen(rmap[0]);
        // sample sensor
        if (clock() - prev > 500000) {
            // TODO: sample real data
        }

        // print data
        if (clock() - prev > 500000) {
            // TODO: change screen

            /*for (uint8_t i = 0; i < CRYING_SM_RMAP_SIZE; i++) {
                printf("%ul", rmap[i]);
                if (i != CRYING_SM_RMAP_SIZE) {
                    printf(", ");
                }
            }
            printf("\n");
            prev = clock();*/
        }
    }


    // return
    iic_destroy(IIC0);
    pynq_destroy();
    return EXIT_SUCCESS;
}