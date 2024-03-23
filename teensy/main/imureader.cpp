#include "imureader.h"
#include <ICM_20948.h>

ICM_20948_I2C myICM;

float xAxisTangent = 0.0;
float yAxisTangent = 0.0;
float zAxisTangent = 0.0;

void IMUReader::begin() {
  Wire1.begin();
  myICM.begin(Wire1);

  Serial.begin(115200);
  while (!Serial) {
  }
  Serial.println("IMU Datastream Begins..");
}

void IMUReader::update() {
  if (myICM.dataReady()) {
    myICM.getAGMT();
    delay(30);
    xAxisTangent = atan2(myICM.accX(), myICM.accY()) * 180 / M_PI;
    yAxisTangent = atan2(myICM.accX(), myICM.accZ()) * 180 / M_PI;
    zAxisTangent = atan2(myICM.accY(), myICM.accZ()) * 180 / M_PI;
    //Serial.println(xAxisTangent);
  } else {
    Serial.println("Waiting for data");
    delay(500);
  }
}

float IMUReader::getXAxisTangent() {
  return xAxisTangent;
}

float IMUReader::getYAxisTangent() {
  return yAxisTangent;
}

float IMUReader::getZAxisTangent() {
  return zAxisTangent;
}