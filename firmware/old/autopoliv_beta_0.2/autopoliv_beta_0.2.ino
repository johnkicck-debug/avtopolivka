#include <Wire.h> //  подключаем библиотеку для работы с шиной I2C
#include <LiquidCrystal_I2C.h> //  подключаем библиотеку для работы с LCD дисплеем по шине I2C
#include "GyverButton.h" // библиотека для обработки нажатия кнопки
LiquidCrystal_I2C lcd(0x27,16,2); //  oбъявляем  объект библиотеки, указывая параметры дисплея (адрес I2C = 0x27, количество столбцов = 16, количество строк = 2)
int x, y; // положение джойстика
bool flag{1};
String nm[4]{"Plant 1","Plant 2","Plant 3","Plant 4"};
unsigned long t[4]{}; // таймер для каждого реле
unsigned long d[4]{}; // задержка между поливом
unsigned long td[4]{}; // время полива
GButton butt(6);
void setup() {
  // put your setup code here, to run once:
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  digitalWrite(2,1);
  digitalWrite(3,1);
  digitalWrite(4,1);
  digitalWrite(5,1);
  butt.setTickMode(AUTO);
  lcd.init();
  lcd.backlight();// включаем подсветку LCD дисплея
  lcd.setCursor(5,0);
  Serial.begin(9600);
  lcd.print("Hello!");
  delay(2000);
  save();
  
}
void loop() {
  resettick();
  pomptick();
  
}
void resettick(){
  if(millis() == 0){ // правильный сброс таймера
    t[1] *= -1;
    t[2] *= -1;
    t[3] *= -1;
    t[4] *= -1;
    Serial.println("Сборс таймера!");
  }
}
void pomptick(){
  if(millis()-t[0]>=d[0]){
      digitalWrite(5,0); // переключение реле 1
    if(millis()-t[0]>=d[0]+td[0]){
      digitalWrite(5,1);
      t[0] = millis();
    }
  }
  if(millis()-t[1]>=d[1]){
      digitalWrite(4,0); // переключение реле 2
    if(millis()-t[1]>=d[1]+td[1]){
      digitalWrite(4,1);
      t[1] = millis();
    }
  }
 if(millis()-t[2]>=d[2]){
      digitalWrite(4,0); // переключение реле 2
    if(millis()-t[2]>=d[2]+td[2]){
      digitalWrite(4,1);
      t[2] = millis();
    }
  }
  if(millis()-t[3]>=d[3]){
      digitalWrite(3,0); // переключение реле 3
    if(millis()-t[3]>=d[3]+td[3]){
      digitalWrite(3,1);
      t[3] = millis();
    }
  }
}
void save(){
  for(int i{0};i<4;i++){
    lcd.setCursor(0,0);
    lcd.print("                "); // очищение экрана
    lcd.setCursor(0,1);
    lcd.print("                "); // очищение экрана
    lcd.setCursor(0,0);
    lcd.print(nm[i]);
    lcd.setCursor(0,1);
    lcd.print("vlme|");
    lcd.setCursor(5,1);
    lcd.print("delay|");
    lcd.setCursor(11,1);
    lcd.print("wtit?");
    while(flag){
      right();
    }
  }
}
void right(){
  x = analogRead(A3);
  y = analogRead(A2);
  Serial.println(x,y);

}