 #include<LiquidCrystal.h>
#include <SoftwareSerial.h>
SoftwareSerial SIM900L(10,11);
LiquidCrystal lcd(7,6,5,4,3,2);

#define buzzer 12
#define led 13

#define x A0
#define y A1
#define z A2

long xsample=0;
long ysample=0;
long zsample=0;

#define samples 50
#define maxVal 20
#define minVal -20
#define buzTime 5000


long initial_x, initial_y;

void setup()
{
  lcd.begin(16,2);
  Serial.begin(9600);
  SIM900L.begin(115200);
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

//  alert();

  initial_x = analogRead(x);
  initial_y = analogRead(y);
  
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

bool shaking = false, earthquake_occur = false;
String intensity;
void loop()
{
  if(shaking) {
    Serial.println("Shaking detected \n Intensity "+ intensity);
//    alert();
  } else {
    Serial.println("Steady...");
    if(earthquake_occur) {
//      delay(5000); // will set sending message delay.
//      sendMessage();
    }
  }
  
  float value1=analogRead(x);
  float value2=analogRead(y);
  float value3=analogRead(z);

  float xValue=xsample-value1;
  float yValue=ysample-value2;
  float zValue=zsample-value3;

  int timeInMillis;
  float velocity_x, velocity_y, indicator = 45.352;
  /*
    0 & -1 is the intial state of detected by sensor.
  */ 
  indicator = computeVelocity(value1, initial_x);
  if(indicator < 0) indicator = indicator * -1;
  Serial.println(indicator, 5);
  if(indicator > 0.03) {
      shaking = true;
      earthquake_occur = true;
   
      velocity_x = computeVelocity(xValue, initial_x);
      detectIntensity(indicator, velocity_y);
  } else {
    shaking = false;
  }
  
  lcd.setCursor(0,1);
  lcd.print(xValue);
  lcd.setCursor(6,1);
  lcd.print(yValue);
  lcd.setCursor(12,1);
  lcd.print(zValue);
  delay(1000);
}

float computeVelocity(float n, float initial) {
  float computed_x;
  int timeInMillis;
  timeInMillis = 1000;
  computed_x = (n - initial) / 67.584;
  return computed_x * timeInMillis * 0.001;
}

void detectIntensity(float x, float y){
  if (x < 0.03) intensity = "IV";
  else if (x < 0.04) intensity = "V";
  else if (x < 0.92)  intensity = "VI";
  else if (x < 0.18 && x > 0.092) intensity = "VII";
  else if (x < 0.34 && x > 0.18) intensity = "VII"; 
  else if (x < 0.65 && x > 0.34) intensity = "IX";
  else if (x > 0.65) intensity = "X";
}

void GsmStatus(){
 if (SIM900L.available()>0)
   Serial.write(SIM900L.read());
}

void sendMessage() {
  Serial.println("Intensity " + intensity);
  SIM900L.println("AT+CMGF=1");
  delay(1000);
  SIM900L.println("AT+CMGS=\"+639216286417\"\r");
  delay(3000); 
  Serial.println(intensity);
  
  if(intensity == "V") SIM900L.println("WARNING - Shaking has stopped! \n \nA Intensity " + intensity + " earthquake has occurred. People are now advised to exit the building safely. Aftershocks are still expected."); 
  else if (intensity == "VI") SIM900L.println("WARNING - Shaking has stopped! \n \nA Intensity " + intensity + " earthquake has occurred. Aftershocks are still expected. People are advised to stay in their respective evacuation areas until further instructions. Stay alert!"); 
  
  delay(3000);
  SIM900L.println((char)26);
  delay(3000);
  Serial.print("Message sent");
  intensity = " ";
  earthquake_occur = false;
}

void alert(){
  digitalWrite(buzzer, 1);
  digitalWrite(led, 1);
  delay(2000);
  digitalWrite(buzzer, 0);
  digitalWrite(led, 0);
}
