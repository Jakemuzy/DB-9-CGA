#!/bin/bash

# Used for conveniently activating esp environment and exporting variables
source ~/.espressif/tools/activate*.sh

DB9_ESPRESSIF_EXPORT_PATH=$(find ~/.espressif/v*/esp-idf/export.sh)
. "$DB9_ESPRESSIF_EXPORT_PATH"

idf.py add-dependency espressif/mqtt
