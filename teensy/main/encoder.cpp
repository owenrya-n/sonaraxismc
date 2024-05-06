#include "Encoder.h"

Encoder::Encoder(int pin_A, int pin_B) : encoder_A_pin(pin_A), encoder_B_pin(pin_B) {
  encoder = new Encoder(encoder_A_pin, encoder_B_pin);
}

long Encoder::read() {
  return Encoder.read();
}

void Encoder::reset() {
  Encoder.write(0);
}
