#include <Wire.h>

#define LEFT_TRIG_PIN   11
#define LEFT_ECHO_PIN   12
#define RIGHT_TRIG_PIN  9
#define RIGHT_ECHO_PIN  10

const unsigned int MAX_DIST = 8; // inches
const unsigned int MIN_DIST = 2; // inches

void setup() {
  pinMode(LEFT_TRIG_PIN, OUTPUT); 
  pinMode(LEFT_ECHO_PIN, INPUT);
  pinMode(RIGHT_TRIG_PIN, OUTPUT); 
  pinMode(RIGHT_ECHO_PIN, INPUT);

  Serial.begin(9600);
}

void loop() {
  float leftDistance = measurePulse(LEFT_TRIG_PIN, LEFT_ECHO_PIN);
  float rightDistance = measurePulse(RIGHT_TRIG_PIN, RIGHT_ECHO_PIN);

  Serial.print("L: ");
  Serial.print(leftDistance);
  Serial.print("in, R: ");
  Serial.print(rightDistance);
  Serial.println("in");

  // no hands visible -- off
  if (leftDistance > MAX_DIST && rightDistance > MAX_DIST) {
    sendTd("BALL_OFF");
    delay(100);
    return; 
  }

  sendTd("BALL_ON");
  
  // only right hand -- fast forward
  if (leftDistance > MAX_DIST) {
    if (rightDistance < MIN_DIST) {
      sendTd("BALL_HOLD");
    } else {
      // fast forward
      sendTd(String("BALL_SPEED ") + mapSpeed(rightDistance));
    }
  } 
  
  // only left hand -- rewind
  else if (rightDistance > MAX_DIST) {
    if (leftDistance < MIN_DIST) {
      sendTd("BALL_HOLD");
    } else {
      // rewind
      sendTd(String("BALL_SPEED ") + mapSpeed(leftDistance * -1.0));
    }
  } 
  
  // both hands -- hold state or balance speeds
  else {
    if (rightDistance < MIN_DIST && leftDistance < MIN_DIST) {
      sendTd("BALL_HOLD");
    } else {
      long delta = rightDistance - leftDistance;
      sendTd(String("BALL_SPEED ") + mapSpeed(delta));
    }
  }

  delay(100); 
}

float measurePulse(pin_size_t trigPin, pin_size_t echoPin) {
  float echoTime;                  
  float calculatedDistance;   

  //send out an ultrasonic pulse that's 10ms long
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  echoTime = pulseIn(echoPin, HIGH);  

  calculatedDistance = echoTime / 148.0;
  return calculatedDistance; 
}

float mapSpeed(float dist) {
  if (dist < 0) {
    return map(dist, MAX_DIST * -1, MIN_DIST * -1, -50, -1);
  }
  return map(dist, MIN_DIST, MAX_DIST, 1, 50);
}

void sendTd(String msg) {
  Serial.print(msg);
  Serial.print('\n');
}
