#!/usr/bin/env python3
"""
Standalone MQTT broker for testing the DB9 firmware.
Should not be used for production, only for testing
"""
import asyncio
import logging
from amqtt.broker import Broker

logging.basicConfig(level=logging.INFO, format="%(asctime)s %(name)s %(message)s")

config = {
    "listeners": {
        "default": {
            "type": "tcp",
            "bind": "0.0.0.0:1883",
        },
    },
    "sys_interval": 0,  # disable periodic $SYS stats topic for a quieter log
    "auth": {
        "allow-anonymous": True,
    },
    "topic-check": {
        "enabled": False,
    },
}


async def main():
    broker = Broker(config)
    await broker.start()
    print("Broker running on 0.0.0.0:1883 — Ctrl+C to stop")
    try:
        await asyncio.Event().wait()  # run forever
    finally:
        await broker.shutdown()


if __name__ == "__main__":
    asyncio.run(main())
