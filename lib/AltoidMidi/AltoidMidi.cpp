// Main program
#include <Arduino.h>
#include "cables.h"
#include <Callback.h>
#include <Menu.h>
#include <DisplayMgr.h>
#include <string>
#include <debug.h>

#include "AltoidMidi.h"


Knob knobA("Casio", A7, A8, A9);
Knob knobB("Keylab", A1, A2, A3);
Knob knobC("Atom SQ", A10, A0, A6);

ChannelState ChannelState::currentState[] = {
  ChannelState(0, &knobB),
  ChannelState(1),
  ChannelState(2),
  ChannelState(3),
  ChannelState(4),
  ChannelState(5),
  ChannelState(6),
  ChannelState(7),
  ChannelState(8),
  ChannelState(9, &knobC),
  ChannelState(10),
  ChannelState(11),
  ChannelState(12),
  ChannelState(13),
  ChannelState(14),
  ChannelState(15, &knobA)
};

const char* const PROGMEM programs[] = {"Off", "Lead", "Piano", "Orch+Piano", "Orchestra", "Orch+Pad", "Pad", "Reed", "Flutes", "Brass", "Strings", "B", "C4", "C#4", "Percussion", "Tuned Perc", "E4", "F4", "F#4", "Solo 1", "Solo 2", "Solo 3", "Solo 4", "FX"};
const uint8_t num_programs = sizeof(programs)/sizeof(const char *);
DMenu programMenu(num_programs, programs);

const char* const PROGMEM kits[] = {"Percussion", "Tuned Perc", "E4", "F4", "F#4", "Solo 1", "Solo 2", "Solo 3", "Solo 4", "FX"};
const uint8_t num_kits = sizeof(kits)/sizeof(const char *);
DMenu kitMenu(num_kits, kits);