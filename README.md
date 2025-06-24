# CH572 Standard Library - English Translation

This repository contains versions of the CH572 Standard Peripheral Drivers and related header files with the Chinese comments translated to English.

## Purpose

The original CH572 microcontroller documentation and library files that contain many Chinese only comments. This repository aims to:
1. Provide English versions of the standard library files that are functionally identical with English comments 
2. Maintain a clean, version-controlled collection of these translated files
3. Serve as a drop-in replacement for the original Chinese library files

## Directory Structure

- `StdPeriphDriver/` - Contains standard peripheral driver source files
  - `inc/` - Header files for standard peripheral drivers
- Additional directories will be added as translation progresses

## Translation Status

- [x ] StdPeriphDriver
  - [x] CH57x_clk.c
  - [x] CH57x_cmp.c
  - [x] CH57x_flash.c
  - [x] CH57x_gpio.c
  - [x] CH57x_i2c.c
  - [x] CH57x_keyscan.c
  - [x] CH57x_pwm.c
  - [x] CH57x_pwr.c
  - [x] CH57x_spi.c
  - [x] CH57x_sys.c
  - [x] CH57x_timer.c
  - [x] CH57x_uart.c
  - [x] CH57x_usbdev.c
  - [x] CH57x_usbhostBase.c
  - [x] CH57x_usbhostClass.c

- [x] StdPeriphDriver Headers
  - [x] CH572SFR.h
  - [x] CH57x_clk.h
  - [x] CH57x_cmp.h
  - [x] CH57x_common.h
  - [x] CH57x_flash.h
  - [x] CH57x_gpio.h
  - [x] CH57x_i2c.h
  - [x] CH57x_keyscan.h
  - [x] CH57x_pwm.h
  - [x] CH57x_pwr.h
  - [x] CH57x_spi.h
  - [x] CH57x_sys.h
  - [x] CH57x_timer.h
  - [x] CH57x_uart.h
  - [x] CH57x_usbdev.h
  - [x] CH57x_usbhost.h
  - [x] ISP572.h

## Usage

These tranlated files can be:
1. Used in place of the equivilant chinese files in your projects
2. Copied to your MounRiver Studio template directory to be used so that they are used in new projects

## Original Source

These files are based on the original CH572 library code provided by WCH (Nanjing Qinheng Microelectronics Co., Ltd.).  The Chinese comments were auto-translated to English by Claude 2.7 Sonnet.

## Testing

These files were auto-translated by Claude 3.7 Sonnet and are largely untested.  We supervised Claude to see that was doing a good job, and it was, but the process may have introduced errors.  Use at your own risk.

## Copyright Notice

Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
This software (modified or not) and binary are used for microcontroller manufactured by Nanjing Qinheng Microelectronics.
