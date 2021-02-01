#define PIN_LED_RXL (12u)
#define PIN_LED_TXL (11u)

#define PIN_LED2 PIN_LED_RXL
#define PIN_LED3 PIN_LED_TXL

#define KNOB1_CLICK A1
#define KNOB1_P1 A2
#define KNOB1_P2 A3

//define ENCODER_OPTIMIZE_INTERRUPTS
#define ENCODER_USE_INTERRUPTS
//define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Encoder.h>

#include <USB-MIDI.h>
#include "lcdgfx.h"

#include "Callback.h"
#include "Knob.h"
#include "Menu.h"

using Display = DisplaySSD1306_128x64_I2C;
Display display(-1);
using DMenu = Menu<Display>;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

USBMIDI_CREATE_INSTANCE(0, CABLE1);
USBMIDI_CREATE_INSTANCE(1, CABLE2);
USBMIDI_CREATE_INSTANCE(2, CABLE3);

enum MOP {
  NOTE_ON = 0x9,
  NOTE_OFF = 0x8,
  SYSEX = 0xF
};
void onNoteOn(byte cable, byte channel, byte note, byte velocity);
void onNoteOff(byte cable, byte channel, byte note, byte velocity);

Encoder enc(KNOB1_P1, KNOB1_P2);

Knob knobA("A", 9, 10, 8);
Knob knobB("B", 2, 3, 4);
Knob knobC("C", 6, 7, 1);

const char* const PROGMEM programs[] = {"Off", "Lead", "Piano", "Orch+Piano", "Orchestra", "Orch+Pad", "Pad", "Reed", "Flutes", "Brass", "Strings", "B", "C4", "C#4", "Percussion", "Tuned Perc", "E4", "F4", "F#4", "Solo 1", "Solo 2", "Solo 3", "Solo 4", "FX"};
const uint8_t num_programs = sizeof(programs)/sizeof(const char *);
DMenu programMenu(num_programs, programs);


const char* const PROGMEM kits[] = {"Percussion", "Tuned Perc", "E4", "F4", "F#4", "Solo 1", "Solo 2", "Solo 3", "Solo 4", "FX"};
const uint8_t num_kits = sizeof(kits)/sizeof(const char *);
DMenu kitMenu(num_kits, kits);

const char * PROGMEM DIGITS = "0123456789abcdef";

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  //pinMode(PIN_LED2, OUTPUT);
  //pinMode(PIN_LED3, OUTPUT);
  pinMode(1, INPUT);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
 // pinMode(4, OUTPUT);
 // pinMode(5, OUTPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT);
  //pinMode(A0, OUTPUT);
  //analogReference(AR_INTERNAL1V0);
  //analogWriteResolution(10);
  CABLE1.begin(MIDI_CHANNEL_OMNI);
  CABLE2.begin(MIDI_CHANNEL_OMNI);
  CABLE3.begin(MIDI_CHANNEL_OMNI);
  CABLE1.setHandleNoteOn([](byte channel, byte note, byte velocity){onNoteOn(1, channel, note, velocity);});
  CABLE1.setHandleNoteOff([](byte channel, byte note, byte velocity){onNoteOff(1, channel, note, velocity);});
  CABLE2.setHandleNoteOn([](byte channel, byte note, byte velocity){onNoteOn(2, channel, note, velocity);});
  CABLE2.setHandleNoteOff([](byte channel, byte note, byte velocity){onNoteOff(2, channel, note, velocity);});
  CABLE3.setHandleNoteOn([](byte channel, byte note, byte velocity){onNoteOn(3, channel, note, velocity);});
  CABLE3.setHandleNoteOff([](byte channel, byte note, byte velocity){onNoteOff(3, channel, note, velocity);});
  auto config = [](Knob &knob, DMenu &menu, uint8_t chan){
    knob
    .range(0, menu.size() - 1, true)
    .precision(Knob::Precision::NORMAL)
    .onChange([&menu, chan](auto knob, auto old, auto pos){
      onKnobChange(knob, menu, chan, pos);
    })
    .start();
  };
  config(knobA, programMenu, 1);
  config(knobB, programMenu, 2);
  config(knobC, kitMenu, 3);
 
    display.begin();
    display.fill(0x00);
    display.setOffset(0, 0);
    display.setTextCursor(0, 0);
    display.getInterface().setStartLine(0);
    display.clear();
    display.setFixedFont(ssd1306xled_font8x16);
    display.printFixed(0, 0, "Altoids MIDI Box", STYLE_NORMAL);
    display.printFixedN (0, 16, "BobKerns", STYLE_BOLD, FONT_SIZE_2X);
}

void onKnobChange(const Knob& knob, DMenu &menu, uint8_t channel, uint32_t pos) {
  char buf[8];
  CABLE1.sendProgramChange(pos & 255, channel);
  const auto [rangeMin, rangeMax, wrap] = knob.getRange();
  Serial.print(knob.getName());
  Serial.print(": ");
  Serial.print(pos, DEC);
  Serial.print("[");
  Serial.print(channel);
  Serial.println("]");
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
  display.clear();
  display.printFixed(0, 0, knob.getName(), STYLE_NORMAL);
  display.printFixed(103, 0, numStr, STYLE_NORMAL);
  const auto size = rangeMax - rangeMin;
  const auto relative = pos - rangeMin;
  const auto percent = (relative * 100) / size;
  //display.drawProgressBar(percent);
  menu.select(pos);
  menu.draw(display);
}

void onClick(byte n) {
  enc.write(n * 4);
}

const char * const PROGMEM NOTES[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

void clearLEDs() {
  digitalWrite(PIN_LED2, HIGH); 
  digitalWrite(PIN_LED3, HIGH); 
  digitalWrite(LED_BUILTIN, HIGH); 
  delay(500);                        // wait for a second
}

void setLED(int led) {
  digitalWrite(led, LOW);
  delay(500);                        // wait for a second
}

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

int oldPos = -9999999;
boolean oldButton;
byte oldQ = 0xff;

int oldState = -999;
int phase = 10;

void loop() {
  CABLE1.read();
  
  knobA.poll();
  knobB.poll();
  knobC.poll();
  //CABLE2.read();
  //CABLE3.read();
}
