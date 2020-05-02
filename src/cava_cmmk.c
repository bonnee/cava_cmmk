#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "keeb.h"

#define BAR_N 22

#define CAVA_CMD "cava "
#define CAVA_OPT "-p "

int end = 0;

static void quit(int sig, siginfo_t *siginfo, void *context) {
	end = 1;
}

int make_conf(char *fname) {
	// Create temp file
	char temp_name[] = "/tmp/cava_cmmk_XXXXXX";
	int fd = mkstemp(temp_name);
	if (fd == -1) {
		return -1;
	}

	// Open temp file
	FILE *tmp_conf = fdopen(fd, "w");
	if (tmp_conf == NULL) {
		return -1;
	}

	// If fname contains something, open it and copy its contents to temp file
	if (fname[0] != '\0') {
		FILE *conf = fopen(fname, "r");
		if (conf == NULL) {
			return -1;
		}

		char buffer[256];
		while (fgets(buffer, sizeof(buffer), conf)) {
			fprintf(tmp_conf, "%s", buffer);
		}

		fclose(conf);
	}

	// Write default parameters at the end, so that they have more priority than the user configuration
	fprintf(tmp_conf,
			"[general]\n"
			"\tbars = %d\n"
			"\tframerate = 50\n"
			"[output]\n"
			"\tmethod = raw\n"
			"\tbit_format = 8bit\n"
			"\tdata_format = binary\n",
			BAR_N);

	fclose(tmp_conf);

	// We want to return the path to temp file
	strcpy(fname, temp_name);
}

int main(int argc, char *argv[]) {
	struct sigaction act;
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = quit;
	sigaction(SIGINT, &act, 0);
	sigaction(SIGHUP, &act, 0);
	sigaction(SIGTERM, &act, 0);

	if (argc > 2) {
		printf("usage: %s [CONF]\n", argv[0]);

		exit(EXIT_FAILURE);
	}

	// Create cava config file
	char cava_cmd[255];
	char file_name[255] = {'\0'};

	// If a conf file is provided, copy it to file_name
	if (argc == 2) {
		strcpy(file_name, argv[1]);
	}

	if (make_conf(file_name) == -1) {
		fprintf(stderr, "Error creating cava configuration file '%s'\n", file_name);
		exit(EXIT_FAILURE);
	}

	// Run cava
	sprintf(cava_cmd, "%s%s%s", CAVA_CMD, CAVA_OPT, file_name);
	printf("running '%s'\n", cava_cmd);
	fflush(stdout);

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

	while (!feof(stream) && !end) {
		unsigned char c[BAR_N] = {'\0'};
		fread(c, 1, BAR_N, stream);

		keeb_print(c, BAR_N);
	}

	printf("\nExiting\n");

	unlink(file_name);	// Delete temp file
	pclose(stream);
	keeb_close();

	return 0;
}
