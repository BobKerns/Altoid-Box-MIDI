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



DisplaySSD1306_128x64_I2C display(-1);

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

const char *DIGITS = "0123456789abcdef";

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  //pinMode(PIN_LED2, OUTPUT);
  //pinMode(PIN_LED3, OUTPUT);
  pinMode(1, INPUT);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
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
  auto config = [](Knob &knob, uint8_t chan){
    knob
    .range(0, 255, true)
    .precision(Knob::Precision::NORMAL)
    .onChange([chan](auto knob, auto old, auto pos){
      onKnobChange(knob, 1, pos);
    })
    .start();
  };
  config(knobA, 1);
  config(knobB, 2);
  config(knobC, 3);
 
    display.begin();
    display.fill(0x02);
    display.setFixedFont(ssd1306xled_font6x8);
    Serial.flush();
}

void onKnobChange(const Knob& knob, uint8_t channel, uint32_t pos) {
  char buf[8];
  CABLE1.sendProgramChange(pos & 255, channel);
  const auto [rangeMin, rangeMax, wrap] = knob.getRawRange();
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
}

void onClick(byte n) {
  enc.write(n * 4);
}

const char * const NOTES[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

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
