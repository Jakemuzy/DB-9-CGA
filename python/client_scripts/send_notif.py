#!/usr/bin/env python3
import argparse
import struct
import sys
import time
import paho.mqtt.client as mqtt

# MQTT crap

HOST = "192.168.68.102" # Get rekt this is local n00b
PORT = 1883

TOPIC_BLOB = "/db9/blob"
TOPIC_CONFIG = "/db9/config"
TOPIC_POWER = "/db9/power"
TOPIC_BUZZER = "/db9/buzzer"

# Color and brightness 
DB9_BLACK, DB9_RED, DB9_GREEN, DB9_YELLOW = 0x00, 0x01, 0x02, 0x03
DB9_BLUE, DB9_PURPLE, DB9_CYAN, DB9_WHITE = 0x04, 0x05, 0x06, 0x07
DB9_DARK, DB9_BRIGHT = 0x00, 0x08

# Defaults 
WIDTH = 640
HEIGHT = 200
ASPECT_RATIO = 0.4

# Buzzers
BUZZER_INFO = b"NOTIFY_INFO"
BUZZER_ALERT = b"NOTIFY_ALERT"
BUZZER_WARNING = b"NOTIFY_WARNING"
BUZZER_CRITICAL = b"NOTIFY_CRITICAL"

def pack_pixels(pixels):
    """Pack a flat list of 4-bit color values into single bytes,
    matching how BufferInfo->draw_buf[idx] = pixel_color is stored on-device
    (data_width=8, 1 byte per pixel)."""
    return struct.pack(f"<{len(pixels)}B", *pixels)

def draw_circle(width, height, posx, posy, radius, color, brightness, aspect_ratio):
    """Direct port of buffer_draw_circle() so the test blob matches firmware output.
    NOTE: aspect_ratio must match the ASPECT_RATIO constant used on-device, or the
    circle will render as an ellipse. Adjust --aspect-ratio if you know that value."""
    inner_sq = (radius - 3) ** 2
    outer_sq = (radius + 3) ** 2
    pixels = [DB9_BLACK] * (width * height)
    for y in range(height):
        dy = y - posy
        for x in range(width):
            dx = (x * aspect_ratio) - (posx * aspect_ratio)
            dist = dx * dx + dy * dy
            if inner_sq <= dist <= outer_sq:
                pixels[y * width + x] = color | brightness
    return pixels


def on_connect(client, userdata, flags, reason_code, properties=None):
    print(f"Connected to broker (reason_code={reason_code})")


def on_publish(client, userdata, mid, reason_code=None, properties=None):
    print(f"Published mid={mid}")

def main():
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id="db9_test_publisher")
    client.on_connect = on_connect
    client.on_publish = on_publish

    client.connect(HOST, PORT, keepalive=30)
    client.loop_start()
    time.sleep(1)  # let CONNACK complete

    client.publish(TOPIC_BUZZER, BUZZER_CRITICAL, qos=1)
    pixels = draw_circle(WIDTH, HEIGHT, WIDTH/2, HEIGHT/2, 40, DB9_BLUE, DB9_BRIGHT, ASPECT_RATIO)
    blob = pack_pixels(pixels)
    client.publish(TOPIC_BLOB, blob, qos=1)
    lit = sum(1 for p in pixels if p != DB9_BLACK)
    print(f"Sent circle frame: {len(blob)} bytes, {lit} lit pixels")

    client.publish(TOPIC_POWER, b"", qos=1)
    client.loop_stop()
    client.disconnect()


if __name__ == "__main__":
    main()

