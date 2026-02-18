unsigned long t1{}, t2{}, t3{}, t4{}; // таймер для каждого реле
unsigned long d1 = 250, d2 = 500, d3 = 1000, d4 = 2000; // задержка между поливом
unsigned long td1 = 1000, td2 = 2000, td3 = 3000, td4 = 4000; // время полива
#include "GyverButton.h" // библиотека для обработки нажатия кнопки
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
  Serial.begin(9600);
}

void loop() {
  if(millis()==0){ // правильный сброс таймера
    t1 = 0;
    t2 = 0;
    t3 = 0;
    t4 = 0;
    Serial.println("Сборс таймера!");
  }
  if(millis()-t1>=d1){
      digitalWrite(5,0); // переключение реле 1
    if(millis()-t1>=d1+td1){
      digitalWrite(5,1);
      t1 = millis();
    }
  }
  if(millis()-t2>=d2){ 
      digitalWrite(4,0); // переключение реле 2
    if(millis()-t2>=d2+td2){
      digitalWrite(4,1);
      t2 = millis();
    }
  }
  if(millis()-t3>=d3){
      digitalWrite(3,0); // переключение реле 3 
    if(millis()-t3>=d3+td3){
      digitalWrite(3,1);
      t3 = millis();
    }
  }
  if(millis()-t4>=d4){
      digitalWrite(2,0); // переключение реле 4
    if(millis()-t4>=d4+td4){
      digitalWrite(2,1);
      t4 = millis();
    }
  }
  
}
