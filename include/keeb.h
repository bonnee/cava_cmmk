#ifndef KEEB_H_
#define KEEB_H_

#include <libcmmk/libcmmk.h>

int keeb_init();
int keeb_print(unsigned char values[CMMK_COLS_MAX]);
int keeb_close();

#endif