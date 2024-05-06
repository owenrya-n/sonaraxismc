#ifndef ENCODER_H
#define ENCODER_H

class Encoder {
  private:
    int encoder_A_pin;
    int encoder_B_pin;

  public:
    Encoder(int pin_A, int pin_B);
    long read();
    void reset();
};

#endif
