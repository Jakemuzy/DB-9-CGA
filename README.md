# Images  

![Red Circle](/assets/red-circle.png "Red Circle")  

---

## Custom DB-9 CGA video output converter  

Written for the Samsung Santron SC 452C, tailored to its horizontal and vertical refresh rate.  
Input a video stream to an the ESP32-S3, and it will automaticallly convert into readable signals by the DB-9 connector.  
Additionally since the ESP32-S3 is dual core we can easily host an MQTT server which both commands and raw images can be sent to.  
    - This allows us to have a dedicated core for outputting the precise vsync and hsync timings that CRT monitors require, and another core specifically for listening and processing commands  
    - Currently planned commands are the following:  
        - *Wake*: Sends a signal to an octopular inside the CRT, triggering the power button  
        - *Img*: Receives a binary array corresponding to image that will be displayed on the screen  
        - *Cmd*: Receives a command, command will display a predefined image based on the command  

### Wireless  

Since we are directly using MQTT and generating video signals on the fly, we can hide the ESP inside the monitor chasis.  
No need to use the existing DB9 CGA cable, instead we can intercept the data lines directly, leading to an essentially wireless experience.  
This also allows us to wake the monitor wirelessly since the esp just needs to send a signal directly to the power button, which will be handled by an octopular to maintain separation of such high voltages.  


### Activation  

1. In order to develop this project further, you will first need to install all ESP32 development prerequsitives. These can be found [here](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html).  
2. From here you can simply do source ./activate.sh and you should have the right venv

### Architectural Notes

* A major bottleneck I orginally faced when I started streaming the video from the ESP was the fact that the SRAM was way too small. With each of my pixels taking up 2 bytes of data, each screen would take up 2 * 640 * 200 = 256KB of data. Although this can easily fit into the SRAM, the rom is not contiguous and therefore is unable to handle this amount of data. On top of this fact, I used 2 video buffers instead of one in order to ensure smooth transitions, meaning no matter what, the videos would not fit on the SRAM.
* For this reason, all video buffers are allocated on the heap in PSRAM. This, however, introduces some new problems: namely PSRAM is not nearly fast enough to be read and streamed at a consistent 15.75 KHz that the CGA standard demands.
* We can cleverly work around this "slow" memory by configuring a 10 line bounce buffer. Essentially an intermediary cache between the PSRAM and the SRAM that allows the data to be continuously be streamed without slowdown. This succesfully prevents the LCD peripheral from being starved of data, hence avoid screen tearing and flickering.
* This design also means I don't have to worry about hyper-optimizing every screen draw function. Since we have an adequete buffer; any slowdowns ie.) a function that has poor cache locality (such as rendering text) can take up as much leeway as they need without affecting the rendering.
    * This obviously has a limit, but we can simply expand the bounce buffer size if needed (remember though there is a hard cap as we faced previously, we cannot store the whole screen in SRAM).
* As for the porch values, don't ask me I have no idea, I just found some data online and adjusted them until they looked correct. 

