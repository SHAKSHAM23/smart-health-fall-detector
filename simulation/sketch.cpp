#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

// ---------------- PIN CONFIGURATION ----------------

#define SDA_PIN 21
#define SCL_PIN 22
#define HEART_RATE_PIN 34

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C

// ---------------- OBJECTS ----------------

Adafruit_MPU6050 mpu;

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

// ---------------- TIMING ----------------

unsigned long lastDisplayUpdate = 0;
unsigned long lastSerialUpdate = 0;

const unsigned long DISPLAY_INTERVAL = 200;
const unsigned long SERIAL_INTERVAL = 250;

// ---------------- FALL DETECTION ----------------

enum FallState {
  NORMAL,
  FREE_FALL,
  IMPACT_DETECTED,
  FALL_CONFIRMED
};

FallState currentState = NORMAL;

const float FREE_FALL_THRESHOLD = 3.0;
const float IMPACT_THRESHOLD = 18.0;
const float ROTATION_THRESHOLD = 4.0;

const float NORMAL_ACC_MIN = 7.0;
const float NORMAL_ACC_MAX = 12.0;

const unsigned long FREE_FALL_WINDOW = 1500;
const unsigned long IMPACT_WINDOW = 1200;
const unsigned long RECOVERY_TIME = 2000;

unsigned long stateStartTime = 0;
unsigned long recoveryStartTime = 0;

// ---------------- HEART RATE ----------------

enum HRStatus {
  HR_NORMAL,
  HR_LOW,
  HR_HIGH
};

HRStatus hrStatus = HR_NORMAL;

const int LOW_HR_LIMIT = 60;
const int HIGH_HR_LIMIT = 100;

const int LOW_HR_CLEAR = 65;
const int HIGH_HR_CLEAR = 95;

const unsigned long HR_ALERT_DELAY = 5000;

unsigned long abnormalHRStart = 0;

int heartRate = 0;

// ---------------- SENSOR VARIABLES ----------------

float totalAcceleration = 0;
float totalRotation = 0;

float ax = 0;
float ay = 0;
float az = 0;

float gx = 0;
float gy = 0;
float gz = 0;

bool freeFallNow = false;
bool impactNow = false;
bool rotationNow = false;

// ---------------- STATUS TEXT ----------------

const char* getFallStatus() {
  switch (currentState) {
    case NORMAL:
      return "NORMAL";

    case FREE_FALL:
      return "FREE FALL";

    case IMPACT_DETECTED:
      return "IMPACT";

    case FALL_CONFIRMED:
      return "FALL ALERT";
  }

  return "UNKNOWN";
}

const char* getHRStatus() {
  switch (hrStatus) {
    case HR_NORMAL:
      return "NORMAL";

    case HR_LOW:
      return "LOW HR";

    case HR_HIGH:
      return "HIGH HR";
  }

  return "UNKNOWN";
}

// ---------------- HEART RATE UPDATE ----------------

void updateHeartRate(int bpm) {

  bool abnormal = false;

  if (hrStatus == HR_NORMAL) {

    abnormal = (bpm < LOW_HR_LIMIT ||
                bpm > HIGH_HR_LIMIT);

    if (abnormal) {

      if (abnormalHRStart == 0) {
        abnormalHRStart = millis();
      }

      if (millis() - abnormalHRStart >= HR_ALERT_DELAY) {

        if (bpm < LOW_HR_LIMIT) {
          hrStatus = HR_LOW;
        } else {
          hrStatus = HR_HIGH;
        }

        abnormalHRStart = 0;
      }

    } else {
      abnormalHRStart = 0;
    }
  }

  else if (hrStatus == HR_HIGH) {

    if (bpm <= HIGH_HR_CLEAR) {
      hrStatus = HR_NORMAL;
      abnormalHRStart = 0;
    }
  }

  else if (hrStatus == HR_LOW) {

    if (bpm >= LOW_HR_CLEAR) {
      hrStatus = HR_NORMAL;
      abnormalHRStart = 0;
    }
  }
}

// ---------------- FALL DETECTION UPDATE ----------------

void updateFallDetection(float acceleration,
                         float rotation) {

  unsigned long currentTime = millis();

  freeFallNow = acceleration < FREE_FALL_THRESHOLD;
  impactNow = acceleration > IMPACT_THRESHOLD;
  rotationNow = rotation > ROTATION_THRESHOLD;

  switch (currentState) {

    case NORMAL:

      if (freeFallNow) {

        currentState = FREE_FALL;
        stateStartTime = currentTime;
      }

      break;

    case FREE_FALL:

      if (impactNow &&
          currentTime - stateStartTime <= FREE_FALL_WINDOW) {

        currentState = IMPACT_DETECTED;
        stateStartTime = currentTime;
      }

      else if (currentTime - stateStartTime >
               FREE_FALL_WINDOW) {

        currentState = NORMAL;
      }

      break;

    case IMPACT_DETECTED:

      if (rotationNow &&
          currentTime - stateStartTime <= IMPACT_WINDOW) {

        currentState = FALL_CONFIRMED;
        stateStartTime = currentTime;
        recoveryStartTime = 0;
      }

      else if (currentTime - stateStartTime >
               IMPACT_WINDOW) {

        currentState = NORMAL;
      }

      break;

    case FALL_CONFIRMED:

      if (acceleration >= NORMAL_ACC_MIN &&
          acceleration <= NORMAL_ACC_MAX &&
          rotation < ROTATION_THRESHOLD) {

        if (recoveryStartTime == 0) {
          recoveryStartTime = currentTime;
        }

        if (currentTime - recoveryStartTime >= RECOVERY_TIME) {

          currentState = NORMAL;
          recoveryStartTime = 0;
        }

      } else {

        recoveryStartTime = 0;
      }

      break;
  }
}

// ---------------- OLED DISPLAY ----------------

void updateOLED() {

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("SMART HEALTH MONITOR");

  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

  display.setCursor(0, 14);
  display.print("HR: ");
  display.print(heartRate);
  display.print(" BPM");

  display.setCursor(0, 25);
  display.print("HR STATUS: ");
  display.println(getHRStatus());

  display.setCursor(0, 36);
  display.print("ACC: ");
  display.print(totalAcceleration, 1);
  display.print(" m/s2");

  display.setCursor(0, 46);
  display.print("ROT: ");
  display.print(totalRotation, 1);

  display.setCursor(0, 56);
  display.print("FALL: ");
  display.print(getFallStatus());

  display.display();
}

// ---------------- SERIAL MONITOR ----------------

void updateSerialMonitor() {

  Serial.printf(
    "HR:%d BPM | HR:%s | ACC:%.2f m/s2 | ROT:%.2f rad/s | FREE:%s | IMPACT:%s | ROTATION:%s | FALL:%s\n",
    heartRate,
    getHRStatus(),
    totalAcceleration,
    totalRotation,
    freeFallNow ? "YES" : "NO",
    impactNow ? "YES" : "NO",
    rotationNow ? "YES" : "NO",
    getFallStatus()
  );
}

// ---------------- SETUP ----------------

void setup() {

  Serial.begin(115200);

  pinMode(HEART_RATE_PIN, INPUT);

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        OLED_ADDRESS)) {

    Serial.println("OLED initialization failed!");
    while (true) {}
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Initializing...");
  display.display();

  if (!mpu.begin(0x68)) {

    Serial.println("MPU6050 not detected!");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("MPU6050 ERROR");
    display.display();

    while (true) {}
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_44_HZ);

  Serial.println("SMART HEALTH MONITOR");
  Serial.println("System initialized successfully.");

  delay(500);
}

// ---------------- MAIN LOOP ----------------

void loop() {

  // Read MPU6050 continuously

  sensors_event_t acceleration;
  sensors_event_t gyro;
  sensors_event_t temperature;

  mpu.getEvent(
    &acceleration,
    &gyro,
    &temperature
  );

  ax = acceleration.acceleration.x;
  ay = acceleration.acceleration.y;
  az = acceleration.acceleration.z;

  gx = gyro.gyro.x;
  gy = gyro.gyro.y;
  gz = gyro.gyro.z;

  // Calculate acceleration magnitude

  totalAcceleration = sqrt(
    ax * ax +
    ay * ay +
    az * az
  );

  // Calculate rotation magnitude

  totalRotation = sqrt(
    gx * gx +
    gy * gy +
    gz * gz
  );

  // Read simulated heart rate

  int sensorValue = analogRead(HEART_RATE_PIN);

  heartRate = map(
    sensorValue,
    0,
    4095,
    50,
    150
  );

  // Process alerts continuously

  updateHeartRate(heartRate);

  updateFallDetection(
    totalAcceleration,
    totalRotation
  );

  // OLED refresh independently

  if (millis() - lastDisplayUpdate >= DISPLAY_INTERVAL) {

    lastDisplayUpdate = millis();

    updateOLED();
  }

  // Serial output independently

  if (millis() - lastSerialUpdate >= SERIAL_INTERVAL) {

    lastSerialUpdate = millis();

    updateSerialMonitor();
  }
}
