/*
 * joystick.c Version 1.0
 *
 * Copyright (c) 2020 RetroSpy Technologies
 *
 * Based on jstest.c Version 1.2
 */

 /*
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 2 of the License, or
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

#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <dirent.h>
#include <string.h>

#include <linux/input.h>
#include <linux/joystick.h>

#include "axbtnmap.h"

char *axis_names[ABS_MAX + 1] = {
"X", "Y", "Z", "Rx", "Ry", "Rz", "Throttle", "Rudder",
"Wheel", "Gas", "Brake", "?", "?", "?", "?", "?",
"Hat0X", "Hat0Y", "Hat1X", "Hat1Y", "Hat2X", "Hat2Y", "Hat3X", "Hat3Y",
"?", "?", "?", "?", "?", "?", "?",
};

char *button_names[KEY_MAX - BTN_MISC + 1] = {
"Btn0", "Btn1", "Btn2", "Btn3", "Btn4", "Btn5", "Btn6", "Btn7", "Btn8", "Btn9",
"?", "?", "?", "?", "?", "?","LeftBtn", "RightBtn", "MiddleBtn", "SideBtn", 
"ExtraBtn", "ForwardBtn", "BackBtn", "TaskBtn", "?", "?", "?", "?", "?", "?", 
"?", "?", "Trigger", "ThumbBtn", "ThumbBtn2", "TopBtn", "TopBtn2", "PinkieBtn",
"BaseBtn", "BaseBtn2", "BaseBtn3", "BaseBtn4", "BaseBtn5", "BaseBtn6", 
"BtnDead", "BtnA", "BtnB", "BtnC", "BtnX", "BtnY", "BtnZ", "BtnTL", "BtnTR", 
"BtnTL2", "BtnTR2", "BtnSelect", "BtnStart", "BtnMode", "BtnThumbL", 
"BtnThumbR", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", 
"?", "?", "?", "?", "WheelBtn", "Gear up",
};

#define NAME_LENGTH 128

static unsigned char axes = 2;
static unsigned char buttons = 2;
static int version = 0x000800;
static char name[NAME_LENGTH] = "Unknown";

static int *axis;
static char *button;

int joystick_init(const char *filename)
{
    int fd, i;
	uint16_t btnmap[BTNMAP_SIZE];
	uint8_t axmap[AXMAP_SIZE];
	int btnmapok = 1;

	if ((fd = open(filename, O_RDONLY)) < 0) {
		perror("jstest");
		return 1;
	}

	ioctl(fd, JSIOCGVERSION, &version);
	ioctl(fd, JSIOCGAXES, &axes);
	ioctl(fd, JSIOCGBUTTONS, &buttons);
	ioctl(fd, JSIOCGNAME(NAME_LENGTH), name);

	getaxmap(fd, axmap);
	getbtnmap(fd, btnmap);

	printf("Driver version is %d.%d.%d.\n",
		version >> 16, (version >> 8) & 0xff, version & 0xff);

	/* Determine whether the button map is usable. */
	for (i = 0; btnmapok && i < buttons; i++) {
		if (btnmap[i] < BTN_MISC || btnmap[i] > KEY_MAX) {
			btnmapok = 0;
			break;
		}
	}
	if (!btnmapok) {
		/* btnmap out of range for names. Don't print any. */
		puts("jstest is not fully compatible with your kernel. Unable to retrieve button map!");
		printf("Joystick (%s) has %d axes ", name, axes);
		printf("and %d buttons.\n", buttons);
	}
	else {
		printf("Joystick (%s) has %d axes (", name, axes);
		for (i = 0; i < axes; i++)
			printf("%s%s", i > 0 ? ", " : "", axis_names[axmap[i]]);
		puts(")");

		printf("and %d buttons (", buttons);
		for (i = 0; i < buttons; i++) {
			printf("%s%s", i > 0 ? ", " : "", button_names[btnmap[i] - BTN_MISC]);
		}
		puts(").");
	}

	close(fd);

	axis = calloc(axes, sizeof(int));
	button = calloc(buttons, sizeof(char));

    return 0;
}

int joystick_tick(const char *filename)
{
    int fd, i;
	int k = 0;
	struct js_event js;

    if ((fd = open(filename, O_RDONLY)) < 0) {
        perror("jstest");
        return 1;
    }

    while (k < axes + buttons)
    {
        if (read(fd, &js, sizeof(struct js_event)) 
            != sizeof(struct js_event)) 
        {
            perror("\njstest: error reading");
            return 1;
        }

        switch (js.type & ~JS_EVENT_INIT) {
        case JS_EVENT_BUTTON:
            button[js.number] = js.value;
            break;
        case JS_EVENT_AXIS:
            axis[js.number] = js.value;
            break;
        }
        k++;
    }

    close(fd);

    //Packets are encoded as hex values. The server allows lines of up to
    //1024 chars, meaning we get to work with up to 512B of data this way.

    printf("%02x%02x", axes, buttons);

    //Collect buttons into groups of four (one hex char).
    i = 0;
    while (i < buttons)
    {
        unsigned char accum = 0;
        int start = i;
        while (i < start + 4 && i < buttons )
        {
            accum |= (button[i] != 0) << (i % 4);
            i++;
        }
        printf("%1x", accum);
    }

    for (i = 0; i < axes; i++)
    {
        printf("%08x", axis[i]);
    }

    //Probably could reduce the bandwidth by outputing the axes values 
    //directly, but can do it later.

    /*
    printf("%d|%d|", axes, buttons);
    for (i = 0; i < buttons; ++i)
    {
        printf(button[i] != 0 ? "1" : "0|");
    }
    printf("|");
    for (i = 0; i < axes; i++)
    {
        printf("%d|", axis[i]);
    }*/
    printf("\n");

    return 0;
}