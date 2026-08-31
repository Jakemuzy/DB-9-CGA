#!/usr/bin/env python3
"""
Test publisher for the DB9 firmware's MQTT topics.
Requires: pip install paho-mqtt
"""
import argparse
import struct
import sys
import time
import paho.mqtt.client as mqtt

TOPIC_BLOB = "/db9/blob"
TOPIC_CONFIG = "/db9/config"
TOPIC_SLEEP = "/db9/sleep"
TOPIC_WAKE = "/db9/wake"
TOPIC_BUZZER = "/db9/buzzer"

# Mirrors the Color/Brightness enum in the firmware.
DB9_BLACK, DB9_RED, DB9_GREEN, DB9_YELLOW = 0x00, 0x01, 0x02, 0x03
DB9_BLUE, DB9_PURPLE, DB9_CYAN, DB9_WHITE = 0x04, 0x05, 0x06, 0x07
DB9_DARK, DB9_BRIGHT = 0x00, 0x08


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


def make_test_blob(width, height):
    """Solid test frame: 1 byte per pixel, matching data_width=8."""
    pixel = DB9_RED | DB9_BRIGHT
    return bytes([pixel]) * (width * height)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--host", required=True, help="Broker IP/hostname")
    ap.add_argument("--port", type=int, default=1883)
    ap.add_argument("--width", type=int, default=320)
    ap.add_argument("--height", type=int, default=200)
    ap.add_argument("--aspect-ratio", type=float, default=0.4,
                     help="Must match firmware's ASPECT_RATIO constant")
    ap.add_argument("--radius", type=int, default=40)
    args = ap.parse_args()

    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id="db9_test_publisher")
    client.on_connect = on_connect
    client.on_publish = on_publish

    client.connect(args.host, args.port, keepalive=30)
    client.loop_start()
    time.sleep(1)  # let CONNACK complete

    print("\nCommands: [b]lob (solid), [d]raw circle, [n]otification, [c]onfig, [s]leep, [w]ake, [q]uit")
    while True:
        cmd = input("> ").strip().lower()
        if cmd == "b":
            blob = make_test_blob(args.width, args.height)
            client.publish(TOPIC_BLOB, blob, qos=1)
            print(f"Sent {len(blob)}-byte test blob")
        elif cmd == "d":
            pixels = draw_circle(
                args.width, args.height,
                posx=args.width // 2, posy=args.height // 2,
                radius=args.radius, color=DB9_WHITE, brightness=DB9_BRIGHT,
                aspect_ratio=args.aspect_ratio,
            )
            blob = pack_pixels(pixels)
            client.publish(TOPIC_BLOB, blob, qos=1)
            lit = sum(1 for p in pixels if p != DB9_BLACK)
            print(f"Sent circle frame: {len(blob)} bytes, {lit} lit pixels")
        elif cmd == "n":
            print("\n\tNotification Levels: [1] Info, [2] Alert, [3] Warning, [4] Critical")
            level_choice = input("\tChoose level (1-4): ").strip()

            if level_choice == "1":
                payload = b"NOTIFY_INFO"
            elif level_choice == "2":
                payload = b"NOTIFY_ALERT"
            elif level_choice == "3":
                payload = b"NOTIFY_WARNING"
            elif level_choice == "4":
                payload = b"NOTIFY_CRITICAL"
            else:
                print("\tInvalid choice, defaulting to INFO")
                payload = b"NOTIFY_INFO"

            # Publish the exact byte string token down the wire
            client.publish(TOPIC_BUZZER, payload, qos=1)
            print(f"\tSent {payload.decode()} payload to buzzer topic")
        elif cmd == "c":
            client.publish(TOPIC_CONFIG, b'{"width":320,"height":200}', qos=1)
        elif cmd == "s":
            client.publish(TOPIC_SLEEP, b"", qos=1)
        elif cmd == "w":
            client.publish(TOPIC_WAKE, b"", qos=1)
        elif cmd == "q":
            break
        else:
            print("Unknown command")

    client.loop_stop()
    client.disconnect()


if __name__ == "__main__":
    main()
