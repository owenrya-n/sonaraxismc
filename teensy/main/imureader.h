#ifndef IMUREADER_H
#define IMUREADER_H

#include <Wire.h>

class IMUReader {
public:
  static void begin();
  static void update();
  static float getXAxisTangent();
  static float getYAxisTangent();
  static float getZAxisTangent();
};

#endif
