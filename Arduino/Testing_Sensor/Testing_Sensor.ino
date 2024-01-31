#include <Wire.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
const int Trig = 2;
const int Echo = 4;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.clear();
  digitalWrite(Trig, LOW);
  delayMicroseconds(4);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);
  long t = pulseIn(Echo, HIGH);
  long cm = t / 29 / 2; // konversi
  String CM = " cm";
  Serial.println(cm + CM);
  lcd.setCursor(0, 0);
  lcd.print(cm + CM);
  delay(1000);
}
