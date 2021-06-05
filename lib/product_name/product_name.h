/**
 * @copyright Copyright (c) 2021 Bob Kerns
 * License: MIT
 */
#pragma once
#include <Arduino.h>

/**
 * @brief You need a unique VID/PID (Vendor/Product ID) pair to distinguish your USB device from others.
 *
 * See the [README.md](README.md) for more information.
 *
 */

/**
 * @brief FTDI makes blocks of 8 PID's available for free for use by manufacturers using FTDI devices.
 *
 * URL: http://www.ftdichip.com/Support/Knowledgebase/index.html?caniuseftdisvidformypr.htm
 */
const uint16_t FTDI_VID = 0x0403;

/**
 * @brief Microchip has an online form for their customers to apply for a PID. It requires specifying which Microchip USB product you will be using.

URL: https://www.microchip.com/usblicensing/
 */
const uint16_t MICROCHIP_VID = 0x04d8;

/**
 * @brief OpenMoku had the following VID's, and has made PID's available.
 *
URL:  * https://github.com/openmoko/openmoko-usb-oui
 */
const uint16_t OPENMOKU_VID = 0x1d50;
/**
 * @brief OpenMoku had the following VID's, and has made PID's available.
 *
 * URL: https://github.com/openmoko/openmoko-usb-oui
 */
const uint16_t FIC_VID = 0x1457;

/**
 * @brief pidcodes provides PIDs via a github pull-request process.
 *
 * URL: https://pid.codes/howto/
 */
const uint16_t PID_CODES_VID = 0x1209;

/**
 * @brief Jan Axelson's Lakeview Research allows private usage in the range 0x9000 to 0x9100.
 *
 * URL: http://janaxelson.com/development_tools.htm
 *
 * >For lab use only
 * >
 * >If your device won't be distributed to unknown host systems, the only requirement is that the VID/PID pair doesn't match any VID/PID pair in the host's INF files (or equivalent database in non-Windows hosts).
 * >
 * >You can use Lakeview Research's VID (0925h) with PIDs from 9000h to 9100h at no charge under these conditions:
 * >
 * >1. You use the devices only in your own lab or office.
 * >
 * >2. You don't sell, give away, post online, or otherwise distribute source code or executable code containing Lakeview Research's Vendor ID.
 * >
 * >3. You don't complain to me if another device uses the same VID/PID pair that your device uses."
 */
// Uncomment to use, but do not distribute uncommented!
// const uint16_t LVR_VID = 0x0925;

/**
 * @brief Wouter van Ooijen at VOTI used to sell PIDs under his VID until he got threats from the USB-IF.
 */

// Uncomment to use, but do not distribute uncommented!
// const uint16_t VOTI_VID =  0x16C0;

/**
 * @brief Rename the USB interface
 *
 * @param vid the vendor ID to use. See [README.md](README.md)
 * @param pid the product ID to use. See [README.md](README.md)
 * @param manufacturer a string or `nullptr`
 * @param product a string or `nullptr`
 */
extern void renameUSB(uint16_t vid, uint16_t pid, char* manufacturer = nullptr, char* product = nullptr);
