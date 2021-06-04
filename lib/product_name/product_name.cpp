/**
 * @copyright Copyright (c) 2021 Bob Kerns
 * License: MIT
 */
#include "product_name.h"
// External definitions from Arduino core.

extern uint8_t STRING_PRODUCT[];
extern uint8_t STRING_MANUFACTURER[];
extern DeviceDescriptor USB_DeviceDescriptorB;
extern DeviceDescriptor USB_DeviceDescriptor;


void renameUSB(uint8_t vid, uint8_t pid, char* manufacturer, char* product) {
    USB_DeviceDescriptor.idVendor = vid;
    USB_DeviceDescriptorB.idVendor =vid;
    USB_DeviceDescriptor.idProduct = pid;
    USB_DeviceDescriptorB.idProduct = pid;
    if (manufacturer) {
        strcpy((char*)STRING_MANUFACTURER, manufacturer);
    }
    if (product) {
        strcpy((char*)STRING_PRODUCT, product);
    }

    // Redo the initialization.
#if defined(USBCON)
    USBDevice.init();
    USBDevice.attach();
#endif
}