// Encoder_ctrl.h

#ifndef ENCODER_CTRL_H
#define ENCODER_CTRL_H

#include <Encoder.h>

class Encoder_ctrl {
public:
    Encoder_ctrl(int pinA, int pinB); // Constructor with pin numbers

    void reset();
    float read();

private:
    Encoder encoder_instance; // Encoder instance
    int encoder_sf = 1;
};

#endif
