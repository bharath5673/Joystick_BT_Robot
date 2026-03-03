#include <BluetoothSerial.h>
#include <ESP32Servo.h>

BluetoothSerial SerialBT;

Servo escLeft;
Servo escRight;

#define ESC_LEFT_PIN 12
#define ESC_RIGHT_PIN 13

int xAxis = 140;
int yAxis = 140;

// ESC signal range
int neutral = 1500;   // Stop
int minPulse = 1000;  // Full reverse
int maxPulse = 2000;  // Full forward

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_RC_CAR");

  escLeft.attach(ESC_LEFT_PIN);
  escRight.attach(ESC_RIGHT_PIN);

  // Arm ESC (very important!)
  escLeft.writeMicroseconds(neutral);
  escRight.writeMicroseconds(neutral);

  delay(1000);  // Wait for ESC arming

  Serial.println("ESC Ready...");
}

void loop() {

  while (SerialBT.available() >= 2) {
    xAxis = SerialBT.read();
    delay(10);
    yAxis = SerialBT.read();

    Serial.print(xAxis);
    Serial.print(",");
    Serial.println(yAxis);
  }

  int leftPulse = neutral;
  int rightPulse = neutral;

  // Forward / Backward
  if (yAxis < 130) {
    int speed = map(yAxis, 130, 60, 0, 500);
    leftPulse = neutral - speed;
    rightPulse = neutral - speed;
  }

  if (yAxis > 150) {
    int speed = map(yAxis, 150, 220, 0, 500);
    leftPulse = neutral + speed;
    rightPulse = neutral + speed;
  }

  // Turning
  if (xAxis < 130) {
    int turn = map(xAxis, 130, 60, 0, 400);
    leftPulse -= turn;
    rightPulse += turn;
  }

  if (xAxis > 150) {
    int turn = map(xAxis, 150, 220, 0, 400);
    leftPulse += turn;
    rightPulse -= turn;
  }

  // Limit safety
  leftPulse = constrain(leftPulse, minPulse, maxPulse);
  rightPulse = constrain(rightPulse, minPulse, maxPulse);

  escLeft.writeMicroseconds(leftPulse);
  escRight.writeMicroseconds(rightPulse);

  delay(20); // 50Hz servo timing
}