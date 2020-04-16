#include "keeb.h"

struct rgb offcolor = MKRGB(0x000000);
struct rgb oncolor = MKRGB(0xff0000);

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

	return cmmk_set_control_mode(&dev, CMMK_MANUAL);
}

int keeb_print(unsigned char values[CMMK_COLS_MAX]) {
	struct cmmk_color_matrix mat;

	for (int row = 0; row < CMMK_ROWS_MAX; ++row) {
		for (int col = 0; col < CMMK_COLS_MAX; ++col) {
			int to_fill = CMMK_ROWS_MAX - (float)(CMMK_ROWS_MAX * values[col]) / 255;
			if (to_fill > row) {
				mat.data[row][col] = offcolor;
			} else {
				mat.data[row][col] = oncolor;
			}
		}
	}

	return cmmk_set_leds(&dev, &mat);
}

int keeb_close() {
	cmmk_set_control_mode(&dev, CMMK_FIRMWARE);
	return cmmk_detach(&dev);
}