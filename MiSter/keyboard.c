 /*
 * keyboard.c Version 1.0
 *
 * Copyright (c) 2026 Max Waine
 * Copyright (c) 2026 RetroSpy Technologies
 *
 * find_device is taken from udevadm-util.c.
 * collect_physical_keyboard_addresses, and find_matching_hidraw_devices are
 * based on code from MiSTer_SAM's MiSTer_SAM_MCP.
 * Please see those functions below for their copyright notices.
 */
 
 /*
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; if not, write to the Free Software
  * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  * 02110-1301 USA.
  */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <unistd.h>
#include <linux/limits.h>
#include <linux/hidraw.h>

#include <libudev.h>

#include "usb_hid_scancodes.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static int openEventFiles = 0;
static int *eventFiles = NULL;

static unsigned char **lastScancodes = NULL;
static unsigned char **lastModifiers = NULL;

// Taken from udevadm-util.c
// Copyright (C) 2008-2009 Kay Sievers <kay@vrfy.org>
// Used under terms of the GPLv2+.
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

//
// Accept dirents that start with "event".
//
static int is_eventx(const struct dirent *dir) {
	return strncmp(dir->d_name, "event", 5) ? 0 : 1;
}

//
// Collects physical keyboard addresses.
// Adapted from MiSTer_SAM's MiSTer_SAM_MCP.
// https://github.com/mrchrisster/MiSTer_SAM/
// Copyright (c) 2023 by mrchrisster and Mellified
// Used under terms of the GPLv3+ (see copyright notice at top of file).
//
static int collect_physical_keyboard_addresses(char ***physicalAddressesOut, int *possibleKeyboardsOut)
{
	char buffer[2048] = {0};

	struct udev *ud = udev_new();

	struct dirent **namelist;
	int eventDevices = scandir("/dev/input", &namelist, is_eventx, NULL);

	int possibleKeyboards = 0;
	char **physicalAddresses = malloc(sizeof(*physicalAddresses) * eventDevices); // Overallocate, then shrink later.

	for (int i = 0; i < eventDevices; i++) {
		struct udev_device *device;

		snprintf(buffer, sizeof(buffer) - 1, "/dev/input/%s", namelist[i]->d_name);
		//printf("Device: %s\n", buffer);

		device = find_device(ud, *&buffer);
		if (!device) {
			continue;
		}

		const char *kb = udev_device_get_property_value(device, "ID_INPUT_KEYBOARD");
		if (!kb || !*kb || *kb != '1') {
			continue;
		}

		const char *dp = udev_device_get_property_value(device, "DEVPATH");
		if (dp && *dp && strstr(dp, "/virtual/")) {
			continue;
		}

		//printf(" - Syspath: %s\n", udev_device_get_syspath(device));
		snprintf(buffer, sizeof(buffer) - 1, "%s/device/uevent", udev_device_get_syspath(device));
		//printf(" - File: %s\n", buffer);

		FILE *fp = fopen(buffer, "r");
		if (!fp) {
			continue;
		}

		size_t n = 0;
		for (char *line = NULL; getline(&line, &n, fp) != -1 && line; line = NULL) {
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

				const size_t addressLength = MIN(strlen(physicalAddress) + 1, PATH_MAX + 1);
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

	for (int i = 0; i < eventDevices; i++) {
		free(namelist[i]);
	}
	free(namelist);

	*possibleKeyboardsOut = possibleKeyboards;

	if (possibleKeyboards) {
		physicalAddresses = realloc(physicalAddresses, sizeof(*physicalAddresses) * possibleKeyboards);
	} else {
		// Don't need to assign to *physicalAddressesOut since we should bail out from further KB work.
		free(physicalAddresses);
		return 1;
	}

	*physicalAddressesOut = physicalAddresses;

	return 0;
}

//
// Accept dirents that start with "hidraw".
//
static int is_hidraw(const struct dirent *dir) {
	return strncmp(dir->d_name, "hidraw", 6) ? 0 : 1;
}

//
// Takes in the physical addresses and quantity of them, and tries to find matching hidraw devices.
// Adapted from MiSTer_SAM's MiSTer_SAM_MCP.
// https://github.com/mrchrisster/MiSTer_SAM/
// Copyright (c) 2023 by mrchrisster and Mellified
// Used under terms of the GPLv3+ (see copyright notice at top of file).
//
static int find_matching_hidraw_devices(char **physicalAddresses, int possibleKeyboards, char ***hidrawPathsOut, int *matchedKeyboardsOut)
{
	char buffer[2048] = {0};

	struct dirent **namelist;

	int hidrawDevices = scandir("/dev/", &namelist, is_hidraw, NULL);

	int matchedKeyboards = 0;
	char **hidrawPaths = malloc(sizeof(*hidrawPaths) * possibleKeyboards); // Overallocate, then shrink later.

	for (int i = 0; i < hidrawDevices; i++) {
		snprintf(buffer, sizeof(buffer) - 1, "/sys/class/hidraw/%s/device/uevent", namelist[i]->d_name);
		//printf("Device: %s\n", buffer);
		FILE *fp = fopen(buffer, "r");
		if (!fp) {
			// TODO: Remove from list.
			continue;
		}

		size_t n = 0;
		for (char *line = NULL; getline(&line, &n, fp) != -1 && line; line = NULL) {
			size_t lineLength = strlen(line);
			if (lineLength > 9 && strncmp(line, "HID_PHYS", 8) == 0) {
				if (line[lineLength - 1] == '\n') {
					line[lineLength - 1] = 0;
				}

				char *physicalAddress = line + 9;
				for (int j = 0; j < possibleKeyboards; j++) {
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

	for (int i = 0; i < hidrawDevices; i++) {
		free(namelist[i]);
	}
	free(namelist);

	// We're done with the physical keyboard addresses.
	for (int i = 0; i < possibleKeyboards; i++) {
		free(physicalAddresses[i]);
	}
	free(physicalAddresses);

	*matchedKeyboardsOut = matchedKeyboards;

	if (matchedKeyboards) {
		hidrawPaths = realloc(hidrawPaths, sizeof(*hidrawPaths) * matchedKeyboards);
	} else {
		free(hidrawPaths);
		return 1;
	}

	*hidrawPathsOut = hidrawPaths;

	return 0;
}

int keyboard_init()
{
	int possibleKeyboards = 0;
	char **physicalAddresses = NULL;
	if (collect_physical_keyboard_addresses(&physicalAddresses, &possibleKeyboards)) {
		return 1;
	}

	//printf("\n");

	int matchedKeyboards = 0;
	char **hidrawPaths = NULL;
	if (find_matching_hidraw_devices(physicalAddresses, possibleKeyboards, &hidrawPaths, &matchedKeyboards)) {
		return 1;
	}

	printf("Matched Keyboard Paths:\n");
	for (int i = 0; i < matchedKeyboards; i++) {
		printf(" - %s\n", hidrawPaths[i]);
	}

	// TODO: Only select a single keyboard?

	openEventFiles = 0;
	eventFiles = malloc(matchedKeyboards * sizeof(*eventFiles));
	for (int i = 0; i < matchedKeyboards; i++) {
		if ((eventFiles[openEventFiles] = open(hidrawPaths[i], O_RDONLY|O_NONBLOCK)) >= 0) {
			struct hidraw_report_descriptor desc;
			ioctl(eventFiles[openEventFiles], HIDIOCGRDESCSIZE, &desc.size);
			ioctl(eventFiles[openEventFiles], HIDIOCGRDESC, &desc);

			// EXTREMELY bootleg HID descriptor parsing.
			int offset = 0;
			int collectionCount = 0;
			int success = 0;
			while (offset < desc.size) {
				const unsigned char dataLength = desc.value[offset] & 0x03;
				if (dataLength == 1) {
					if (offset < desc.size) {
						if (desc.value[offset] == 0xA1 && desc.value[offset + 1] == 0x01) {
							// Forbid devices with >1 application collection.
							collectionCount++;
							if (collectionCount > 1) {
								close(eventFiles[openEventFiles]);
								success = 0;
								break;
							}
						} else if(desc.value[offset] == 0x05 && desc.value[offset + 1] != 0x07) {
							// If we encounted the keyboard/keypad page then we should be good.
							success = 1;
						}
					}
				}
				offset += dataLength + 1;
			}

			if (success) {
				openEventFiles++;
			}
		}
	}
	eventFiles = realloc(eventFiles, openEventFiles * sizeof(*eventFiles));

	// We're done with the matched keyboard paths.
	for (int i = 0; i < matchedKeyboards; i++) {
		free(hidrawPaths[i]);
	}
	free(hidrawPaths);

	lastScancodes = calloc(openEventFiles, sizeof(char *));
	lastModifiers = calloc(openEventFiles, sizeof(char *));

	// Give each device a bit of time to try get nonzero initial state.
	for (int i = 0; i < openEventFiles; i++) {
		lastScancodes[i] = calloc(256, 1);
		lastModifiers[i] = calloc(8, 1);

		{
			fd_set set;
			struct timeval timeout;
			int rv;

			FD_ZERO(&set);
			FD_SET(eventFiles[i], &set);
			timeout.tv_sec = 0;
			timeout.tv_usec = 50 * 1000; // 50ms.

			rv = select(eventFiles[i] + 1, &set, NULL, NULL, &timeout);
			if (rv == -1 || rv == 0) {
				// Error or timed out.
				continue;
			}
		}

		unsigned char events[1024];
		ssize_t count = 0;
		if ((count = read(eventFiles[i], events, sizeof(events))) < 0) {
			continue;
		}

		for (ssize_t currByte = 2; currByte < count; currByte++) {
			lastScancodes[i][events[currByte]] = 1;
		}

		if (count > 1) {
			for (int j = 0; j < 8; j++) {
				lastModifiers[i][j] = ((events[0] >> j) & 1);
			}
		}
	}

#if 0
	for (int i = 0; i < openEventFiles; i++) {
		close(eventFiles[i]);
	}
	free(eventFiles);
#endif

	return 0;
}

int keyboard_tick()
{
	for (int i = 0; i < openEventFiles; i++) {
		unsigned char events[1024];
		unsigned char currScancodes[256] = {0};
		unsigned char currModifiers[8] = {0};

		// IMPORTANT: USB HID keyboards start with a byte of modifiers, a reserved byte, then scancodes.

		int event = 0;
		ssize_t count = 0;
		if ((count = read(eventFiles[i], events, sizeof(events))) < 0) {
			continue;
		}

		if (count > 0) {
			int changeModifiers = 0;
			int changeScancodes = 0;

			for (int j = 0; j < 8; j++) {
				currModifiers[j] = ((events[0] >> j) & 1);
				if (lastModifiers[i][j] != currModifiers[j]) {
					//printf("%s: %d->%d. ", MODIFIER_NAMES[j], lastModifiers[i][j], currModifiers[j]);
					if (!changeModifiers) {
						printf("RS");
						printf("M"); // M flags that modifiers follow.
					}
					printf("%1x", (currModifiers[j] << 3) | j); // MSB for on/off, 3 bits for modifier.
					lastModifiers[i][j] = currModifiers[j];
					changeModifiers = 1;
				}
			}

			for (ssize_t currByte = 2; currByte < count; currByte++) {
				currScancodes[events[currByte]] = 1;
				// TODO: It seems like scancodes are on a stack.
				//       Could we break once we encounter 0 (after second byte)?
			}

			changeScancodes = 0;
			for (int j = 1; j < 256; j++) {
				if (lastScancodes[i][j] != currScancodes[j]) {
					//printf("%s: %d->%d. ", SCANCODE_NAMES[j], lastScancodes[i][j], currScancodes[j]);
					if (!changeScancodes) {
						if (!changeModifiers) {
							printf("RS");
						}
						printf("S"); // S flags that scancodes follow.
					}
					printf("%02x%x", j, currScancodes[j]); // 2 hex for scancode value then 1 char for on/off.
					lastScancodes[i][j] = currScancodes[j];
					changeScancodes = 1;
				}
			}

			if (changeModifiers || changeScancodes) {
				printf("\n");
			}
		}
	}

	return 0;
}
