# Images  

![Red Circle](/assets/red-circle.png "Red Circle")  

---

## Custom DB-9 CGA video output converter  

Written for the Samsung Santron SC 452C, tailored to its horizontal and vertical refresh rate.  
Input a video stream to an the ESP32-S3, and it will automaticallly convert into readable signals by the DB-9 connector.  
Additionally since the ESP32-S3 is dual core we can easily host an MQTT server which both commands and raw images can be sent to.  
    - This allows us to have a dedicated core for outputting the precise vsync and hsync timings that CRT monitors require, and another core specifically for listening and processing commands  
    - Current signal commands are the following: 
        - */db9/sleep*: Sends a signal to an octopular inside the CRT, triggering the power button off 
        - */db9/wake*: Sends a signal to an octopular inside the CRT, triggering the power button on
        - */db9/config*: Receives a configuration to change. Takes a name and a value and will change the config on the esp32
        - */db9/blob*: Receives a binary blob corresponding to image that will be displayed on the screen  
        - */db9/buzzer*: Receives a notification t        - */db9/wake*: Sends a signal to an octopular inside the CRT, triggering the power button  ype which corresopnds to a buzzer frequency and duration

### Wireless  

Since we are directly using MQTT and generating video signals on the fly, we can hide the ESP inside the monitor chasis.  
No need to use the existing DB9 CGA cable, instead we can intercept the data lines directly, leading to an essentially wireless experience.  
This also allows us to wake the monitor wirelessly since the esp just needs to send a signal directly to the power button, which will be handled by an octopular to maintain separation of such high voltages.  


### Activation  

1. In order to develop this project further, you will first need to install all ESP32 development prerequsitives. These can be found [here](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html).  
2. From here you can simply do source ./activate.sh and you should have the right venv
3. You must activate these settings in idf.py menuconfig
    - Component config ---> ESP PSRAM ---> Support for external PSRAM
        - Set Mode (QUAD/OCT) to 'Octal'
    - SPI RAM Config ---> RAM Clock Speed 
        - Set to 80MHz the default 40 will cause a zebra stripe pattern
    - Please note only ESP-S3 will work with the lcd rgb library due to the specialized DMA and the 16mb of psram


### Architectural Notes

* A major bottleneck I orginally faced when I started streaming the video from the ESP was the fact that the SRAM was way too small. With each of my pixels taking up 2 bytes of data, each screen would take up 2 * 640 * 200 = 256KB of data. Although this can easily fit into the SRAM, the rom is not contiguous and therefore is unable to handle this amount of data. On top of this fact, I used 2 video buffers instead of one in order to ensure smooth transitions, meaning no matter what, the videos would not fit on the SRAM.
* For this reason, all video buffers are allocated on the heap in PSRAM. This, however, introduces some new problems: namely PSRAM is not nearly fast enough to be read and streamed at a consistent 15.75 KHz that the CGA standard demands.
* We can cleverly work around this "slow" memory by configuring a 10 line bounce buffer. Essentially an intermediary cache between the PSRAM and the SRAM that allows the data to be continuously be streamed without slowdown. This succesfully prevents the LCD peripheral from being starved of data, hence avoid screen tearing and flickering.
* This design also means I don't have to worry about hyper-optimizing every screen draw function. Since we have an adequete buffer; any slowdowns ie.) a function that has poor cache locality (such as rendering text) can take up as much leeway as they need without affecting the rendering.
    * This obviously has a limit, but we can simply expand the bounce buffer size if needed (remember though there is a hard cap as we faced previously, we cannot store the whole screen in SRAM).
* As for the porch values, don't ask me I have no idea, I just found some data online and adjusted them until they looked correct. 
* That being said, most of the work fitting the framebuffers into the PSRAM was in vain, since I later encountered issues with the PSRAM and memory copies. Due to the nature of the PSRAM being shared by the SPI and DMA for consistent video streaming any stalling operation on these sections of memory could cause the hsync of vsync to go out of sync with the monitor.
    * Since I decided to keep one core dedicated to video processing and the other dedicated to listening for mqtt commands, this meant that whenever I received a binary blob image over the mqtt protocol, it would stall the shared SPRAM memory during the memory copy from.
    * This frequently lead to a sort of zebra stripe pattern, where the short stall in the memory reading lead to the signals going out of sync and hence inaccuracies on the screen
    * I later mitigated this by switching back to one frame buffer initialized in SRAM and directly memory copying whenever I received a binary blob image. Although this again introduced subtle flickering; images aren't being continuously sent over the protocol, but rather continuously displayed whenever an image was received. In this scenario I found the tradeoff worth it since a minor screen tearing for a fraction of a second is prefably to signals going out of sync.
* Speaking of the mqtt image blob, this was an interesting issue to solve. Since the binary image blob is sent directly over mqtt, it is split into multiple packets. These packets need to be reassembled once they are received by our mqtt core. This reassembly takes quite a bit of time, leading to around 6 seconds of delay before an image is displayed on the screen.
    * Although this delay isn't ideal, again I figured its fine since the goal of the project is to display notifications on the monitor. These notifications aren't time sensitive, and will stay displayed on the screen for an arbitrary amount of time before disappearing.

