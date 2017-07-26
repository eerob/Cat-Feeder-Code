#include "Arduino.h"
#include <Wire.h>  // Comes with Arduino IDE
//#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal_I2C.h>
#include <Time.h>
#include <TimeLib.h>

// set the LCD address to 0x27 for a 20 chars 4 line display
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol

// Declare Feeding times ---------------------------------
int f1h = 1;  // feeding time 1 hours
int f1m = 1;  // feeding time 1 minutes
LiquidCrystal_I2C lcd(0x27,20,4);   // Set the LCD I2C address A4 (SDA), A5 (SCL)
//#include <Time.h>  
#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

int button = 6;

int feedSetting = 1;

int pinI1=8;//define I1 interface
int pinI2=11;//define I2 interface 
int pinI3=12;//define I3 interface 
int pinI4=13;//define I4 interface 
int speedpinB=10;//enable motor B
int spead2 =255;//define the spead of motor 2

int f1s = 1;  // feeding time 1 seconds
int f2h = 9;  // feeding time 1 hours
int f2m = 1;  // feeding time 1 minutes
int f2s = 1;  // feeding time 1 seconds
int f3h = 16;  // feeding time 1 hours
int f3m = 1;  // feeding time 1 minutes
int f3s = 1;  // feeding time 1 seconds
unsigned long clearTime = 0;  // used to know when to clear the LCD
unsigned long curTime;
unsigned long startTime;
unsigned long startTime1;
unsigned long systemStartTime;

bool error = 1;
bool errLight = 0;
bool timeoutSetTime = 0;

void setup() {
  pinMode(pinI1,OUTPUT);
  pinMode(pinI2,OUTPUT);
  pinMode(pinI3,OUTPUT);
  pinMode(pinI4,OUTPUT);
  pinMode(speedpinB,OUTPUT);
  
  digitalWrite(button, HIGH); // turn on pull up resistor
  
  lcd.init();         // initialize the lcd for 20 chars 4 lines, turn on backlight

// ------- Quick 3 blinks of backlight  -------------
  blinkScreen(); 
  lcd.setCursor(0,1);
  lcd.print("   CROOKSHANKS V4");
  lcd.setCursor(0,2);
  lcd.print("   MEOW MEOW!");
  delay(1000);
  
  Serial.begin(9600);
  while (!Serial); // Needed for Leonardo only
  pinMode(13, OUTPUT);
  setSyncProvider(requestSync);  //set function to call when sync required
  lcd.clear();
  lcd.print("Waiting for sync msg");
 
  delay(500);

  systemStartTime = millis();
  startTime1 = millis();
  
}
//------------------------------------------------------------------------------
void loop () {
  if (millis() - systemStartTime > 30000 && timeoutSetTime == 0) {
    timeoutSetTime = 1;
    setTime(1450742400); 
  }
  
  if (Serial.available()) {
    timeoutSetTime = 1;
    processSyncMessage();
  }

  if (timeStatus()!= timeNotSet) {
    digitalClockDisplay(feedSetting);  
  }
  if (timeStatus() == 0) {
    blinkScreen();  // time not set blink LED
    } 
  else {  
    digitalWrite(13, HIGH); // LED on if synced
  }
  //Serial.println("d6");
  //Serial.println(digitalRead(button));
  
  if(digitalRead(button) == 0) {
    startTime = millis();
    
    while(1) {
      if (digitalRead(button) == 0){
        curTime = millis();
      }

      else {

        if (curTime - startTime > 5000) {
          jog();
          break;
        }
        
        else if (curTime - startTime > 1000) {
          if(feedSetting == 2)
            feedSetting = 0;
          else
            feedSetting = feedSetting + 1;
          break;
        }
        else
          break;
      }
    }
  }

  
  // if 5 minutes has elapsed since the last time the LCD was intialized then intialize it
  if(millis() - startTime1 > 300000) {
    lcd.init();
    startTime1 = millis();
  }
    
  
  // If time is equal to feeding time and feedsetting = 0 then feed the damn cat once per day
  if ((hour() == f1h && minute() == f1m && second() == f1s && feedSetting == 0))
  {
    // ------- Quick 3 blinks of backlight  -------------
    for(int i = 0; i< 3; i++)
    {
      blinkScreen();
    }
    
    // Run Cat feeder
    jog();
    
  }

    // If time is equal to feeding time and feedsetting = 1 then feed the damn cat twice per day
  if ((hour() == f1h && minute() == f1m && second() == f1s && feedSetting == 1) || (hour() == f2h && minute() == f2m && second() == f2s && feedSetting == 1))
  {
    // ------- Quick 3 blinks of backlight  -------------
    for(int i = 0; i< 3; i++)
    {
      blinkScreen();
    }
    
    // Run Cat feeder
    jog();
    
  }

  // If time is equal to feeding time and feedsetting = 2 then feed the damn cat three times per day
  if ((hour() == f1h && minute() == f1m && second() == f1s && feedSetting == 2) || (hour() == f2h && minute() == f2m && second() == f2s && feedSetting == 2)|| (hour() == f3h && minute() == f3m && second() == f3s && feedSetting == 2))
  {
    // ------- Quick 3 blinks of backlight  -------------
    for(int i = 0; i< 3; i++)
    {
      blinkScreen();
    }
    
    // Run Cat feeder
    jog();
    
  }
  
  //delay(250);
}

//------------------------------------------------------------------------------------
void digitalClockDisplay(bool setting) {
  // digital clock display of the time
  //delay(10);
  if (millis() - clearTime > 1000) {
    lcd.clear();
    clearTime = millis();
  }
  
  if (feedSetting == 0) {
    lcd.setCursor(0,0);
    lcd.print("Time: ");
    lcd.print(hour());
    lcd.print(":");
    lcd.print(minute());
    lcd.print(":");
    lcd.print(second()); 
    
    lcd.setCursor(0,1);
    lcd.print("Feed time 1 ");
    lcd.print(f1h);
    lcd.print(":");
    lcd.print(f1m);
    lcd.print(":");
    lcd.print(f1s); 
  }

  if (feedSetting == 1) {
    lcd.setCursor(0,0);
    lcd.print("Time: ");
    lcd.print(hour());
    lcd.print(":");
    lcd.print(minute());
    lcd.print(":");
    lcd.print(second()); 
    
    lcd.setCursor(0,1);
    lcd.print("Feed time 1 ");
    lcd.print(f1h);
    lcd.print(":");
    lcd.print(f1m);
    lcd.print(":");
    lcd.print(f1s); 
    
    lcd.setCursor(0,2);
    lcd.print("Feed time 2 ");
    lcd.print(f2h);
    lcd.print(":");
    lcd.print(f2m);
    lcd.print(":");
    lcd.print(f2s); 
    }
    if (feedSetting == 2) {
    lcd.setCursor(0,0);
    lcd.print("Time: ");
    lcd.print(hour());
    lcd.print(":");
    lcd.print(minute());
    lcd.print(":");
    lcd.print(second()); 
    
    lcd.setCursor(0,1);
    lcd.print("Feed time 1 ");
    lcd.print(f1h);
    lcd.print(":");
    lcd.print(f1m);
    lcd.print(":");
    lcd.print(f1s); 
    
    lcd.setCursor(0,2);
    lcd.print("Feed time 2 ");
    lcd.print(f2h);
    lcd.print(":");
    lcd.print(f2m);
    lcd.print(":");
    lcd.print(f2s); 

    lcd.setCursor(0,3);
    lcd.print("Feed time 3 ");
    lcd.print(f3h);
    lcd.print(":");
    lcd.print(f3m);
    lcd.print(":");
    lcd.print(f3s);
    }
}
//--------------------------------------------------------------------------------
void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}


void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     if( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
       setTime(pctime); // Sync Arduino clock to the time received on the serial port
     }
  }
}

time_t requestSync()
{
  Serial.write(TIME_REQUEST);  
  return 0; // the time will be sent later in response to serial mesg
}

//------------------------------------------------------------------------------

void forward()
{
     analogWrite(speedpinB,spead2);
     digitalWrite(pinI4,HIGH);//turn DC Motor B move clockwise
     digitalWrite(pinI3,LOW);
}

void backward()
{
     analogWrite(speedpinB,spead2);
     digitalWrite(pinI4,LOW);//turn DC Motor B move clockwise
     digitalWrite(pinI3,HIGH);
}

void stop()
{    // Unenble the pin, to stop the motor. this should be done to avid damaging the motor. 
     digitalWrite(speedpinB,LOW);
     delay(1000);
}

void jog() {
    
    startTime = millis();
    // go forward
    while(millis() - startTime < 1500) {
      forward();
    }
    stop();
}

void blinkScreen() {
  lcd.backlight();
  delay(250);
  lcd.noBacklight();
  delay(250);
  lcd.backlight();
  delay(250);
}


