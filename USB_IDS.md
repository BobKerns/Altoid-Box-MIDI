# About USB Vendor ID's and Product ID's (VID/PID)

You need a unique VID/PID (Vendor/Product ID) pair to distinguish your type of USB device from others.

The [USB-IF](https://www.usb.org/) is essentially an industry cartel, and want [\$6000 USD (up from \$5000) from you before they will
assign you a Vendor ID](https://www.usb.org/getting-vendor-id) (and the associated `0x0000-0xffff` block of product IDs). But if you don't have a signed agreement with them, and you don't infringe on their trademarks, there is nothing they can do to you even [if their lawyers get shouty](http://www.arachnidlabs.com/blog/2013/10/18/usb-if-no-vid-for-open-source/).

The real issues are practical—you don't want your device to be mistaken for another by using
a conflicting VID/PID pair. The pair needs to be unique to your product
(even if it's a one-off).

Short of dropping \$6000 for the benefit of the cartel, here are a few options.

0. Pay \$6000, agree to USB-IF's terms.
1. Obtain a PID from a vendor under their VID. The cartel modified their member agreements to forbid this, but some vendors' agreements predate this change.
2. A few defunct vendors have made their VIDs availble and are now managed by open source groups.
3. Use a testing PID or unused PID in a vendor's range, for private experimentation.
4. The USB-IF claims to offer prototype PIDs, if you agree to never use it in a product.
5. Completely wing it, and make up a VID/PID pair and hope it doesn't conflict.

[Option 3](#option3) and [option 4](#option4) are viable, and even [option 5](#option5), if you never provide your project to others, either as DIY or finished items (hardware or software).

But if you are sharing, you need [option 1](#option1) or [option 2](#option2) (or [option 0](#option0)). Standards, and standards organizations are good things; it is only when they act as cartels protecting the interests of established players through exclusionary barriers that they become a problem.

[Wouter van Ooijen at voti.nl used to sell PID's under his VID, until the USB-IF sent him a threatening letter](https://www.voti.nl/pids/).

Putting up barriers like works against the very purpose of standards, forcing people onto alternate tracks, such as [options 3-5](#option3).

So I strongly recommend avoiding creating chaos, and working around the cartel's restrictions while retaining the goal of avoiding VID/PID conflicts.

To that end, here are some VIDs associated with [option 1](#option1), [option 2](#option2), and [option 3](#option3).

## <a name="option0"/>Option 0, Official USB-IF Vendor ID allocation

You have \$6000 to spend? USB certification and logo licensing are important to you? Perhaps you even wish to become a full member? Great!

URL: [Getting a Vendor ID from the USB-IF](https://www.usb.org/getting-vendor-id)

## <a name="option1"/>Option 1, Vendors

### FTDI

FTDI makes blocks of 8 PID's available for free for use by manufacturers using FTDI devices.

URL: http://www.ftdichip.com/Support/Knowledgebase/index.html?caniuseftdisvidformypr.htm
```c++
const uint16_t FTDI_VID = 0x0403;
```

### Microchip
Microchip has an online form for their customers to apply for a PID. It requires specifying which Microchip USB product you will be using.

URL: https://www.microchip.com/usblicensing/
```C++
const uint16_t MICROCHIP_VID = 0x04d8;
```

## <a name="option2"/>Option 2, Defunct Vendors

## <a name="openmoku"/>OpenMoku
OpenMoku was a modular phone vendor, which has ceased operations. They had the following VID's, and have passed them off to a group that now makes PID's available.

URL: https://github.com/openmoko/openmoko-usb-oui
```c++
const uint16_t OPENMOKU_VID = 0x1d50;
const uint16_t FIC_VID = 0x1457;
```

## <a name="pid.codes"/>pid.codes
The group [pid.codes](https://pid.codes/about/) provides PIDs via a github pull-request process. They currently have one VID donated by an unnamed company (and will accept donation of others).

This seems to me to be the most attractive method for simple open-source projects.

URL: https://pid.codes/howto/
```c++
const uint16_t PID_CODES_VID = 0x1209;
```

## <a name="option3"/>Option 3, Testing Ranges

These should not appear in your published code, executables, or distributed hardware. That way lies madness.

### <a name="lvr"/>Lakeview Research
Jan Axelson's Lakeview Research allows private usage in the range 0x9000 to 0x9100.

Note carefully the restriction #2.

 URL: http://janaxelson.com/development_tools.htm

>For lab use only

>If your device won't be distributed to unknown host systems, the only requirement is that the VID/PID pair doesn't match any VID/PID pair in the host's INF files (or equivalent database in non-Windows hosts).

>You can use Lakeview Research's VID (0925h) with PIDs from 9000h to 9100h at no charge under these conditions:

>1. You use the devices only in your own lab or office.

>2. You don't sell, give away, post online, or otherwise distribute source code or executable device containing Lakeview Research's Vendor ID.

>3. You don't complain to me if another device uses the same VID/PID pair that your device uses.
```c++
// Uncomment to use this locally, but do not distribute.
// const uint16_t LVR_VID = 0x0925;
 ```

## <a name="option4"/>Option 4, USB-IF Prototyping Range

You're on your own. All I have is this message to [Nick Johnson of Arachnidlabs](): The

>Hi Nick,
>
>We do have a vendor ID number designated for prototype products. This vendor ID number may not be used for a production product. Anybody who has such a need may contact us directly and we will provide them with the proto VID once they confirm that will not be making production products to be distributed and/or sold in the marketplace.
>
>Best regards, Traci

This does not seem an attractive option for anyone, but it lets them say "See, we do provide an option!". I am not impressed.

## <a name="option5"/>Option 5, Winging it
_I do not recommend this._ It works; I've done it during quick experiments. But [Option 4](#option4) has the same advantages, with less chance of introducing chaos down the line.

Even checking into a public github repository poses the risk of people blindly copying and not recognizing the potential for conflicts.

## <a name="references"/>References
* [Arachnidlabs' Nick Johnson's attempt](http://www.arachnidlabs.com/blog/2013/10/18/usb-if-no-vid-for-open-source/)
* [About pid.codes](https://pid.codes/about/)
* [USB IDs (unofficial list)](http://www.linux-usb.org/usb.ids)
* [Jan Axelson's pages](http://janaxelson.com/development_tools.htm#vendor_id)
* [Brian Benchoff's article at Hackday](https://hackaday.com/2013/10/22/usb-implementers-forum-says-no-to-open-source/)
* [Wooter van Ooijen's statements on why he stopped selling PIDs](https://www.voti.nl/docs/usb-pid.html)
  * [His now obsolete FAQ on buying PIDs](https://web.archive.org/save/https://www.voti.nl/pids/pidfaq.html)
