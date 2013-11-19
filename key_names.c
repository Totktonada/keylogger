#include <stdio.h>
#include <string.h>
#include "key_names.h"
#include "main.h"

/* Non-static, because setKeyName invoked from various contexts.
 * See SetWindowsHookEx documetation, about method of keyboard
 * hooks invocation. */

const char * key_names_0x01[] =
{
    "Left mouse button",
    "Right mouse button",
    "Control-break processing",
    "Middle mouse button (three-button mouse)",
    "X1 mouse button",
    "X2 mouse button",
    NULL,
    "BACKSPACE key",
    "TAB key",
    NULL,
    NULL,
    "CLEAR key",
    "ENTER key",
    NULL,
    NULL,
    "SHIFT key",
    "CTRL key",
    "ALT key",
    "PAUSE key",
    "CAPS LOCK key",
    "IME Kana mode",
    NULL,
    "IME Junja mode",
    "IME final mode",
    "IME Kanji mode",
    NULL,
    "ESC key",
    "IME convert",
    "IME nonconvert",
    "IME accept",
    "IME mode change request",
    "SPACEBAR",
    "PAGE UP key",
    "PAGE DOWN key",
    "END key",
    "HOME key",
    "LEFT ARROW key",
    "UP ARROW key",
    "RIGHT ARROW key",
    "DOWN ARROW key",
    "SELECT key",
    "PRINT key",
    "EXECUTE key",
    "PRINT SCREEN key",
    "INS key",
    "DEL key",
    "HELP key"
};

const char * key_names_0x5B[] = {
    "Left Windows key (Natural keyboard)",
    "Right Windows key (Natural keyboard)",
    "Applications key (Natural keyboard)",
    NULL,
    "Computer Sleep key",
    "Numeric keypad 0 key",
    "Numeric keypad 1 key",
    "Numeric keypad 2 key",
    "Numeric keypad 3 key",
    "Numeric keypad 4 key",
    "Numeric keypad 5 key",
    "Numeric keypad 6 key",
    "Numeric keypad 7 key",
    "Numeric keypad 8 key",
    "Numeric keypad 9 key",
    "Multiply key",
    "Add key",
    "Separator key",
    "Subtract key",
    "Decimal key",
    "Divide key"
};

const char * key_names_0x90[] = {
    "NUM LOCK key",
    "SCROLL LOCK key"
};

const char * key_names_0xA0[] = {
    "Left SHIFT key",
    "Right SHIFT key",
    "Left CONTROL key",
    "Right CONTROL key",
    "Left MENU key",
    "Right MENU key",
    "Browser Back key",
    "Browser Forward key",
    "Browser Refresh key",
    "Browser Stop key",
    "Browser Search key",
    "Browser Favorites key",
    "Browser Start and Home key",
    "Volume Mute key",
    "Volume Down key",
    "Volume Up key",
    "Next Track key",
    "Previous Track key",
    "Stop Media key",
    "Play/Pause Media key",
    "Start Mail key",
    "Select Media key",
    "Start Application 1 key",
    "Start Application 2 key"
};

const char * key_names_0xBA[] = {
    "The ';:' key (on US keyboard)"
    "The '+' key",
    "The ',' key",
    "The '-' key",
    "The '.' key",
    "The '/?' key (on US keyboard)",
    "The '`~' key (on US keyboard)"
};

const char * key_names_0xDB[] = {
    "The '[{' key (on US keyboard)",
    "The '\\|' key (on US keyboard)",
    "The ']}' key (on US keyboard)",
    "The 'single-quote/double-quote' key (on US keyboard)"
};

const char * key_names_0xF6[] = {
    "Attn key",
    "CrSel key",
    "ExSel key",
    "Erase EOF key",
    "Play key",
    "Zoom key",
    NULL,
    "PA1 key",
    "Clear key"
};

#define CHECK_KEY_NAME(vk_code, NUM) \
    ((vk_code) >= (NUM) && \
    (vk_code) < (NUM) + (int) ARRAYSIZE(key_names_##NUM) && \
    key_names_##NUM[(vk_code) - (NUM)] != NULL)

#define GET_KEY_NAME(vk_code, NUM) \
    do { \
        strcpy(buf, key_names_##NUM[(vk_code) - (NUM)]); \
    } while(0)

void setKeyName(char * buf, int vk_code)
{
    if (CHECK_KEY_NAME(vk_code, 0x01)) {
        GET_KEY_NAME(vk_code, 0x01);
    } else if (vk_code >= 0x30 && vk_code < 0x5B) {
        sprintf(buf, "%c key", (char) vk_code);
    } else if (CHECK_KEY_NAME(vk_code, 0x5B)) {
        GET_KEY_NAME(vk_code, 0x5B);
    } else if (vk_code >= 0x70 && vk_code < 0x70 + 24) {
        sprintf(buf, "F%d key", vk_code - 0x70 + 1);
    } else if (CHECK_KEY_NAME(vk_code, 0x90)) {
        GET_KEY_NAME(vk_code, 0x90);
    } else if (CHECK_KEY_NAME(vk_code, 0xA0)) {
        GET_KEY_NAME(vk_code, 0xA0);
    } else if (CHECK_KEY_NAME(vk_code, 0xBA)) {
        GET_KEY_NAME(vk_code, 0xBA);
    } else if (CHECK_KEY_NAME(vk_code, 0xDB)) {
        GET_KEY_NAME(vk_code, 0xDB);
    } else if (vk_code == 0xE2) {
        strcpy(buf, "Angle bracket or backslash key");
    } else if (vk_code == 0xE5) {
        strcpy(buf, "IME PROCESS key");
    } else if (CHECK_KEY_NAME(vk_code, 0xF6)) {
        GET_KEY_NAME(vk_code, 0xF6);
    } else {
        sprintf(buf, "Undefined key: vk code %d", vk_code);
    }
}
