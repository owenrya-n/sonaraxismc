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
  while (!Serial) {
  }
  Serial.println("IMU Datastream Begins..");
}

void IMUReader::update() {
  if (myICM.dataReady()) {
    myICM.getAGMT();
    delay(30);
    float sumXAxis = 0.0;
    float sumYAxis = 0.0;
    float sumZAxis = 0.0;

    for (int i = 0; i < count; i++) {
      sumXAxis += atan2(myICM.accX(), myICM.accY()) * 180 / M_PI; // Accumulate the values
      sumYAxis += atan2(myICM.accX(), myICM.accZ()) * 180 / M_PI;
      sumZAxis += atan2(myICM.accY(), myICM.accZ()) * 180 / M_PI;
      //delay(5); // Add a slight delay if needed to allow sensor to stabilize
    }

    // Average the accumulated values and assign to original variables
    xAxisTangent = sumXAxis / count;
    yAxisTangent = sumYAxis / count;
    zAxisTangent = sumZAxis / count;

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
