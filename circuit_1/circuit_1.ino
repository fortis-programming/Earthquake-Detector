#include<LiquidCrystal.h>
#include <SoftwareSerial.h>
SoftwareSerial SIM900L(10,11); // GSM MODULE
LiquidCrystal lcd(7,6,5,4,3,2);

#define buzzer 12
#define led 13

#define x A0
#define y A1
#define z A2

int xsample=0;
int ysample=0;
int zsample=0;

#define samples 50
#define maxVal 20
#define minVal -20
#define buzTime 5000

void setup()
{
  lcd.begin(16,2);
  Serial.begin(9600);
  SIM900L.begin(115200); // GSM SETUP
  delay(1000);
  lcd.print("EarthQuake ");
  lcd.print("Detector    ");
  delay(2000);
  lcd.clear();
  lcd.print("Calibrating....");
  lcd.setCursor(0,1);
  lcd.print("Please wait...");
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);

  alert();
  
  for(int i=0;i<samples;i++)
  {
    xsample+=analogRead(x);
    ysample+=analogRead(y);
    zsample+=analogRead(z);
  }

  xsample/=samples;
  ysample/=samples;
  zsample/=samples;

  delay(3000);
  lcd.clear();
  lcd.print("Calibrated");
  delay(1000);
  lcd.clear();
  lcd.print("Device Ready");
  delay(1000);
  lcd.clear();
  lcd.print(" X     Y     Z     ");
}

void loop()
{
  float value1=analogRead(x);
  float value2=analogRead(y);
  float value3=analogRead(z);

  float xValue=xsample-value1;
  float yValue=ysample-value2;
  float zValue=zsample-value3;

  lcd.setCursor(0,1);
  lcd.print(xValue);
  lcd.setCursor(6,1);
  lcd.print(yValue);
  lcd.setCursor(12,1);
  lcd.print(zValue);
  delay(1000);

//  Serial.println(xValue);
//  Serial.println(sqrt(analogRead(x) * analogRead(x) + analogRead(y) * analogRead(y)));
//  Serial.println(xValue);
  detectIntensity(sqrt(xValue * xValue + yValue * yValue + zValue * zValue)/32);
  GsmStatus();
}

void detectIntensity(float convertedValue){
  Serial.println(convertedValue);
  if (convertedValue < 2.5 && convertedValue > 0.8){
    alert();
    sendMessage("I", true);  
  } else if (convertedValue < 8 && convertedValue > 2.5){
    alert();
    sendMessage("II", true);
  } else if (convertedValue < 25 && convertedValue > 8){
    alert();
    sendMessage("III", true);
  } else if (convertedValue < 80 && convertedValue > 25){
    alert();
    sendMessage("IV", true);
  } else if (convertedValue < 250 && convertedValue > 80){
    alert();
    sendMessage("V", true);
  } else if (convertedValue < 400 && convertedValue > 250){
    alert();
    sendMessage("VI", true);
  } else if (convertedValue > 400){
    alert();
    sendMessage("VII", true);
  }
}

void GsmStatus(){
 if (SIM900L.available()>0)
   Serial.write(SIM900L.read());
}

void sendMessage(String intensity, bool during){
  Serial.println ("Intensity " + intensity);
  Serial.println ("Sending Message please wait....");
  SIM900L.println("AT+CMGF=1");
  delay(1000);
  SIM900L.println("AT+CMGS=\"+639216286417\"\r");
  delay(3000);
  
  if(during) SIM900L.println("WARNING - Earthquake alert! \n \nIntensity " + intensity + " \n \nExpect *light/heavy* shaking. Damages and aftershocks are expected. Duck, cover, and hold!"); 
  else SIM900L.println("After");
  
  delay(3000);
  SIM900L.println((char)26);
}

void alert(){
  digitalWrite(buzzer, 1);
  digitalWrite(led, 1);
  delay(2000);
  digitalWrite(buzzer, 0);
  digitalWrite(led, 0);
}
