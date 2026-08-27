
//very important infooo
// while pressing the pushbutton push it goofor solid 0.5sec since the tinkercad has slow time/freuency 
//it is little slow in reading so press it till the state is changed to anchor drop
#include<Adafruit_LiquidCrystal.h>

//pin declaration for ultrasonic sensor 
const int trigpin = 9;
const int echopin = 10;
const int ldrpin = A0;


//pin declaration for blinker storm
const int ledpin = 6; 
unsigned long lastblinktime = 0;
bool ledstate = false;

//pin declaration for buzzer
const int buzzerpin = 8; 

//fixed val
const float vcc = 5.0;

//anchor button pin declaration
const int buttonpin = 7;
int lastbuttonstate = HIGH;   // HIGH means not pressed (with INPUT_PULLUP)
bool anchordropped = false;   // starts as "not dropped"

//as of now useless since tinker cad is auto starting brightness at 0
//for photo resistor but this variable will be used in reall life
String displayv;
float halfinitialbrightness;

Adafruit_LiquidCrystal lcd_1(0);

//timer charybdis
unsigned long charybdisStartTime = 0;
bool wasCharybdis = false; // only resets the timer once u switch disaster
//timer storm
unsigned long stormStartTime = 0;
bool wasstorm = false; // reset the timer for storm + variable to maintain the order on which disaster came

// function defined for light
float brightness() {
  int rawValue = analogRead(ldrpin);
  return rawValue;
}



//function defined for charybdis dist
float getDistance() {
  digitalWrite(trigpin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin, LOW);

  long duration = pulseIn(echopin, HIGH, 20000); // this might give error if u pull the object farther than 300 cm cause 
  if (duration == 0) return 999; // no echo = treat as far away, not "0cm"
  float distanceCm = duration * 0.0343 / 2; //cause there is a timeout for pulse in at 20000 (to make the code even faster)

  return distanceCm;
}



//function defined for anchor button state check
bool checkButtonPressed() {
  int currentState = digitalRead(buttonpin);
  bool justPressed = (currentState == LOW && lastbuttonstate == HIGH);
  lastbuttonstate = currentState;
  return justPressed;
}


void setup() {
  Serial.begin(9600);
  pinMode(trigpin, OUTPUT);// ultrasonic sensor 
  pinMode(echopin, INPUT);
  pinMode(buttonpin, INPUT_PULLUP);//anchor button
  pinMode(ledpin, OUTPUT);// storm light
  pinMode(buzzerpin, OUTPUT);//buzzer pin
  lcd_1.begin(16, 2);

//rightnow nor used but usable in real life simulations
  halfinitialbrightness = brightness() / 2;
  displayv = "open sea";
}



void loop() {
  float distv = getDistance(); // getting all the raw info before the loop starts
  float lightv = brightness();


//the  master key-- anchor button checker 
  if (checkButtonPressed()) {
    anchordropped = !anchordropped;
  }
//nothing works if anchor dropped prpgram directly ends this loop
  if (anchordropped) {
    displayv = "anchor dropped";
    lcd_1.setCursor(0, 1);
    lcd_1.clear();
    lcd_1.print(displayv);
    Serial.println(displayv);

    wasstorm = false; // resets the timer for storm 
    wasCharybdis = false; // reset the timer for charybdis

    digitalWrite(ledpin, LOW);// stops the blinker 
    digitalWrite(buzzerpin, LOW); // not in charybdis, buzzer off
    return; // restarts the loop ,basically ends the code here 
  }




  else {

    //CHARYBDIS
    if (distv < 100 && wasstorm == false) {
      
      if (!wasCharybdis) {
        charybdisStartTime = millis();
        wasCharybdis = true;
      }
      // 5 sec wrecked -- basically if the disatser time extends above 5 sec 
      // it will enter this infinite loop never to escape until simulation restarted
       if (millis() - charybdisStartTime >= 5000) {
        lcd_1.setCursor(0, 1);
        lcd_1.clear();
        lcd_1.print("wrecked");
        while (true) {
          // trapped here forever — charybdis wrecked the ship
        }
       }
      //charybdis timer
    unsigned long elapsedSeconds = (millis() - charybdisStartTime) / 1000;
    displayv = "charybdis " + String(elapsedSeconds) + "s";
    
      digitalWrite(buzzerpin, HIGH); // buzzer sounds while near charybdis




    }
    //STORM
    else if (lightv < 512) {
      
      displayv = "storm";
      wasCharybdis = false;

      digitalWrite(buzzerpin, LOW); // not in charybdis, buzzer off

       if (!wasstorm) {
        stormStartTime = millis();
        wasstorm = true;
      }
     //5 sec disaster rule- wrecked - same as above 
     // if disatster persist more than 5 sec code  will run into a infinite loop to never escape again
     // until simulation restarted
      if (millis() - stormStartTime >= 5000) {
        lcd_1.setCursor(0, 1);
        lcd_1.clear();
        lcd_1.print("wrecked");
        while (true) {
          // trapped here forever — storm wrecked the ship
        }
      }
      // timer for storm printer
      unsigned long stormElapsed = (millis() - stormStartTime) / 1000;
      displayv = "storm " + String(stormElapsed) + "s";


       // LED blink , without blocking the entire code 
  if (millis() - lastblinktime >= 150) { // toggle every 300ms
    ledstate = !ledstate;
    digitalWrite(ledpin, ledstate);
    lastblinktime = millis();
  }

    }
    else {
      // OPEN SEA 
      displayv = "opensea";
      wasCharybdis = false; //reseting the charybdis timer 
      wasstorm = false; // resets the storm timer + also helps to maintain the disaster of order 

      digitalWrite(ledpin, LOW); //shuts of the blinker
      digitalWrite(buzzerpin, LOW); // not in charybdis, buzzer off
    }



    
// final printer 

    lcd_1.setCursor(0, 1);
    lcd_1.clear();
    lcd_1.print(displayv);
    Serial.println(displayv);
  }
}