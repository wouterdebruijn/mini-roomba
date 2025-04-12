#include "Buzzer.h"

Buzzer::Buzzer(uint8_t pin, uint8_t id) : pin(pin), id(BUZZER_ID_MASK | id) {
  pinMode(pin, OUTPUT);
}

uint8_t Buzzer::getId() {
  return id;
}

uint8_t Buzzer::available() {
  return NOTES_L - currentNote;
}

// Handle one note and sleep
void Buzzer::handle() {
  if (currentNote < NOTES_L) {
    tone(pin, notes[currentNote], 250);
    delay(100);

    currentNote++;
  } else {
    currentNote = 0;
  }
}

void Buzzer::happy()
{
  currentNote = 0;
  notes[0] = 1600;
  notes[1] = 1800;
  notes[2] = 2000;
  notes[3] = 1800;
  notes[4] = 1600;
}

void Buzzer::sad()
{
  currentNote = 0;
  notes[0] = 400;
  notes[1] = 500;
  notes[2] = 600;
  notes[3] = 500;
  notes[4] = 400;
}

void Buzzer::angry()
{
  currentNote = 0;
  notes[0] = 600;
  notes[1] = 700;
  notes[2] = 600;
  notes[3] = 700;
  notes[4] = 600;
}

void Buzzer::surprise()
{
  currentNote = 0;
  notes[0] = 1000;
  notes[1] = 2000;
  notes[2] = 1500;
  notes[3] = 1200;
  notes[4] = 1800;
}