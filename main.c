#include <libpynq.h>
#include <time.h>

#define CRYING_SM_ADDR        0x53U
#define CRYING_SM_RMAP_SIZE   0x01U

uint32_t rmap[CRYING_SM_RMAP_SIZE] = {
    0x0UL,
};

#define CRYING_SM_LEVEL_REG   0x00U

int main(void) {
    pynq_init();

    switchbox_set_pin(IO_AR_SCL, SWB_IIC0_SCL);
    switchbox_set_pin(IO_AR_SDA, SWB_IIC0_SDA);

    iic_init(IIC0);
    iic_reset(IIC0);
    iic_set_slave_mode(IIC0, CRYING_SM_ADDR, rmap, CRYING_SM_RMAP_SIZE);

    __clock_t prev = clock();

    for (;;) {
        iic_slave_mode_handler(IIC0);
        sleep_msec(1);

        // Check if 50 ms have passed
        if (clock() - prev > (CLOCKS_PER_SEC / 20)) {  // CLOCKS_PER_SEC / 20 = 50 ms
            rmap[0] += 10;
            prev = clock();
        }
    }
    
    iic_destroy(IIC0);
    pynq_destroy();
    return EXIT_SUCCESS;
}

 
