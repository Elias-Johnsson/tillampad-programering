/*
* Name: clock and temp project
* Author: Elias Johnsson
* Date: 2024-10-10
* Description: This project uses a ds3231 to measure time and displays the time to an 1306 oled display,
* Further, it measures temprature with a analog temprature module and displays a mapped value to a 9g-servo-motor
*/

// Include Libraries
#include <RTClib.h>
#include <Wire.h>
#include "U8glib.h"
#include <Servo.h>
Servo myservo; 
// Init constants
const int x_cord = 30;
const int y_cord = 20;
const int tempPin = A1;
const int piezo_pin = 7;
const int fan_pin = 6;
const int button_pin = 5;
// Init global variables
RTC_DS3231 rtc;
char t[32];
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);
// construct objects


void setup() {
  // init communication
  u8g.setFont(u8g_font_unifont);
  Serial.begin(9600);
  myservo.attach(8);
  Wire.begin();
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));
  pinMode(fan_pin,OUTPUT);
  pinMode(button_pin,INPUT);
  for(int i = 9;i <=12;i++){
    pinMode(i,OUTPUT);
  }

  // Init Hardware
  pinMode(tempPin, INPUT);
  }


void loop() {
  oledWrite("Time:" + String(getTime()), "Temp:" + String(getTemp()));
  servoWrite(getTemp());
  warning();
}


/*
*This function reads time from an ds3231 module and package the time as a String
*Parameters: Void
*Returns: time in hh:mm:ss as String
*/
String getTime() {
  DateTime now = rtc.now();
  String time = String(now.hour()) + " " + String(now.minute()) + " " + String(now.second());
  return time;
}

/*
* This function reads an analog pin connected to an analog temprature sensor and calculates the corresponding temp
*Parameters: Void
*Returns: temprature as float
*/
float getTemp() {
  float temp = 0;
  float R1 = 10000;  // value of R1 on board
  float logR2, R2, T;
  float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741;  //steinhart-hart coeficients for thermistor

  int Vo = analogRead(tempPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);  //calculate resistance on thermistor
  logR2 = log(R2);
  temp = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));  // temperature in Kelvin
  temp = temp - 273.15;                                           //convert Kelvin to Celcius

  return temp;
}

/*
* This function takes a string and draws it to an oled display
*Parameters: - text: String to write to display
*Returns: void
*/
void oledWrite(String text, String text2){
  u8g.firstPage();
  do {
    u8g.drawStr(y_cord, x_cord, text.c_str());
    u8g.drawStr(y_cord, x_cord -20, text2.c_str());
  } while (u8g.nextPage());

}
/*
*This function flashes warning lights and makes a sound if the tempreture gets to high and starts to cool it with a fan
*Parameters: none
*returns: Void
*/
void warning(){
  if(getTemp() >= 28){
    for(int i=10;i<=12;i++){ // led pins är kopplade till digital pin 10-12
      digitalWrite(i,HIGH);
      delay(50);
      digitalWrite(i,LOW);
      tone(piezo_pin,1000,500);
    }
    digitalWrite(fan_pin, HIGH);
  }else{
    digitalWrite(fan_pin,LOW);
  }

}

/*
* takes a temprature value and maps it to corresppnding degree on a servo
*Parameters: - value: temprature
*Returns: void
*/
void servoWrite(float value) {
  int pos=(map(value, 0, 40, 0, 180));
  myservo.write(pos);
}