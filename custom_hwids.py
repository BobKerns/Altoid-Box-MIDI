Import("env")

board_config = env.BoardConfig()


# VID/PID pair assigned by picodes at https://github.com/pidcodes/pidcodes.github.com/blob/master/1209/C10C/index.md

board_config.update("build.hwids", [[
        "0x1209", # picodes VID
        "0xC10C"  # PID for this project
      ],
      [
        "0x2886", # From the standard board configuration. Seeeduino's VID
        "0x002F"  # Seeeduino PID for this board. Bootloader?
      ]
])

board_config.update("build.usb_product", "Altoid MIDIBox")
