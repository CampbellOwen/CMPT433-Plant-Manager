#include "../include/general.h"
#include <time.h>

static _Bool isShuttingDown = false;

void General_shutdown(void) {
	isShuttingDown = true;
}

_Bool General_isShuttingDown(void) {
	return isShuttingDown;
}

void die_on_failed(char *msg) {
	if (errno < 0) {
		perror(msg);
		exit(-1);
	}
}

void sleep_usec(long usec) {
	struct timespec sleep_time;
	sleep_time.tv_sec = (usec / 1000000);
	sleep_time.tv_nsec = (usec % 1000000) * 1000;
	nanosleep(&sleep_time, NULL);
}

void sleep_msec(long msec) {
	sleep_usec(msec * 1000);
}

_Bool areStringsSame(char* str1, const char* str2) {
	return strncmp(str1, str2, strlen(str2)) == 0;
}

void WriteToFile(char* filename, char* value) {
	FILE* pfile = fopen((filename), "w");
	if (pfile == NULL) {
		printf("ERROR OPENING %s.\n", filename);
	}

	int charWritten = fprintf(pfile, "%s", value);
	if (charWritten <= 0) {
		printf("ERROR WRITING DATA\n");
	}

	fclose(pfile);
}

char* ReadFile(char* filename) {
	FILE* pfile = fopen(filename, "r");
	if (pfile == NULL) {
		printf("ERROR: Unable to open file (%s) for read\n", filename);
	}

	const int max_length = 1024;
	char* buff = malloc(max_length * sizeof(char));
	fgets(buff, max_length, pfile);

	fclose(pfile);
	return buff;
}
