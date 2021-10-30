#include <Arduino.h>
#include <OscilloVectrex.h>

OscilloVectrex vectrex;

void setup() {
  vectrex.begin();
}

void loop() {
  // Full screen rectangle
  vectrex.rect(-127,-127,128,128);
  
  // Smaller rectangle (less than half screen)
  vectrex.rect(-50,-50,50,50);

  
}