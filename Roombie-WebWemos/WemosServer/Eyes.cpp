#include "Eyes.h"

Eyes::Eyes(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b)
{
  pinMode(r, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(b, OUTPUT);

  analogWrite(r, 255);
  analogWrite(g, 0);
  analogWrite(b, 0);
}

void Eyes::set(uint8_t happiness)
{
  uint8_t targetR, targetG, targetB;

  if (happiness <= 63)
  {
    // Angry: More red, less green and blue
    targetR = 255;
    targetG = 0;
    targetB = 0;
  }
  else if (happiness <= 191)
  {
    // Neutral (White): More equal mix of red, green, and blue
    targetR = 0;
    targetG = 0;
    targetB = 255;
  }
  else
  {
    // Happy: More green, less red and blue
    targetR = 0;
    targetG = 255;
    targetB = 0;
  }

  analogWrite(r, targetR);
  analogWrite(g, targetG);
  analogWrite(b, targetB);
}