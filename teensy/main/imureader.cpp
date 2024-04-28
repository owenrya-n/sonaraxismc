#include "imureader.h"
#include <ICM_20948.h>

ICM_20948_I2C myICM;

float xAxisTangent = 0.0;
float yAxisTangent = 0.0;
float zAxisTangent = 0.0;
int count = 100000;

void IMUReader::begin() {
  Wire1.begin();
  myICM.begin(Wire1);

  Serial.begin(115200);
  while (!Serial) {}
  if (myICM.dataReady()) {
    Serial.println("Status: IMU detected on Wire1");
  }
  else {
    Serial.println("Error: IMU not detected on Wire1");
  }

}

void IMUReader::update() {
  if (myICM.dataReady()) {
    myICM.getAGMT();
    delay(30);
    float sumXAxis = 0.0;
    float sumYAxis = 0.0;
    float sumZAxis = 0.0;

    for (int i = 0; i < count; i++) {
      sumXAxis += atan2(myICM.accX(), myICM.accY()) * 180 / M_PI; 
      sumYAxis += atan2(myICM.accX(), myICM.accZ()) * 180 / M_PI;
      sumZAxis += atan2(myICM.accY(), myICM.accZ()) * 180 / M_PI;
    }

    xAxisTangent = sumXAxis / count;
    yAxisTangent = sumYAxis / count;
    zAxisTangent = sumZAxis / count;

  } else {
    Serial.println("Retrying IMU qwery");
    Serial.println("");
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
