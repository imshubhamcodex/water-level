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
float static_distance[30]; // to auto on motor
int static_distance_len = 30;
int static_distance_index = 0;

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  pinMode(trigger,OUTPUT);
  pinMode(echo,INPUT);
  pinMode(light,OUTPUT);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Made by");
  lcd.setCursor(0,1);
  lcd.print("Shubham");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Boost by");
  lcd.setCursor(0,1);
  lcd.print("Monu");
  delay(2000);
}

void loop() {
  // Activating sonar
  digitalWrite(trigger,LOW);
  delayMicroseconds(2);
  digitalWrite(trigger,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger,LOW);
  delayMicroseconds(2);
  time_taken = pulseIn(echo,HIGH);
  distance_left = time_taken*340/20000;

  // A distance measured so:
  reads++;

  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("Reading");
  lcd.setCursor(0, 1);
  lcd.print(distance_left);
  lcd.print("cm");

  
  if(reads == len){
    // If 7 read are taken
    reads = 0; // Reset reads
    
    float absolute_distance = discard_outliers(distances,len); // Value after discarding outliers
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LEVEL =>");
    lcd.setCursor(0, 1);
    lcd.print(absolute_distance);
    lcd.print("cm");

    if(absolute_distance <= 5.0){
      // Self defence
      if(override <= 2){
          disable_manual_override();
          override++;
       }
    }
    else if(absolute_distance > 5.0 && absolute_distance <= 11.0){
      // Self defence
       if(override==0){
          disable_manual_override();
          override++;
       } 
       
    }else if(absolute_distance <= 20.0){
      // Moter OFF
      digitalWrite(light,LOW);
      
    }else if(absolute_distance >= 55.0){
      // Motor ON
      override = 0;
      digitalWrite(light,HIGH);

      Serial.println("absolute_distance = >");
      Serial.println(absolute_distance);

      //Auto ON
      if(static_distance_index < static_distance_len){
        static_distance[static_distance_index] = absolute_distance;
        static_distance_index++;
      }

      if(static_distance_index == static_distance_len){
        boolean auto_on = sorting_and_trigger(static_distance,static_distance_len);
        
        if(auto_on){
          digitalWrite(light,LOW);
          delay(3000);
          digitalWrite(light,HIGH);
         }

          static_distance_index = 0;
         
      }
      
    }
    
  }else{
    // If 5 read are not taken
    distances[reads-1] = distance_left;
  }

  delay(2000);
  
}

float discard_outliers(float distances[],int len){
  // Insertion Sort
   for (int i = 1; i < len; ++i){
    float j = distances[i];
    int k;
    for (k = i - 1; (k >= 0) && (j < distances[k]); k--){
      distances[k + 1] = distances[k];
    }
    distances[k + 1] = j;
  }

  // Returning middle value
  if(len%2 == 0){
    return distances[len/2 -1];
  }else{
    return distances[((len+1)/2)-1];
  }
}

boolean sorting_and_trigger(float static_distance[],int static_distance_len ){

  // Insertion Sort
   for (int i = 1; i < static_distance_len; ++i){
    float j = static_distance[i];
    int k;
    for (k = i - 1; (k >= 0) && (j < static_distance[k]); k--){
      static_distance[k + 1] = static_distance[k];
    }
    static_distance[k + 1] = j;
  }

  int count = 0;
  float middle_static_distances[10];

  while(count < 10){
    middle_static_distances[count] = static_distance[count+9];
    count++;
  }

  float val1 = middle_static_distances[0];
  for(int i = 1; i < 10; i++){
    float val2 = middle_static_distances[i];
    float diff = val2 - val1;
    val1 = val2;

    if(diff < 3.0){
      Serial.println("diff => ");
      Serial.println(diff);
      return true;
    }
  }

  return false;
  
}


void disable_manual_override(){
  // Turning ON light
  digitalWrite(light,HIGH);
  delay(4000);
  digitalWrite(light,LOW);
  // Turning OFF light
}
