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

//char *arrayString[] = {"9216286417", "9201386387", "9984056306", "9669745640", "9162863111", "9056054246", "9084915409", "9166240508", "9994585533", "9989230570", "9304149708", "9451540860", "9274106855", "9517158009", "9777406205", "9159360532", "9667286011", "9983990006", "9516764492", "9636626940"};
void loop()
{
//  for(int i = 0; i < sizeof(arrayString)/sizeof(arrayString[0]); i++){
//    sendMessage(arrayString[i]);  
//  }
//  Serial.println(sizeof(arrayString)/sizeof(arrayString[0]));
  if(shaking) {
    Serial.println("Shaking detected \n Intensity "+ intensity);
//    alert();
  } else {
//    sizeof(arrayString)/sizeof(arrayString[0])
    Serial.println("Steady...");
    if(earthquake_occur) {
      delay(5000); // will set sending message delay.
      sendMessage();
      delay(50000);
      earthquake_occur = false;
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
  Serial.println("Value: "+ String(x));
  if (x <= 0.03) intensity = "IV";
  else if (x <= 0.04) intensity = "V";
  else if (x <= 0.09)  intensity = "VI";
  else if (x <= 0.18) intensity = "VII";
  else if (x <= 0.34) intensity = "VIII"; 
  else if (x <= 0.65) intensity = "IX";
  else if (x > 0.65) intensity = "X";
}

void GsmStatus(){
 if (SIM900L.available()>0)
   Serial.write(SIM900L.read());
}

// sizeof(arrayString)/sizeof(arrayString[0])
String personnel = "9216286417";
void sendMessage() {
  for(int i = 0; i < sizeof(arrayString)/sizeof(arrayString[0]); i++){
    SIM900L.println("AT+CMGF=1");
    delay(1000);
    SIM900L.println("AT+CMGS=\"+63"+String(arrayString[i])+"\"\r");
    delay(3000);
    SIM900L.println("TEST " + intensity);
//    SIM900L.println("WARNING - Shaking has stopped! \n \nAn intensity " + intensity + " earthquake has occurred. People are now advised to exit the building safely. Aftershocks are still expected.");
//    if(intensity == "V") SIM900L.println("WARNING - Shaking has stopped! \n \nA Intensity " + intensity + " earthquake has occurred. People are now advised to exit the building safely. Aftershocks are still expected."); 
//    else if (intensity == "VI") SIM900L.println("WARNING - Shaking has stopped! \n \nA Intensity " + intensity + " earthquake has occurred. Aftershocks are still expected. People are advised to exit the building safely. Aftershocks are still expected"); 
//    else if(intensity == "VII" || intensity == "VIII" || intensity == "XI" || intensity == "X") SIM900L.println("WARNING - Shaking has stopped! \n \nA Intensity " + intensity + " earthquake has occurred. Aftershocks are still expected. People are advised to stay in their respective evacuation areas until further instructions. Stay alert!"); 
    delay(3000);
    SIM900L.println((char)26);
    delay(3000);
    Serial.println("Message sent");
  }
}

void alert(){
  digitalWrite(buzzer, 1);
  digitalWrite(led, 1);
  delay(2000);
  digitalWrite(buzzer, 0);
  digitalWrite(led, 0);
}
