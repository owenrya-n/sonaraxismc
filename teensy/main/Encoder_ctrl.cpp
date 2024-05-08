// Encoder_ctrl.cpp

#include "Encoder_ctrl.h"

Encoder_ctrl::Encoder_ctrl(int pinA, int pinB) : encoder_instance(pinA, pinB) {}

void Encoder_ctrl::reset() {
    encoder_instance.write(0);
}

float Encoder_ctrl::read() {  
    return encoder_instance.read() * -encoder_sf;
}
