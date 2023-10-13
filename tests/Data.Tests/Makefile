# See https://github.com/bxparks/EpoxyDuino for documentation about using
# EpoxyDuino to compile and run AUnit tests natively on Linux or MacOS.make run

include ../../.env

APP_NAME := LocalStorageConfigTests
ARDUINO_LIBS := AUnit EpoxyEepromEsp EpoxyFS Data Local
ARDUINO_LIB_DIRS := \
	../../src \
	../../src/Data
include $(arduino_ide_dir)/libraries/EpoxyDuino/EpoxyDuino.mk