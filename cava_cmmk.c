#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "keeb.h"

#define CAVA_CMD "cava -p "
#define BAR_N 22

int g_stop = 0;

static void interrupted(int sig) {
	(void)sig;

	g_stop = 1;
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("usage %s <cava_config>\n", argv[0]);

		exit(EXIT_FAILURE);
	}

	signal(SIGINT, interrupted);

	// Run cava
	char cava_cmd[255];
	sprintf(cava_cmd, CAVA_CMD "%s", argv[1]);

	printf("cmd: %s", cava_cmd);

	FILE *stream = popen(cava_cmd, "r");

	if (stream == NULL) {
		fprintf(stderr, "Unable to open stream\n");
		exit(EXIT_FAILURE);
	}

	// Init keeb
	if (keeb_init() != 0) {
		fprintf(stderr, "Unable to load keyboard\n");
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