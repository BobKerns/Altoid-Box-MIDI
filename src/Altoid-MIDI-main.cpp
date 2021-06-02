// Main program
#include <Arduino.h>
#include "cables.h"
#include <string>
#include <debug.h>
#include <AltoidMidi.h>

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
      ChannelState::currentState[chan - 1].menu = &menu;
        knob
        .range(0, menu.size() - 1, true)
        .precision(Knob::Precision::NORMAL)
        .onChange([chan](Knob & knob, int old, int pos){
        onKnobChange(knob, chan, pos);
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

    display.printFixedN (0, 8, DEBUG ? "DEBUG" : "BobKerns", STYLE_BOLD, FONT_SIZE_2X);
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

  ChannelState::sendProgramChanges();
}
