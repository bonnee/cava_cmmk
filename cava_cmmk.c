#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "keeb.h"

#define BAR_N 22

int g_stop = 0;

static void interrupted(int sig) {
	(void)sig;

	g_stop = 1;
}

int main(int argc, char *argv[]) {
	signal(SIGINT, interrupted);

	if (keeb_init() != 0) {
		fprintf(stderr, "Unable to load keyboard\n");
		exit(EXIT_FAILURE);
	}

	// Run cava
	FILE *stream = popen("cava", "r");

	if (stream == NULL) {
		fprintf(stderr, "Unable to open stream\n");
		exit(EXIT_FAILURE);
	}

	while (!feof(stream) && !g_stop) {
		unsigned char c[BAR_N] = {'\0'};
		fread(c, 1, BAR_N, stream);

		keeb_print(c);
	}

	printf("Exiting\n");
	pclose(stream);
	keeb_close();

	return 0;
}