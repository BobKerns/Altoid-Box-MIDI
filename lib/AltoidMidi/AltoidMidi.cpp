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


// When we last sent a message. We suppress display of incoming MIDI for 500 ms after sending
uint32_t last_receive = 0;
const uint32_t receive_display_delay = 500;


const char * PROGMEM DIGITS = "0123456789abcdef";


const char * const PROGMEM NOTES[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

std::string noteName(int note) {
  const char * note_name = NOTES[((int)note) % 12];
  int octave = ((int) note) / 12 - 2;
  return std::string(note_name) + std::to_string(octave) + (strlen(note_name) < 2 ? " " : "");
}

void onKnobClick(const Knob& knob, uint8_t channel) {
  auto &state = ChannelState::currentState[channel - 1];
  auto menu = state.menu;
  if (state.on) {
    state.on = false;
    auto itemName = menu->item(0);
    state.queueProgramChange(0, itemName);
    showBodyFor(1000, [itemName]{
      display.printFixedN(0, 16, itemName, STYLE_BOLD, FONT_SIZE_2X);
    });
  } else {
    state.on = true;
    auto itemName = menu->item(state.program);
    state.queueProgramChange(state.program, itemName);
    showBodyFor(1000, [itemName]{
      display.printFixedN(0, 16, itemName, STYLE_BOLD, FONT_SIZE_2X);
    });
  }
}

void onKnobChange(const Knob& knob, uint8_t channel, uint32_t pos) {
  auto &state = ChannelState::currentState[channel - 1];
  auto pgm = pos & 255;
  auto menu = state.menu;
  state.queueProgramChange(pgm, menu->item(pgm));
  showFor(5000, [knob, pos]() {
  char buf[8];
      buf[0] = DIGITS[(pos/100)%10];
      buf[1] = DIGITS[(pos/10)%10];
      buf[2] = DIGITS[pos%10];
      buf[3] = '\0';
      // Suppress leading zero and right-justify.
      auto numStr = buf;
      auto numStart = 127 - 6 * 3;
      if (buf[0] == '0') {
        numStr++;
        numStart += 6;
        if (buf[1] == '0') {
          numStr++;
          numStart += 6;
        }
      }
      display.printFixed(0, 0, knob.getName(), STYLE_NORMAL);
      display.printFixed(103, 0, numStr, STYLE_NORMAL);
  }, [menu, pos] {
    menu->select(pos);
    menu->draw(display);
  });
}

void onNoteOn(byte cable, byte channel, byte note, byte velocity) {
  if (velocity > 0) {
        if (DEBUG_MAIN) {
            debug((std::string("ON ") + std::to_string(channel) + " " + std::to_string(note)));
        }
        ChannelState::currentState[channel-1].keys.down(note);
        noteMsg(true, cable, " ON", channel, note, velocity);
    } else {
        onNoteOff(cable, channel, note, velocity);
    }
}


void onNoteOff(byte cable, byte channel, byte note, byte velocity) {
  ChannelState::currentState[channel-1].keys.up(note);
  noteMsg(false, cable, "OFF", channel, note, velocity);
}

int notes_on = 0;

void noteMsg(boolean on, byte cable, const char* msg, byte channel, byte note, byte velocity) {
  notes_on += (on ? 1 : -1);
  digitalWrite(LED_BUILTIN, notes_on > 0 ? LOW : HIGH);
  if (DEBUG_MAIN) {
    if (last_receive + receive_display_delay <= millis()) {
        std::string txt =  std::to_string(cable) + "!" + std::to_string(channel) + ":" + msg + " " + noteName(note) + "@" + std::to_string(velocity);
        debug(txt);
        showHeadFor(500, [txt]{
        display.invertColors();
        display.printFixed(0, 0, txt.c_str(), STYLE_NORMAL);
        display.invertColors();
        });
    }
  }
}


void onProgramChange(byte cable,  byte channel, byte b2) {
  byte pgm = b2 % programMenu.count;
  ChannelState::currentState[channel - 1].program = pgm;
  ChannelState::currentState[channel - 1].programName = programMenu.item(pgm);
  if (ChannelState::currentState[channel - 1].knob) {
      ChannelState::currentState[channel - 1].knob->write(pgm);
  }
  if (DEBUG_MAIN) {
    if (last_receive + receive_display_delay <= millis()) {
        std::string txt =  std::to_string(cable) + "!" + std::to_string(channel) + ":PGM" + " #" + std::to_string(b2);
        debug(txt);
        showHeadFor(500, [txt]{
        display.invertColors();
        display.printFixed(0, 0, txt.c_str(), STYLE_NORMAL);
        display.invertColors();
        });
    }
  }
}


void defaultDisplayHead() {
  display.printFixed(0, 0, DEBUG ? "DEBUG BOX" : "Altoids MIDI Box", STYLE_NORMAL);
}

void defaultDisplayBody() {
  auto line = [](uint8_t i, uint8_t channel){
    auto &state = ChannelState::currentState[channel- 1];
    if (state.on) {
      display.printFixed(0, i * 16, state.programName, STYLE_BOLD);
    } else {
      display.invertColors();
      display.printFixed(12, i * 16, state.programName, STYLE_NORMAL);
      display.invertColors();
    }
  };
  line(0, 16);
  line(1, 1);
  line(2, 10);
}
