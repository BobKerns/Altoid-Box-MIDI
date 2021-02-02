#include <USB-MIDI.h>

#include "Callback.h"
#include "Knob.h"
#include "Menu.h"
#include "DisplayMgr.h"

using DMenu = Menu<Display>;

USBMIDI_CREATE_INSTANCE(0, CABLE1);
//USBMIDI_CREATE_INSTANCE(1, CABLE2);
//USBMIDI_CREATE_INSTANCE(2, CABLE3);

void onNoteOn(byte cable, byte channel, byte note, byte velocity);
void onNoteOff(byte cable, byte channel, byte note, byte velocity);

Knob knobA("Casio", A9, A10, A8);
Knob knobB("Keylab", A2, A3, A1);
Knob knobC("Atom SQ", A6, A7, A0);

const char *selected[] = { "--??--", "--??--", "--??--"};

const char* const PROGMEM programs[] = {"Off", "Lead", "Piano", "Orch+Piano", "Orchestra", "Orch+Pad", "Pad", "Reed", "Flutes", "Brass", "Strings", "B", "C4", "C#4", "Percussion", "Tuned Perc", "E4", "F4", "F#4", "Solo 1", "Solo 2", "Solo 3", "Solo 4", "FX"};
const uint8_t num_programs = sizeof(programs)/sizeof(const char *);
DMenu programMenu(num_programs, programs);

const char* const PROGMEM kits[] = {"Percussion", "Tuned Perc", "E4", "F4", "F#4", "Solo 1", "Solo 2", "Solo 3", "Solo 4", "FX"};
const uint8_t num_kits = sizeof(kits)/sizeof(const char *);
DMenu kitMenu(num_kits, kits);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  CABLE1.begin(MIDI_CHANNEL_OMNI);
  CABLE1.setHandleNoteOn([](byte channel, byte note, byte velocity){onNoteOn(1, channel, note, velocity);});
  //CABLE2.begin(MIDI_CHANNEL_OMNI);
  CABLE1.setHandleNoteOff([](byte channel, byte note, byte velocity){onNoteOff(1, channel, note, velocity);});
  //CABLE3.begin(MIDI_CHANNEL_OMNI);
  //CABLE2.setHandleNoteOn([](byte channel, byte note, byte velocity){onNoteOn(2, channel, note, velocity);});
  //CABLE2.setHandleNoteOff([](byte channel, byte note, byte velocity){onNoteOff(2, channel, note, velocity);});
  //CABLE3.setHandleNoteOn([](byte channel, byte note, byte velocity){onNoteOn(3, channel, note, velocity);});
  //CABLE3.setHandleNoteOff([](byte channel, byte note, byte velocity){onNoteOff(3, channel, note, velocity);});
  auto config = [](Knob &knob, DMenu &menu, uint8_t chan){
    knob
    .range(0, menu.size() - 1, true)
    .precision(Knob::Precision::NORMAL)
    .onChange([&menu, chan](auto knob, auto old, auto pos){
      onKnobChange(knob, menu, chan, pos);
    })
    .onPress([](Knob& knob, bool state){
      showBodyFor(1000, [state]{
        display.printFixedN(0, 16, state ? "ON" : "OFF", STYLE_BOLD, FONT_SIZE_2X);
      });
    })
    .onRelease([](Knob& knob, bool state){
      showBodyFor(1000, [state]{
        display.printFixedN(0, 16, state ? "ON" : "OFF", STYLE_BOLD, FONT_SIZE_2X);
      });
    })
    .start();
  };
  config(knobA, programMenu, 1);
  config(knobB, programMenu, 2);
  config(knobC, kitMenu, 3);
 
  display.begin();
  showBodyFor(10000, []{
    display.printFixedN (0, 16, "BobKerns", STYLE_BOLD, FONT_SIZE_2X);
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

void onKnobChange(const Knob& knob, DMenu &menu, uint8_t channel, uint32_t pos) {
  CABLE1.sendProgramChange(pos & 255, channel);
  Serial.print(knob.getName());
  Serial.print(": ");
  Serial.print(pos, DEC);
  Serial.print("[");
  Serial.print(channel);
  Serial.println("]");
  showFor(10000, [knob, pos]() {
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
    /*
    const auto [rangeMin, rangeMax, wrap] = knob.getRange();
    const auto size = rangeMax - rangeMin;
    const auto relative = pos - rangeMin;
    const auto percent = (relative * 100) / size;
    display.drawProgressBar(percent);
    */
    menu.select(pos);
    menu.draw(display);
  });
}

void onClick(bool on) {
  
}

const char * const PROGMEM NOTES[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

void onNoteOn(byte cable, byte channel, byte note, byte velocity) {
  noteMsg(true, cable, " ON", channel, note, velocity);
}


void onNoteOff(byte cable, byte channel, byte note, byte velocity) {
  noteMsg(false, cable, "OFF", channel, note, velocity);
}

int notes_on = 0;

void noteMsg(boolean on, byte cable, const char* msg, byte channel, byte note, byte velocity) {
    notes_on += (on ? 1 : -1);
    const char * note_name = NOTES[((int)note) % 12];
    int octave = ((int) note) / 12 - 2;
    digitalWrite(LED_BUILTIN, notes_on > 0 ? LOW : HIGH);
    Serial.print(cable);
    Serial.print('!');
    Serial.print(channel);
    Serial.print(':');
    Serial.print(msg);
    Serial.print(' ');
    Serial.print(note_name);
    Serial.print(octave);
    Serial.print("@");
    Serial.print(velocity);
    Serial.print('\n');
}

void defaultDisplayHead() {
  display.printFixed(0, 0, "Altoids MIDI Box", STYLE_NORMAL);
}

void defaultDisplayBody() {
  display.printFixed(0, 16, programs[knobA.read()], STYLE_BOLD);
  display.printFixed(0, 32, programs[knobB.read()], STYLE_BOLD);
  display.printFixed(0, 48, kits[knobC.read()], STYLE_BOLD);
}
