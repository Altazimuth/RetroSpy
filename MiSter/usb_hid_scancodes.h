
#ifndef __USB_HID_SCANCODES_H__
#define __USB_HID_SCANCODES_H__

// First byte of event, second should be 0x00.
#define KMOD_LCTRL  0x01
#define KMOD_LSHIFT 0x02
#define KMOD_LALT   0x04
#define KMOD_LMETA  0x08
#define KMOD_RCTRL  0x10
#define KMOD_RSHIFT 0x20
#define KMOD_RALT   0x40
#define KMOD_RMETA  0x80

static const char *MODIFIER_NAMES[8] = {
	"LCTRL", "LSHIFT", "LALT", "LMETA", "RCTRL", "RSHIFT", "RALT", "RMETA",
};

// Scancodes!
// Based on HID Usage Tables Version 1.12: https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf
#define KEY_RESERVED            0x00
#define KEY_ERROR_ROLL_OVER     0x01 // No NKRO, so >6 keys means bytes are filled w/ this.
#define KEY_POST_FAIL           0x02
#define KEY_ERROR_UNDERFINED    0x03
#define KEY_A                   0x04
#define KEY_B                   0x05
#define KEY_C                   0x06
#define KEY_D                   0x07
#define KEY_E                   0x08
#define KEY_F                   0x09
#define KEY_G                   0x0A
#define KEY_H                   0x0B
#define KEY_I                   0x0C
#define KEY_J                   0x0D
#define KEY_K                   0x0E
#define KEY_L                   0x0F
#define KEY_M                   0x10
#define KEY_N                   0x11
#define KEY_O                   0x12
#define KEY_P                   0x13
#define KEY_Q                   0x14
#define KEY_R                   0x15
#define KEY_S                   0x16
#define KEY_T                   0x17
#define KEY_U                   0x18
#define KEY_V                   0x19
#define KEY_W                   0x1A
#define KEY_X                   0x1B
#define KEY_Y                   0x1C
#define KEY_Z                   0x1D
#define KEY_1                   0x1E
#define KEY_2                   0x1F
#define KEY_3                   0x20
#define KEY_4                   0x21
#define KEY_5                   0x22
#define KEY_6                   0x23
#define KEY_7                   0x24
#define KEY_8                   0x25
#define KEY_9                   0x26
#define KEY_0                   0x27
#define KEY_RETURN              0x28 // AKA Enter.
#define KEY_ESCAPE              0x29
#define KEY_BACKSPACE           0x2A // Docs call it DELETE which is very silly.
#define KEY_TAB                 0x2B
#define KEY_SPACE               0x2C
#define KEY_MINUS               0x2D
#define KEY_EQUALS              0x2E
#define KEY_LEFTBRACKET         0x2F // [ and {
#define KEY_RIGHTBRACKET        0x30 // ] and }
#define KEY_BACKSLASH           0x31
#define KEY_NONUSHASH           0x32 // Non-US # and ~
#define KEY_SEMICOLON           0x33
#define KEY_APOSTROPHE          0x34
#define KEY_GRAVE               0x35 // ` and ~
#define KEY_COMMA               0x36
#define KEY_PERIOD              0x37 // .
#define KEY_FORWARDSLASH        0x38
#define KEY_CAPSLOCK            0x39 // I'M YELLING.
#define KEY_F1                  0x3A
#define KEY_F2                  0x3B
#define KEY_F3                  0x3C
#define KEY_F4                  0x3D
#define KEY_F5                  0x3E
#define KEY_F6                  0x3F
#define KEY_F7                  0x40
#define KEY_F8                  0x41
#define KEY_F9                  0x42
#define KEY_F10                 0x43
#define KEY_F11                 0x44
#define KEY_F12                 0x45
#define KEY_PRINTSCREEN         0x46
#define KEY_SCROLLLOCK          0x47 // "Why is this on the keyboard?"
#define KEY_PAUSE               0x48 // "Why is this on the keyboard?" Pt. 2
#define KEY_INSERT              0x49 // "Help, Word is typing over all my text."
#define KEY_HOME                0x4A
#define KEY_PAGEUP              0x4B
#define KEY_DELETE              0x4C // Called "Delete Forward" in the spec. It's the actual delete key.
#define KEY_END                 0x4D
#define KEY_PAGEDOWN            0x4E
#define KEY_RIGHTARROW          0x4F
#define KEY_LEFTARROW           0x50
#define KEY_DOWNARROW           0x51
#define KEY_UPARROW             0x52
#define KEY_NUMLOCK             0x53
#define KEY_PAD_FORWARDSLASH    0x54
#define KEY_PAD_ASTERISK        0x55 // And Obelisk.
#define KEY_PAD_MINUS           0x56
#define KEY_PAD_PLUS            0x57
#define KEY_PAD_ENTER           0x58 // Oh NOW you call it enter and not return.
#define KEY_PAD_1               0x59 // Also End.
#define KEY_PAD_2               0x5A // Also Down Arrow.
#define KEY_PAD_3               0x5B // Also Page Down.
#define KEY_PAD_4               0x5C // Also Left Arrow.
#define KEY_PAD_5               0x5D // No secondary function! :(
#define KEY_PAD_6               0x5E // Also Right Arrow.
#define KEY_PAD_7               0x5F // Also Home.
#define KEY_PAD_8               0x60 // Also Up Arrow.
#define KEY_PAD_9               0x61 // Also Page Up.
#define KEY_PAD_0               0x62 // Also Insert.
#define KEY_PAD_PERIOD          0x63 // Also Delete.
#define KEY_NONUSBACKSLASH      0x64 // Also |.
#define KEY_APPLICATION         0x65 // Windows key for Windows 95 and Compose.
#define KEY_POWER               0x66 // Reserved for typical keyboard status or keyboard errors. Not a physical key.
#define KEY_PAD_EQUALS          0x67
#define KEY_F13                 0x68
#define KEY_F14                 0x69
#define KEY_F15                 0x6A
#define KEY_F16                 0x6B
#define KEY_F17                 0x6C
#define KEY_F18                 0x6D
#define KEY_F19                 0x6E
#define KEY_F20                 0x6F
#define KEY_F21                 0x70
#define KEY_F22                 0x71
#define KEY_F23                 0x72
#define KEY_F24                 0x73
#define KEY_EXECUTE             0x74
#define KEY_HELP                0x75
#define KEY_MENU                0x76
#define KEY_SELECT              0x77
#define KEY_STOP                0x78
#define KEY_AGAIN               0x79
#define KEY_UNDO                0x7A
#define KEY_CUT                 0x7B
#define KEY_COPY                0x7C
#define KEY_PASTE               0x7D
#define KEY_FIND                0x7E
#define KEY_MUTE                0x7F
#define KEY_VOLUME_UP           0x80
#define KEY_VOLUME_DOWN         0x81
#define KEY_LOCKING_CAPSLOCK    0x82
#define KEY_LOCKING_NUMLOCK     0x83
#define KEY_LOCKING_SCROLLOCK   0x84
#define KEY_PAD_COMMA           0x85
#define KEY_PAD_EQUAL_SIGN      0x86 // Used on AS/400 keyboards.
#define KEY_INTERNATIONAL_1     0x87
#define KEY_INTERNATIONAL_2     0x88
#define KEY_INTERNATIONAL_3     0x89
#define KEY_INTERNATIONAL_4     0x8A
#define KEY_INTERNATIONAL_5     0x8B
#define KEY_INTERNATIONAL_6     0x8C
#define KEY_INTERNATIONAL_7     0x8D
#define KEY_INTERNATIONAL_8     0x8E
#define KEY_INTERNATIONAL_9     0x8F
#define KEY_LANG_1              0x90
#define KEY_LANG_2              0x91
#define KEY_LANG_3              0x92
#define KEY_LANG_4              0x93
#define KEY_LANG_5              0x94
#define KEY_LANG_6              0x95
#define KEY_LANG_7              0x96
#define KEY_LANG_8              0x97
#define KEY_LANG_9              0x98
#define KEY_ALTERNATE_ERASE     0x99
#define KEY_SYSREQ              0x9A
#define KEY_CANCEL              0x9B
#define KEY_CLEAR               0x9C
#define KEY_PRIOR               0x9D
#define KEY_RETURN_2            0x9E // I have no idea what this key is or does different from Return/Enter.
#define KEY_SEPARATOR           0x9F
#define KEY_OUT                 0xA0 // Let's go.
#define KEY_OPER                0xA1
#define KEY_CLEAR_AGAIN         0xA2
#define KEY_CRSEL_PROPS         0xA3
#define KEY_EXSEL               0xA4
#define KEY_RESERVED_A5         0xA5
#define KEY_RESERVED_A6         0xA6
#define KEY_RESERVED_A7         0xA7
#define KEY_RESERVED_A8         0xA8
#define KEY_RESERVED_A9         0xA9
#define KEY_RESERVED_AA         0xAA
#define KEY_RESERVED_AB         0xAB
#define KEY_RESERVED_AC         0xAC
#define KEY_RESERVED_AD         0xAD
#define KEY_RESERVED_AE         0xAE
#define KEY_RESERVED_AF         0xAF
#define KEY_PAD_00              0xB0
#define KEY_PAD_000             0xB1
#define KEY_THOUSANDS_SEPARATOR 0xB2
#define KEY_DECIMAL_SEPARATOR   0xB3
#define KEY_CURRENCY_UNIT       0xB4
#define KEY_CURRENT_SUBUNIT     0xB5
#define KEY_PAD_LEFTPAREN       0xB6
#define KEY_PAD_RIGHTPAREN      0xB7
#define KEY_PAD_LEFTBRACE       0xB8
#define KEY_PAD_RIGHTBRACE      0xB9
#define KEY_PAD_TAB             0xBA
#define KEY_PAD_BACKSPACE       0xBB
#define KEY_PAD_A               0xBC
#define KEY_PAD_B               0xBD
#define KEY_PAD_C               0xBE
#define KEY_PAD_D               0xBF
#define KEY_PAD_E               0xC0
#define KEY_PAD_F               0xC1
#define KEY_PAD_XOR             0xC2
#define KEY_PAD_CARET           0xC3
#define KEY_PAD_PERCENT         0xC4
#define KEY_PAD_LESSTHAN        0xC5
#define KEY_PAD_GREATERTHAN     0xC6
#define KEY_PAD_BITWISEAND      0xC7
#define KEY_PAD_LOGICALAND      0xC8
#define KEY_PAD_BITWISEOR       0xC9
#define KEY_PAD_LOGICALOR       0xCA
#define KEY_PAD_COLON           0xCB
#define KEY_PAD_HASH            0xCC
#define KEY_PAD_SPACE           0xCD
#define KEY_PAD_AT              0xCE
#define KEY_PAD_EXCLAMATION     0xCF
#define KEY_PAD_MEM_STORE       0xD0
#define KEY_PAD_MEM_RECALL      0xD1
#define KEY_PAD_MEM_CLEAR       0xD2
#define KEY_PAD_MEM_ADD         0xD3
#define KEY_PAD_MEM_SUBTRACT    0xD4
#define KEY_PAD_MEM_MULTIPLY    0xD5
#define KEY_PAD_MEM_DIVIDE      0xD6
#define KEY_PAD_PLUSMINUS       0xD7
#define KEY_PAD_CLEAR           0xD8
#define KEY_PAD_CLEARENTRY      0xD9
#define KEY_PAD_BINARY          0xDA
#define KEY_PAD_OCTAL           0xDB
#define KEY_PAD_DECIMAL         0xDC
#define KEY_PAD_HEXADECIMAL     0xDD
#define KEY_RESERVED_DE         0xDE
#define KEY_RESERVED_DF         0xDF
#define KEY_LEFTCONTROL         0xE0 // Not actually used.
#define KEY_LEFTSHIFT           0xE1 // Not actually used.
#define KEY_LEFTALT             0xE2 // Not actually used.
#define KEY_LEFTGUI             0xE3 // Not actually used.
#define KEY_RIGHTCONTROL        0xE4 // Not actually used.
#define KEY_RIGHTSHIFT          0xE5 // Not actually used.
#define KEY_RIGHTALT            0xE6 // Not actually used.
#define KEY_RIGHTGUI            0xE7 // Not actually used.
// Everything up to 0xFF is reserved.

static const char *SCANCODE_NAMES[256] = {
	"RESERVED", "ERROR_ROLL_OVER", "POST_FAIL", "ERROR_UNDERFINED",
	"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
	"1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
	"RETURN", "ESCAPE", "BACKSPACE", "TAB", "SPACE",
	"MINUS", "EQUALS", "LEFTBRACKET", "RIGHTBRACKET", "BACKSLASH", "NONUSHASH",
	"SEMICOLON", "APOSTROPHE", "GRAVE", "COMMA", "PERIOD", "FORWARDSLASH",
	"CAPSLOCK",
	"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
	"PRINTSCREEN", "SCROLLLOCK", "PAUSE",
	"INSERT", "HOME", "PAGEUP", "DELETE", "END", "PAGEDOWN",
	"RIGHTARROW", "LEFTARROW", "DOWNARROW", "UPARROW",
	"NUMLOCK",
	"PAD_FORWARDSLASH", "PAD_ASTERISK", "PAD_MINUS", "PAD_PLUS", "PAD_ENTER",
	"PAD_1", "PAD_2", "PAD_3", "PAD_4", "PAD_5", "PAD_6", "PAD_7", "PAD_8", "PAD_9", "PAD_0",
	"PAD_PERIOD",
	"NONUSBACKSLASH",
	"APPLICATION", "POWER",
	"PAD_EQUALS",
	"F13", "F14", "F15", "F16", "F17", "F18", "F19", "F20", "F21", "F22", "F23", "F24",
	"EXECUTE", "HELP", "MENU", "SELECT", "STOP", "AGAIN", "UNDO", "CUT", "COPY", "PASTE", "FIND", "MUTE",
	"VOLUME_UP", "VOLUME_DOWN",
	"LOCKING_CAPSLOCK", "LOCKING_NUMLOCK", "LOCKING_SCROLLOCK",
	"PAD_COMMA", "PAD_EQUAL_SIGN",
	"INTERNATIONAL_1", "INTERNATIONAL_2", "INTERNATIONAL_3", "INTERNATIONAL_4", "INTERNATIONAL_5", "INTERNATIONAL_6", "INTERNATIONAL_7", "INTERNATIONAL_8", "INTERNATIONAL_9",
	"LANG_1", "LANG_2", "LANG_3", "LANG_4", "LANG_5", "LANG_6", "LANG_7", "LANG_8", "LANG_9",
	"ALTERNATE_ERASE", "SYSREQ", "CANCEL", "CLEAR", "PRIOR", "RETURN_2",
	"SEPARATOR", "OUT", "OPER", "CLEAR_AGAIN", "CRSEL_PROPS", "EXSEL",
	"RESERVED_A5", "RESERVED_A6", "RESERVED_A7", "RESERVED_A8", "RESERVED_A9", "RESERVED_AA", "RESERVED_AB", "RESERVED_AC", "RESERVED_AD", "RESERVED_AE", "RESERVED_AF",
	"PAD_00", "PAD_000", "THOUSANDS_SEPARATOR", "DECIMAL_SEPARATOR", "CURRENCY_UNIT", "CURRENT_SUBUNIT",
	"PAD_LEFTPAREN", "PAD_RIGHTPAREN", "PAD_LEFTBRACE", "PAD_RIGHTBRACE", "PAD_TAB", "PAD_BACKSPACE",
	"PAD_A", "PAD_B", "PAD_C", "PAD_D", "PAD_E", "PAD_F",
	"PAD_XOR", "PAD_CARET", "PAD_PERCENT", "PAD_LESSTHAN", "PAD_GREATERTHAN", "PAD_BITWISEAND", "PAD_LOGICALAND", "PAD_BITWISEOR", "PAD_LOGICALOR",
	"PAD_COLON", "PAD_HASH", "PAD_SPACE", "PAD_AT", "PAD_EXCLAMATION",
	"PAD_MEM_STORE", "PAD_MEM_RECALL", "PAD_MEM_CLEAR", "PAD_MEM_ADD", "PAD_MEM_SUBTRACT", "PAD_MEM_MULTIPLY", "PAD_MEM_DIVIDE",
	"PAD_PLUSMINUS",
	"PAD_CLEAR", "PAD_CLEARENTRY",
	"PAD_BINARY", "PAD_OCTAL", "PAD_DECIMAL", "PAD_HEXADECIMAL",
	"RESERVED_DE", "RESERVED_DF",
	"LEFTCONTROL", "LEFTSHIFT", "LEFTALT", "LEFTGUI", "RIGHTCONTROL", "RIGHTSHIFT", "RIGHTALT", "RIGHTGUI",
	"RESERVED_E8", "RESERVED_E9", "RESERVED_EA", "RESERVED_EB", "RESERVED_EC", "RESERVED_ED", "RESERVED_EE", "RESERVED_EF", "RESERVED_F0", "RESERVED_F1", "RESERVED_F2", "RESERVED_F3", "RESERVED_F4", "RESERVED_F5", "RESERVED_F6", "RESERVED_F7", "RESERVED_F8", "RESERVED_F9", "RESERVED_FA", "RESERVED_FB", "RESERVED_FC", "RESERVED_FD", "RESERVED_FE", "RESERVED_FF",
};

#endif