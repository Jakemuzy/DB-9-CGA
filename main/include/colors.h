#ifndef DB9_COLORS_H_
#define DB9_COLORS_H_

typedef uint8_t Color;
typedef uint8_t Brightness;

enum {
    // Base colors
    DB9_RED = 0x01,
    DB9_GREEN = 0x02,
    DB9_BLUE = 0x04,
    DB9_YELLOW = 0x03,
    DB9_PURPLE = 0x05,
    DB9_CYAN = 0x06,
    DB9_WHITE = 0x07,
    DB9_BLACK = 0x00,

    // Intensity modifier
    DB9_DARK = 0x00,
    DB9_BRIGHT = 0x08
};

#endif