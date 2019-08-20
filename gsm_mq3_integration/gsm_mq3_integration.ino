#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

//Define variables 

#define I2C_ADDR          0x27        //Define I2C Address where the PCF8574A is
#define BACKLIGHT_PIN      3
#define En_pin             2
#define Rw_pin             1
#define Rs_pin             0
#define D4_pin             4
#define D5_pin             5
#define D6_pin             6
#define D7_pin             7

//Initialise the LCD
LiquidCrystal_I2C      lcd(I2C_ADDR, En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

//Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(11, 10); //SIM800L Tx & Rx is connected to Arduino #11 & #10


int mq3_dout=2;
int mq3_aout=A0;
int led_pin=13;
int buzzer=12;  // Connect Buzzer to pin-8 
int ad_value;


void setup()
{  
  pinMode(led_pin,OUTPUT);
  pinMode(mq3_dout,INPUT);
  pinMode(mq3_aout,INPUT);
  pinMode(buzzer, OUTPUT);
  
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  
  //Define the LCD as 16 column by 2 rows
  lcd.begin(16, 2);
  
  //Switch on the backlight
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(HIGH);
  
  //goto first column (column 0) and first line (Line 0)
  lcd.setCursor(0,0);
  
  lcd.print("ALCOHOL DETECTOR!");
  delay(1000); 

  //Begin serial communication with Arduino and SIM800L
  mySerial.begin(9600);

  Serial.println("Initializing..."); 
  delay(1000);
}

void loop()
{
  lcd.clear();

  ad_value=analogRead(mq3_aout);
  if(digitalRead(mq3_dout)==LOW)
  {
    Serial.println("Drunk!!!!");
    lcd.print("Drunk!!!!");
    digitalWrite(buzzer, HIGH);
    
    mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
    updateSerial();
    sendMessage();
    updateSerial();
    mySerial.write(26);
  }
  else
  {
    Serial.println("Sober as a Judge");
    lcd.print("Sober as a Judge");
    digitalWrite(buzzer, LOW);
  
  }
  if(ad_value>500)
  {
    digitalWrite(led_pin,HIGH);
    delay(10);
    digitalWrite(led_pin,LOW);
    delay(10);
  }
  Serial.println(ad_value);
  lcd.setCursor(0, 1);
  
  lcd.print(ad_value/6000);
  
  delay(500);
}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}

void sendMessage()
{
  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CMGS=\"+254715455503\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
  updateSerial();
  //text content
  mySerial.print("Hello Abu, Alcohol has been detected in the driver's system. Here is his location: | https://www.google.com/maps/place/College+of+Physical+and+Biological+Sciences/@-1.2732378,36.8094171,15.89z/data=!4m5!3m4!1s0x182f1737396cf163:0x798d57fadab4c1c7!8m2!3d-1.2728643!4d36.8065408"); 
}
