#include <stdio.h>

#define VSYNC 
#define HSYNC

#ifdef ESP32
    #define CLOCK_RATE 1600000000
#else
    exit(1); // Have this be an enum from another file
#endif

int main()
{
    return 0;
}
