#include <libpynq.h>
#include <time.h>


#define CRYING_SM_ADDR		0x53U

#define CRYING_SM_RMAP_SIZE	0x01U
uint32_t rmap[CRYING_SM_RMAP_SIZE] = {
    0x63727921UL,		// crying intensity
};

#define CRYING_SM_LEVEL_REG	0x00U



int main(void) {
    // init
    pynq_init();
    switchbox_init();

    // pins
    switchbox_set_pin(IO_PMODA3, SWB_IIC0_SCL);
    switchbox_set_pin(IO_PMODA4, SWB_IIC0_SDA);

    // I2C
    iic_init(IIC0);
    iic_reset(IIC0);
    iic_set_slave_mode(IIC0, CRYING_SM_ADDR, &(rmap[0]), CRYING_SM_RMAP_SIZE);


    __clock_t prev = clock();

    // loop
    for (;;) {
        // handle I2C
		iic_slave_mode_handler(IIC0);

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
