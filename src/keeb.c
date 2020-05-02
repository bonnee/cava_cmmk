#include "keeb.h"

#include <math.h>

//#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#endif

#define KEEB_HEIGHT 6
const struct rgb offcolor = MKRGB(0x000000);

struct cmmk dev;

int keeb_init() {
	dev.cxt = NULL;

	int product;

	if (cmmk_find_device(&product) != 0) {
		return 1;
	}

	if (cmmk_attach(&dev, product, -1) != 0) {
		return 1;
	}

	cmmk_set_control_mode(&dev, CMMK_MANUAL);
	return cmmk_set_all_single(&dev, &offcolor);
}

int keeb_print(unsigned char values[CMMK_COLS_MAX], size_t size) {
	struct cmmk_color_matrix mat;

	for (int row = 0; row < KEEB_HEIGHT; ++row) {
		for (int col = 0; col < size; ++col) {
			float grad = KEEB_HEIGHT - (KEEB_HEIGHT * values[col]) / 255.0F;

			mat.data[row][col] = MKRGBS(fmin(255U, fmax(0, 255U - ((grad - row) * 255U))), 0xFF, 0xFF);
#ifdef DEBUG
			printf("%.2f\t", grad);
#endif
		}
#ifdef DEBUG
		printf("\n");
#endif
	}

	return cmmk_set_leds(&dev, &mat);
}

int keeb_close() {
	cmmk_set_control_mode(&dev, CMMK_FIRMWARE);
	return cmmk_detach(&dev);
}