// TODO: Proper copyright. Credit for MiSTer_SAM.

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#include <libudev.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include <unistd.h>
#include <fcntl.h>

#include <linux/limits.h>
//#include <linux/input.h>
#include <linux/hidraw.h>

#include "usb_hid_scancodes.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

// TODO: More effort on this comment.
// Copyright (C) 2008-2009 Kay Sievers <kay@vrfy.org>
// GPLv2
// udevadm-util.c
static struct udev_device *find_device(struct udev *ud, const char *path)
{
	struct stat statbuf;
	char type;

	if (stat(path, &statbuf) < 0) {
		return NULL;
	}

	if (S_ISBLK(statbuf.st_mode)) {
		type = 'b';
	} else if (S_ISCHR(statbuf.st_mode)) {
		type = 'c';
	} else {
		return NULL;
	}

	return udev_device_new_from_devnum(ud, type, statbuf.st_rdev);
}

static int is_eventx(const struct dirent *dir) {
	int fd;

	if (strncmp(dir->d_name, "event", 5)) {
		return 0;
	}

	return 1;
}

static int is_hidraw(const struct dirent *dir) {
	int fd;

	if (strncmp(dir->d_name, "hidraw", 6)) {
		return 0;
	}

	return 1;
}

int keyboard_init()
{
	static char buffer[2048];
	static char command[256];

	int fd, i, j;
	FILE *fp;
	struct dirent **namelist;
	struct dirent *dir;
	int eventDevices = 0;
	int hidrawDevices = 0;
	struct udev *ud = udev_new();

	int possibleKeyboards = 0;
	char **physicalAddresses;

	int matchedKeyboards = 0;
	char **hidrawPaths;

	if(!ud) {
		return 1;
	}

	eventDevices = scandir("/dev/input", &namelist, is_eventx, NULL);

	physicalAddresses = malloc(sizeof(*physicalAddresses) * eventDevices); // Overallocate, then shrink later.

	for (i = 0; i < eventDevices; i++) {
		struct udev_device *device;
		size_t addressLength;

		snprintf(buffer, sizeof(buffer) - 1, "/dev/input/%s", namelist[i]->d_name);
		//printf("Device: %s\n", buffer);

		device = find_device(ud, *&buffer);
		if (!device) {
			continue;
		}

		const char *kb = udev_device_get_property_value(device, "ID_INPUT_KEYBOARD");
		if(!kb || !*kb || *kb != '1') {
			continue;
		}

		const char *dp = udev_device_get_property_value(device, "DEVPATH");
		if(dp && *dp && strstr(dp, "/virtual/")) {
			continue;
		}

		//printf(" - Syspath: %s\n", udev_device_get_syspath(device));
		snprintf(buffer, sizeof(buffer) - 1, "%s/device/uevent", udev_device_get_syspath(device));
		//printf(" - File: %s\n", buffer);

		fp = fopen(buffer, "r");
		if(!fp) {
			continue;
		}

		size_t n = 0;
		for(char *line = NULL; getline(&line, &n, fp) != -1 && line; line = NULL) {
			size_t lineLength = strlen(line);
			if (lineLength > 5 && strncmp(line, "PHYS", 4) == 0) {
				if (line[lineLength - 1] == '\n') {
					line[lineLength - 1] = 0;
				}
				if (line[lineLength - 2] == '"') {
					line[lineLength - 2] = 0;
				}

				char *physicalAddress = line + 5;

				if (*physicalAddress == '"') {
					physicalAddress++;
				}

				addressLength = MIN(strlen(physicalAddress) + 1, PATH_MAX + 1);
				physicalAddresses[possibleKeyboards] = malloc(addressLength);

				strncpy(physicalAddresses[possibleKeyboards], physicalAddress, addressLength - 1);
				physicalAddresses[possibleKeyboards][addressLength - 1] = 0;
				//printf("%s\n", physicalAddresses[possibleKeyboards]);
				possibleKeyboards++;
			}
			free(line);
		}

		fclose(fp);
	}

	for (i = 0; i < eventDevices; i++) {
		free(namelist[i]);
	}
	free(namelist);

	if (possibleKeyboards) {
		physicalAddresses = realloc(physicalAddresses, sizeof(*physicalAddresses) * possibleKeyboards);
	} else {
		free(physicalAddresses);
		return 1;
	}

	//printf("\n");

	hidrawDevices = scandir("/dev/", &namelist, is_hidraw, NULL);

	hidrawPaths = malloc(sizeof(*hidrawPaths) * possibleKeyboards); // Overallocate, then shrink later.

	for (i = 0; i < hidrawDevices; i++) {
		snprintf(buffer, sizeof(buffer) - 1, "/sys/class/hidraw/%s/device/uevent", namelist[i]->d_name);
		//printf("Device: %s\n", buffer);
		fp = fopen(buffer, "r");
		if (!fp) {
			// TODO: Remove from list.
			continue;
		}

		size_t n = 0;
		for (char *line = NULL; getline(&line, &n, fp) != -1 && line; line = NULL) {
			size_t lineLength = strlen(line);
			if (lineLength > 9 && strncmp(line, "HID_PHYS", 8) == 0) {
				if(line[lineLength - 1] == '\n') {
					line[lineLength - 1] = 0;
				}

				char *physicalAddress = line + 9;
				for (j = 0; j < possibleKeyboards; j++) {
					if (strncmp(physicalAddresses[j], physicalAddress, strlen(physicalAddresses[j])) == 0) {
						//printf(" - File: %s\n", physicalAddress);
						size_t pathLength = MIN(strlen(buffer) + 1, PATH_MAX + 1);
						snprintf(buffer, sizeof(buffer) - 1, "/dev/%s", namelist[i]->d_name);
						hidrawPaths[matchedKeyboards] = malloc(pathLength);
						strncpy(hidrawPaths[matchedKeyboards], buffer, pathLength - 1);
						hidrawPaths[matchedKeyboards][pathLength - 1] = 0;
						matchedKeyboards++;
						break;
					}
				}
			}
			free(line);
		}

		fclose(fp);
	}

	for (i = 0; i < hidrawDevices; i++) {
		free(namelist[i]);
	}
	free(namelist);

	// We're done with the physical keyboard addresses.
	for (i = 0; i < possibleKeyboards; i++) {
		free(physicalAddresses[i]);
	}
	free(physicalAddresses);

	if (matchedKeyboards) {
		hidrawPaths = realloc(hidrawPaths, sizeof(*hidrawPaths) * matchedKeyboards);
	} else {
		free(hidrawPaths);
		return 1;
	}

	printf("Matched Keyboard Paths:\n");
	for (i = 0; i < matchedKeyboards; i++) {
		printf(" - %s\n", hidrawPaths[i]);
	}

	// TODO: Only select a single keyboard?

	int openEventFiles = 0;
	int *eventFiles = malloc(matchedKeyboards * sizeof(*eventFiles));
	for (i = 0; i < matchedKeyboards; i++) {
		if((eventFiles[openEventFiles] = open(hidrawPaths[i], O_RDONLY|O_NONBLOCK)) >= 0) {
			openEventFiles++;
		}
	}
	eventFiles = realloc(eventFiles, openEventFiles * sizeof(*eventFiles));

	// TODO: I think if poll or select are used then we don't have to keep these handles open constantly?
	//       May be a decent idea to try pull off.
	unsigned char **lastScancodes = calloc(openEventFiles, sizeof(char *));
	unsigned char currScancodes[256];
	unsigned char currModifiers[8];
	while(1) {
		for (i = 0; i < openEventFiles; i++) {
			if(eventFiles[i] == (size_t)-1) {
				continue;
			}

			unsigned char events[1024];

			// IMPORTANT: USB HID keyboards start with a byte of modifiers, a reserved byte, then scancodes.

			int event = 0;
			ssize_t count = 0;
			if ((count = read(eventFiles[i], events, sizeof(events))) < 0) {
				if (!lastScancodes[i]) {
					lastScancodes[i] = calloc(256, 1);
				}
				continue;
			}

			memset(currScancodes, 0, sizeof(currScancodes));

			if (count > 0) {
				int change = 0;

				if (!lastScancodes[i]) {
					lastScancodes[i] = calloc(256, 1);
					for (ssize_t currByte = 2; currByte < count; currByte++) {
						lastScancodes[i][events[currByte]] = 1;
					}
					continue;
				}

				for (ssize_t currByte = 2; currByte < count; currByte++) {
					currScancodes[events[currByte]] = 1;
				}

				for (j = 1; j < 256; j++) {
					if (lastScancodes[i][j] != currScancodes[j]) {
						printf("%s: %d->%d. ", SCANCODE_NAMES[j], lastScancodes[i][j], currScancodes[j]);
						lastScancodes[i][j] = currScancodes[j];
						change = 1;
					}
				}

				if (change) {
					printf("\n");
				}
			}
		}
	}

	for (i = 0; i < openEventFiles; i++) {
		close(eventFiles[i]);
	}
	free(eventFiles);

	// We're done with the matched keyboard paths
	for (i = 0; i < matchedKeyboards; i++) {
		free(hidrawPaths[i]);
	}
	free(hidrawPaths);

	// TODO: Return 0 after we're done testing.
	return 1;
}
