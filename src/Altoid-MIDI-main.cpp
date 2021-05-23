
#include <Arduino.h>
#include <USB-MIDI.h>
#include "Callback.h"
#include "Knob.h"
#include "Menu.h"
#include "DisplayMgr.h"
#include "ChannelState.h"
#include "Window.h"
#include <string>
#include "debug.h"

using DMenu = Menu<Display>;

USBMIDI_CREATE_INSTANCE(0, CABLE1);
//USBMIDI_CREATE_INSTANCE(1, CABLE2);
//USBMIDI_CREATE_INSTANCE(2, CABLE3);

void onNoteOn(byte cable, byte channel, byte note, byte velocity);
void onNoteOff(byte cable, byte channel, byte note, byte velocity);
void onProgramChange(byte cable,  byte channel, byte b2);
void onKnobChange(const Knob& knob, DMenu &menu, uint8_t channel, uint32_t pos);
void onKnobClick(const Knob& knob, uint8_t channel);
void noteMsg(boolean on, byte cable, const char* msg, byte channel, byte note, byte velocity);

Knob knobA("Casio", A7, A8, A9);
Knob knobB("Keylab", A1, A2, A3);
Knob knobC("Atom SQ", A10, A0, A6);

ChannelState currentState[] = {
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
uint32_t last_send = 0;
const uint32_t send_delay = 500;

void setup() {
  delay(2000);
  if(DEBUG) {
    Serial.begin(9600);
  }

  pinMode(LED_BUILTIN, OUTPUT);

  CABLE1.begin(MIDI_CHANNEL_OMNI);
  CABLE1.setHandleNoteOn([](byte channel, byte note, byte velocity){onNoteOn(1, channel, note, velocity);});
  CABLE1.setHandleNoteOff([](byte channel, byte note, byte velocity){onNoteOff(1, channel, note, velocity);});
  CABLE1.setHandleProgramChange([](byte channel, byte b2){onProgramChange(1, channel, b2);});
  //CABLE2.begin(MIDI_CHANNEL_OMNI);
  //CABLE3.begin(MIDI_CHANNEL_OMNI);
  //CABLE2.setHandleNoteOn([](byte channel, byte note, byte velocity){onNoteOn(2, channel, note, velocity);});
  //CABLE2.setHandleNoteOff([](byte channel, byte note, byte velocity){onNoteOff(2, channel, note, velocity);});
  //CABLE3.setHandleNoteOn([](byte channel, byte note, byte velocity){onNoteOn(3, channel, note, velocity);});
  //CABLE3.setHandleNoteOff([](byte channel, byte note, byte velocity){onNoteOff(3, channel, note, velocity);});

  auto config = [](Knob &knob, DMenu &menu, uint8_t chan){
    knob
    .range(0, menu.size() - 1, true)
    .precision(Knob::Precision::NORMAL)
    .onChange([&menu, chan](Knob & knob, int old, int pos){
      onKnobChange(knob, menu, chan, pos);
    })
    .onPress([chan](Knob& knob, bool state){
      onKnobClick(knob, chan);
    })
    .start(Knob::PULLUP, Knob::PULLUP, Knob::PULLUP);
  };
  programMenu.wrap();
  kitMenu.wrap();
  config(knobA, programMenu, 16);
  config(knobB, programMenu, 1);
  config(knobC, kitMenu, 10);

  display.begin();
  display.clear();
  display.setTextCursor(0, 0);
  display.setOffset(0, 0);
  display.setFixedFont(ssd1306xled_font8x16);

  showBodyFor(10000, []{

    display.printFixedN (0, 8, "BobKerns", STYLE_BOLD, FONT_SIZE_2X);
  });
}

void loop() {
  doDisplay();

  CABLE1.read();
  //CABLE2.read();
  //CABLE3.read();

  knobA.poll();
  knobB.poll();
  knobC.poll();
}

const char * PROGMEM DIGITS = "0123456789abcdef";


const char * const PROGMEM NOTES[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

std::string noteName(int note) {
  const char * note_name = NOTES[((int)note) % 12];
  int octave = ((int) note) / 12 - 2;
  return std::string(note_name) + std::to_string(octave) + (strlen(note_name) < 2 ? " " : "");
}


void sendProgramChange(uint8_t program, uint8_t channel) {
  last_send = millis();
  currentState[channel - 1].keys.doKeys([channel](uint8_t key){
    CABLE1.sendNoteOff(key, 0, channel);
    return false;
  });
  CABLE1.sendProgramChange(program, channel);
}

void onKnobClick(const Knob& knob, uint8_t channel) {
  auto &state = currentState[channel - 1];
  if (state.on) {
    state.on = false;
    sendProgramChange(0, channel);
    showBodyFor(1000, []{
      display.printFixedN(0, 16, "OFF", STYLE_BOLD, FONT_SIZE_2X);
    });
  } else {
    state.on = true;
    sendProgramChange(state.program, channel);
    showBodyFor(1000, [state]{
      display.printFixedN(0, 16, state.programName, STYLE_BOLD, FONT_SIZE_2X);
    });
  }
}

void onKnobChange(const Knob& knob, DMenu &menu, uint8_t channel, uint32_t pos) {
  auto &state = currentState[channel - 1];
  state.program = pos & 255;
  state.programName = menu.item(state.program);
  state.on = true;
  sendProgramChange(pos & 255, channel);
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
  }, [&menu, pos] {
    menu.select(pos);
    menu.draw(display);
  });
}

void onNoteOn(byte cable, byte channel, byte note, byte velocity) {
  if (velocity > 0) {
    currentState[channel - 1].keys.down(note);
    noteMsg(true, cable, " ON", channel, note, velocity);
  } else {
    onNoteOff(cable, channel, note, velocity);
  }
}


void onNoteOff(byte cable, byte channel, byte note, byte velocity) {
  currentState[channel - 1].keys.up(note);
  noteMsg(false, cable, "OFF", channel, note, velocity);
}

int notes_on = 0;

void noteMsg(boolean on, byte cable, const char* msg, byte channel, byte note, byte velocity) {
  notes_on += (on ? 1 : -1);
  digitalWrite(LED_BUILTIN, notes_on > 0 ? LOW : HIGH);
  if (DEBUG) {
    if (last_send + send_delay <= millis()) {
        std::string txt =  std::to_string(cable) + "!" + std::to_string(channel) + ":" + msg + " " + noteName(note) + "@" + std::to_string(velocity);
        showHeadFor(500, [txt]{
        display.invertColors();
        display.printFixed(0, 0, txt.c_str(), STYLE_NORMAL);
        display.invertColors();
        });
    }
  }
}


void onProgramChange(byte cable,  byte channel, byte b2) {
  byte pgm = b2 % num_programs;
  currentState[channel - 1].program = pgm;
  currentState[channel - 1].programName = programs[pgm];
  if (currentState[channel - 1].knob) {
      currentState[channel - 1].knob->write(pgm );
  }
  if (DEBUG) {
    if (last_send + send_delay <= millis()) {
        std::string txt =  std::to_string(cable) + "!" + std::to_string(channel) + ":PGM" + " #" + std::to_string(b2);
        showHeadFor(500, [txt]{
        display.invertColors();
        display.printFixed(0, 0, txt.c_str(), STYLE_NORMAL);
        display.invertColors();
        });
    }
  }
}


void defaultDisplayHead() {
  display.printFixed(0, 0, "Altoids MIDI Box", STYLE_NORMAL);
}

void defaultDisplayBody() {
  auto line = [](uint8_t i, uint8_t channel){
    auto &state = currentState[channel- 1];
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
