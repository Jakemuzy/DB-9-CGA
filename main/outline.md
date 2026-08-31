mqtt.h
    - Initializes mqtt client
    - Receives binary blobs, notification type, notification duration, calls display.h writes the blob to the back buffer
    - Receives config change requests

display.h
    - Struct that stores both buffers, pointer to active one and lcd panel info
    - Function that swaps buffers
    - Function that writes to back buffer
    - Function that draws whats currently in the front buffer

buzzer.h
    - Stores predefined frequencies and durations of different notification types

config.h

main.c
    - Calls mqtt.h to initialize client
    - Infinitely draws back buffer
