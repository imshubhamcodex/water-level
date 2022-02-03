#include <LiquidCrystal.h>

// Create an LCD object. Parms: (RS, E, D4, D5, D6, D7):
LiquidCrystal lcd = LiquidCrystal(2, 3, 4, 5, 6, 7);

// Ultrasonic sensor pin declaration:
int trigger = 10;
int echo = 11;

// Light pin declaration:
int light = 8;

// Distance calculation params declaration:
float time_taken = 0;
float distance_left = 0;
int reads = 0; // Numbers of distance captured
int len = 7; // Array length
float distances[7];
int override = 0;
boolean motor_off = false;


void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(light, OUTPUT);
}

void loop() {
  // Activating sonar
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  time_taken = pulseIn(echo, HIGH);
  distance_left = time_taken * 340 / 20000;

  // A distance measured so:
  reads++;

  if (reads == len) {
    reads = 0; // Reset reads

    float absolute_distance = discard_outliers(distances, len); // Value after discarding outliers

    Serial.println(absolute_distance);


    if (absolute_distance <= 7.0) {
      // Self defence
      if (override <= 1) {
        // Turning ON light
        digitalWrite(light, HIGH);
        delay(4000);
        digitalWrite(light, LOW);
        // Turning OFF light
        override++;
      } else {
        digitalWrite(light, LOW);
      }
    }
    else if (absolute_distance >= 16.0 && absolute_distance <= 26.0) {
      // OFF
      if (override == 0) {
        // Turning ON light
        digitalWrite(light, HIGH);
        delay(4000);
        digitalWrite(light, LOW);
        // Turning OFF light
        override++;
      } else {
        delay(2 * 60 * 1000);
      }

    }
    else if (absolute_distance >= 27.0 && absolute_distance < 36.0) {
      delay(2 * 60 * 1000);
    }
    else if (absolute_distance >= 36.0 && absolute_distance <= 65.0) {
      delay(6 * 60 * 1000);
    }
    else if (absolute_distance >= 75.0) {
      // Motor ON
      override = 0;

      if (absolute_distance >= 1000) {
        digitalWrite(light, LOW);
      } else {

        digitalWrite(light, HIGH);

        Serial.println("absolute_distance = >");
        Serial.println(absolute_distance);
        delay(2 * 60 * 1000);
      }

    }

  } else {
    // If 7 read are not taken
    distances[reads - 1] = distance_left;
  }

  delay(1000);

}

float discard_outliers(float distances[], int len) {
  // Insertion Sort
  for (int i = 1; i < len; ++i) {
    float j = distances[i];
    int k;
    for (k = i - 1; (k >= 0) && (j < distances[k]); k--) {
      distances[k + 1] = distances[k];
    }
    distances[k + 1] = j;
  }

  // Returning middle value
  if (len % 2 == 0) {
    return distances[len / 2 - 1];
  } else {
    return distances[((len + 1) / 2) - 1];
  }
}
