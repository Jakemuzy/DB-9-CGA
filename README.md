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
