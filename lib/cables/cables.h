/**
 * @copyright Copyright (c) 2021 Bob Kerns
 * License: MIT
 */
#pragma once
#include <USB-MIDI.h>

// Define our virtual cables.
extern midi::MidiInterface<usbMidi::usbMidiTransport> CABLE1;
extern midi::MidiInterface<usbMidi::usbMidiTransport> CABLE2;
extern midi::MidiInterface<usbMidi::usbMidiTransport> CABLE3;
