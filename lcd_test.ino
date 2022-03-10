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


void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(light, OUTPUT);
}

void loop() {
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  time_taken = pulseIn(echo, HIGH);
  distance_left = time_taken * 340 / 20000;

  Serial.println(distance_left);

  if (reads == len) {
    reads = 0;

    float absolute_distance = discard_outliers(distances, len);
    Serial.println("abs dist ----->  ");
    Serial.print(absolute_distance);


    if (absolute_distance <= 5.0 && override == 101) {
      digitalWrite(light, HIGH);
      delay(4000);
      digitalWrite(light, LOW);
      override = 102;
    }
    else if (absolute_distance <= 10.0 && absolute_distance > 5 && override == 0 || override == 102) {
      digitalWrite(light, HIGH);
      delay(4000);
      digitalWrite(light, LOW);
      override = 101;
    }
    else if (absolute_distance >= 15.0 && absolute_distance <= 25.0 && digitalRead(light) == HIGH) {
      digitalWrite(light, LOW);
      override = 0;
    }
    else if (absolute_distance >= 15.0 && absolute_distance <= 25.0 && digitalRead(light) == LOW) {
      delay(60 * 60 * 1000);
    }
    else if (absolute_distance >= 65.0 && absolute_distance <= 250.0 && digitalRead(light) == LOW ) {
      digitalWrite(light, HIGH);
      override = 0;
    }
    else if (absolute_distance >= 65.0 && absolute_distance <= 250.0 && digitalRead(light) == HIGH ) {
      delay(5 * 60 * 1000);
    }

  } else {
    distances[reads] = distance_left;
  }

  reads++;

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
