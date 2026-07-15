#ifndef BUFFER_LETTERS_H_
#define BUFFER_LETTERS_H_

#include <stdint.h>

/*
     Simple file for const bitmap representations
    of alphanumeric variables for buffer rendering.
*/

typedef struct bitmap_display {
    uint16_t width;
    uint16_t height; 
    const uint8_t *bitmap;
} bitmap_display;

// Uppercase Letters
extern const uint8_t BITMAP_A[];
extern const uint8_t BITMAP_B[];
extern const uint8_t BITMAP_C[];
extern const uint8_t BITMAP_D[];
extern const uint8_t BITMAP_E[];
extern const uint8_t BITMAP_F[];
extern const uint8_t BITMAP_G[];
extern const uint8_t BITMAP_H[];
extern const uint8_t BITMAP_I[];
extern const uint8_t BITMAP_J[];
extern const uint8_t BITMAP_K[];
extern const uint8_t BITMAP_L[];
extern const uint8_t BITMAP_M[];
extern const uint8_t BITMAP_N[];
extern const uint8_t BITMAP_O[];
extern const uint8_t BITMAP_P[];
extern const uint8_t BITMAP_Q[];
extern const uint8_t BITMAP_R[];
extern const uint8_t BITMAP_S[];
extern const uint8_t BITMAP_T[];
extern const uint8_t BITMAP_U[];
extern const uint8_t BITMAP_V[];
extern const uint8_t BITMAP_W[];
extern const uint8_t BITMAP_X[];
extern const uint8_t BITMAP_Y[];
extern const uint8_t BITMAP_Z[];

// Lowercase Letters
extern const uint8_t BITMAP_a[];
extern const uint8_t BITMAP_b[];
extern const uint8_t BITMAP_c[];
extern const uint8_t BITMAP_d[];
extern const uint8_t BITMAP_e[];
extern const uint8_t BITMAP_f[];
extern const uint8_t BITMAP_g[];
extern const uint8_t BITMAP_h[];
extern const uint8_t BITMAP_i[];
extern const uint8_t BITMAP_j[];
extern const uint8_t BITMAP_k[];
extern const uint8_t BITMAP_l[];
extern const uint8_t BITMAP_m[];
extern const uint8_t BITMAP_n[];
extern const uint8_t BITMAP_o[];
extern const uint8_t BITMAP_p[];
extern const uint8_t BITMAP_q[];
extern const uint8_t BITMAP_r[];
extern const uint8_t BITMAP_s[];
extern const uint8_t BITMAP_t[];
extern const uint8_t BITMAP_u[];
extern const uint8_t BITMAP_v[];
extern const uint8_t BITMAP_w[];
extern const uint8_t BITMAP_x[];
extern const uint8_t BITMAP_y[];
extern const uint8_t BITMAP_z[];

// Numbers
extern const uint8_t BITMAP_0[];
extern const uint8_t BITMAP_1[];
extern const uint8_t BITMAP_2[];
extern const uint8_t BITMAP_3[];
extern const uint8_t BITMAP_4[];
extern const uint8_t BITMAP_5[];
extern const uint8_t BITMAP_6[];
extern const uint8_t BITMAP_7[];
extern const uint8_t BITMAP_8[];
extern const uint8_t BITMAP_9[];

#endif 

