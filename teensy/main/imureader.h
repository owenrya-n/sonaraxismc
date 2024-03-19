#ifndef IMUREADER_H
#define IMUREADER_H

#include <Wire.h>

class IMUReader {
public:
  static void begin();
  static void update();
   float xAxisTangent;
   float yAxisTangent;
   float zAxisTangent;

};

#endif
