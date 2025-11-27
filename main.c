//
// Created by marijn on 10/14/25.
//
#include <libpynq.h>
#include <time.h>


#define CRYING_SM_ADDR		0x53U

#define CRYING_SM_RMAP_SIZE	0x01U
uint32_t rmap[CRYING_SM_RMAP_SIZE] = {
	0x0UL,		// crying level
};

#define CRYING_SM_LEVEL_REG	0x00U


// buffer 1s of measurements
#define ADC_BUFFER_SIZE 1000
// from measurements
#define CRYING_LEVEL_MIN 1000

uint16_t ADC_sample = 0;
uint32_t ADC_buffer[ADC_BUFFER_SIZE] = {};
uint32_t ADC_smoothed[ADC_BUFFER_SIZE] = {};
uint32_t crying_level = 0;


display_t display;
FontxFile fx;

void write_to_screen(uint32_t crying_level);

inline uint32_t rmoffset(const uint32_t a, const uint32_t min);
void smooth(const uint32_t* src, uint32_t* dst, const uint32_t size, const uint8_t kernel_size);
uint32_t max(const uint32_t* buffer, const uint32_t size);



int main(void) {
	// init
	pynq_init();
	switchbox_init();

	// pins
	switchbox_set_pin(IO_AR_SCL, SWB_IIC0_SCL);
	switchbox_set_pin(IO_AR_SDA, SWB_IIC0_SDA);

	// init display
    InitFontx(&fx, "../../fonts/ILGH24XB.FNT", "");
    display_init(&display);

	// I2C
	iic_init(IIC0);
	iic_reset(IIC0);
	iic_set_slave_mode(IIC0, CRYING_SM_ADDR, &(rmap[0]), CRYING_SM_RMAP_SIZE);

	// ADC
	adc_init();

	// loop
	for (;;) {
		// handle I2C
		iic_slave_mode_handler(IIC0);
		sleep_msec(1);

		// sample sensor
		ADC_buffer[ADC_sample] = rmoffset(adc_read_channel_raw(ADC0), CRYING_LEVEL_MIN);
		ADC_sample = (ADC_sample + 1) % ADC_BUFFER_SIZE;

		// find peak intensity
		if (!ADC_sample) {
			smooth(ADC_buffer, ADC_smoothed, ADC_BUFFER_SIZE, 3);
			crying_level = max(ADC_smoothed, ADC_BUFFER_SIZE);
			rmap[CRYING_SM_LEVEL_REG] = crying_level;

			// show result
            write_to_screen(crying_level);
			printf("\rcrying_level: %d", crying_level);
			fflush(stdout);
		}
	}

	// return
	iic_destroy(IIC0);
	pynq_destroy();
	return EXIT_SUCCESS;
}


void write_to_screen(uint32_t crying_level) {
    // Clear display
    displayFillScreen(&display, RGB_BLACK);

    int y = 20;

    // Crying level
    char crying_string[50];
    snprintf(crying_string, sizeof(crying_string), "crying level: %lu", (unsigned long)crying_level);
    displayDrawString(&display, &fx, 10, y, (uint8_t *)crying_string, RGB_GREEN);
}



inline uint32_t rmoffset(const uint32_t a, const uint32_t min) {
	return ((a > min) ? (a - min) : 0);
}

void smooth(const uint32_t* src, uint32_t* dst, const uint32_t size, const uint8_t kernel_size) {
	// simple running avg to smooth out outlier peaks
	uint8_t kernel_runout = kernel_size >> 1;
	uint32_t sum;
	for (uint32_t i = 0; i < size; i++) {
		sum = 0;
		for (int32_t j = (i - kernel_runout); j < (i + kernel_runout); j++) {
			if ((j >= 0) || (j < size)) { sum += src[j]; }
		}
		dst[i] = (uint32_t)(((float)sum) / ((float)kernel_size));
	}
}

uint32_t max(const uint32_t* buffer, const uint32_t size) {
	uint32_t maximum = 0;
	for (uint32_t i = 0; i < size; i++) {
		if (buffer[i] > maximum) { maximum = buffer[i]; }
	}
	return maximum;
}

