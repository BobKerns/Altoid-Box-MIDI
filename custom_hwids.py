Import("env")

board_config = env.BoardConfig()


# Supply the correct Uncomment the following code to set the VID/PID and product name,
# once they are assigned. The values below are from the standard board configuration.

#board_config.update("build.hwids", [[
#        "0x2886", # Update this w/ VID
#        "0x8024"  # Update this w/ PID
#      ],
#      [
#        "0x2886",
#        "0x002F"
#      ]
#])
#
#board_config.update("build.usb_product", "Altoid MIDIBox")
