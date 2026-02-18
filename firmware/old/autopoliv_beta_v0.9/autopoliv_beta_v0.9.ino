#include <EEPROM.h> // библиотека для работы с энергонезависимой памятью
#include <Wire.h> //  подключаем библиотеку для работы с шиной I2C
#include <LiquidCrystal_I2C.h> //  подключаем библиотеку для работы с LCD дисплеем по шине I2C
#include "GyverButton.h" // библиотека для обработки нажатия кнопки
LiquidCrystal_I2C lcd(0x27,16,2); //  oбъявляем  объект библиотеки, указывая параметры дисплея (адрес I2C = 0x27, количество столбцов = 16, количество строк = 2)
int x, y; // положение джойстика
unsigned long k, t_for_j, t_for_d; // несколько вспомогательных переменных
int m, flagM, flag_forM;// несколько вспомогательных переменных
bool flag{1}, flagYorN, w; 
bool pause[4], checkP[4], pw[4]{1,1,1,1}, wtit[4]; // пауза
String nm[4]{"Plant 1","Plant 2","Plant 3","Plant 4"};
unsigned long t[4]; // таймер для каждого реле
unsigned long d[4]; // задержка между поливом
unsigned long td[4]; // время полива
unsigned long con[4]; 
double l[4]; // кол литров длякаждого растения
GButton butt(6);
//--------------------------------------------------------------------------------------//
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

  if(EEPROM.read(1023) != 2){
    EEPROM.write(1023,2);
    save(); // сохранение введённых данных в энергонезависимую память EPROM
    lcd.setCursor(5,0);
    lcd.print("saved");
    delay(1000);
  }
  else{
    bool i;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Change settings?");
    lcd.setCursor(0,1);
    if(i == 0) lcd.print("NO <");
    else lcd.print("YES<");
    while(flag){
      if(left()){
        i = !i;
        lcd.setCursor(0,1);
        if(i == 0) lcd.print("NO <");
        else lcd.print("YES<");
      }
      if(right()){
        i = !i;
        lcd.setCursor(0,1);
        if(i == 0) lcd.print("NO <");
        else lcd.print("YES<");
      }
      if(butt.isClick()){
        flag = 0;
        if(i == 1){
          save();
          lcd.setCursor(5,0);
          lcd.print("saved");
          delay(1000);
        }
        else{
          EEPROM.put(0,wtit);
          EEPROM.put(4,d);
          EEPROM.put(20,l);
        }
      }
    }
  }
  flag = 1;
  lcd.setCursor(5,0);
  lcd.print("     ");
  preparing(); // сброс таймеров
}
//--------------------------------------------------------------------------------------//
void loop() {
  resettick(); // когда таймер переполняется, он сбрасывается до 0, поэтому нужно сбросить таймеры
  pomptick(); // включение и выключение реле по времени
  menu(); // обработка меню на дисплее
}
//--------------------------------------------------------------------------------------//
void resettick(){
  if(millis() <= 25){ // правильный сброс таймера
    for(int i;i<4;i++){
      t[i] *= -1; 
    }
    t_for_j *= -1;
    t_for_d *= -1;
  }
  //Serial.print(d[0]+t[0]-millis()), Serial.print(" "), Serial.print(td[0]), Serial.print(" "), Serial.println(pause[0]); // отладка
}
void pomptick(){ // проходим по каждому растению
  for(int i{0};i < 4;i++){
    if(pause[i] == 1){ 
      t[i] = millis()-con[i];
    }
    if(pause[i] == 0) con[i] = millis()-t[i];
    if(millis()-t[i]>=d[i] && pause[i] == 0){
        powerDigit(i,0);
      if(millis()-t[i]>=d[i]+td[i]){
        powerDigit(i,1);
        t[i] = millis();
      }
    }
  }
}
//--------------------------------------------------------------------------------------//
void save(){
  for(int i{0};i<4;i++){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(nm[i]);
    lcd.setCursor(0,1);
    lcd.print("vlme| delay |wt?");
    k = 0;
    flag = 1;
    while(flag){
      switch (k){
        case 0:
          if(right()){
            if(l[i]<9.9){
              l[i] += 0.1;
              lcd.setCursor(0,1);
              lcd.print(l[i]);
              lcd.setCursor(3,1);
              lcd.print("<");
              break;
            }
          }
          if(left()){
            if(l[i]>=0.1){
              l[i] -= 0.1;
              lcd.setCursor(0,1);
              lcd.print(l[i]);
              lcd.setCursor(3,1);
              lcd.print("<");
              break;
            }
          }
          if(butt.isClick()){
            k+=1;
            lcd.setCursor(0,1);
            lcd.print(" ok ");
            break;
          }
          break;
        case 1:
          if(right()){
            if(d[i]<1008){
              d[i] += 24;
              lcd.setCursor(5,1);
              lcd.print("       ");
              lcd.setCursor(5,1);
              lcd.print(d[i]/24);
              lcd.print("d<");
              break;
            }
          }
          if(left()){
            if(d[i]>=24){
              d[i] -= 24;
              lcd.setCursor(5,1);
              lcd.print("       ");
              lcd.setCursor(5,1);
              lcd.print(d[i]/24);
              lcd.print("d<");
              break;
            }
          }
          if(butt.isClick()){
            k+=1;
            break;
          }
          break;
        case 2:
          if(right()){
            if(d[i]<1008){
              d[i] += 1;
              lcd.setCursor(5,1);
              lcd.print("       ");
              lcd.setCursor(5,1);
              lcd.print(d[i]/24);
              lcd.print("d");
              lcd.print(d[i]%24);
              lcd.print("h<");
              break;
            }
          }
          if(left()){
            if(d[i]>=1){
              d[i] -= 1;
              lcd.setCursor(5,1);
              lcd.print("       ");
              lcd.setCursor(5,1);
              lcd.print(d[i]/24);
              lcd.print("d");
              lcd.print(d[i]%24);
              lcd.print("h<");
              break;
            }
          }
          if(butt.isClick()){
            k+=1;
            lcd.setCursor(5,1);
            lcd.print("  ok  ");
            break;
          }
          break;
        case 3:
          if(right()){
            lcd.setCursor(14,0);
            lcd.print("v");
            flagYorN = !flagYorN;
            lcd.setCursor(13,1);
            if(flagYorN == 1){
              lcd.print("Yes");
              break;
            }
            else lcd.print("No ");
          }
          if(left()){
            lcd.setCursor(14,0);
            lcd.print("v");
            flagYorN = !flagYorN;
            lcd.setCursor(13,1);
            if(flagYorN == 1){
              lcd.print("Yes");
              break;
            }
            else lcd.print("No ");
          }
          if(butt.isClick()){
            wtit[i] = flagYorN;
            k+=1;
            flag = 0;
            lcd.setCursor(14,1);
            lcd.print("ok");
            break;
          }
          break;
      }
    }
  }
  lcd.clear();
}

void preparing(){
  for(int i;i<4;i++){
    d[i]*=3600000;
    td[i] = l[i]/1.2*3600000/60;
    t[i] = millis();
  }
  lcd.setCursor(0,0);
  lcd.print("       <        ");
  lcd.setCursor(0,1);
  lcd.print("                ");
}

void menu(){
  if(flag_forM == 0){
    if(up() && flagM>0){
      flagM--;
      if(flagM/2==0){
        lcd.setCursor(0,0);
        lcd.print(nm[0]);
        d_h_m(0,0);
        lcd.setCursor(0,1);
        lcd.print(nm[1]);
        d_h_m(1,1);
      }

      else{
        lcd.setCursor(0,0);
        lcd.print(nm[2]);
        d_h_m(2,0);
        lcd.setCursor(0,1);
        lcd.print(nm[3]);
        d_h_m(3,1);
      }
      lcd.setCursor(7,0);
      lcd.print(" ");
      lcd.setCursor(7,1);
      lcd.print(" ");
      lcd.setCursor(7,flagM%2);
      lcd.print("<");
    }

    if(down() && flagM<3){
      flagM++;
      if(flagM/2==0){
        lcd.setCursor(0,0);
        lcd.print(nm[0]);
        d_h_m(0,0);
        lcd.setCursor(0,1);
        lcd.print(nm[1]);
        d_h_m(1,1);
      }
      else{
        lcd.setCursor(0,0);
        lcd.print(nm[2]);
        d_h_m(2,0);
        lcd.setCursor(0,1);
        lcd.print(nm[3]);
        d_h_m(3,1);
      }
      lcd.setCursor(7,0);
      lcd.print(" ");
      lcd.setCursor(7,1);
      lcd.print(" ");
      lcd.setCursor(7,flagM%2);
      lcd.print("<");
    }

    if(millis()-t_for_d>=1000){
      if(flagM/2==0){
        lcd.setCursor(0,0);
        lcd.print(nm[0]);
        d_h_m(0,0);
        lcd.setCursor(0,1);
        lcd.print(nm[1]);
        d_h_m(1,1);
        lcd.setCursor(7,abs(flagM%2-1));
        lcd.print(" ");
        lcd.setCursor(7,flagM%2);
        lcd.print("<");
      }
      else{
        lcd.setCursor(0,0);
        lcd.print(nm[2]);
        d_h_m(2,0);
        lcd.setCursor(0,1);
        lcd.print(nm[3]);
        d_h_m(3,1);
        lcd.setCursor(7,abs(flagM%2-1));
        lcd.print(" ");
        lcd.setCursor(7,flagM%2);
        lcd.print("<");
      }
      t_for_d = millis();
    }
  }

  if(flag_forM == 1 && right()){
    powerDigit(flagM,w);
    powerDigit(flagM,2);
    powerDigit(flagM,w);
    if(pw[flagM] == 1) lcd.print("OFF");
    else lcd.print("ON ");
    w = !w;
  }

  if(flag_forM == 1 && butt.isClick()){
    pause[flagM] = !pause[flagM];
    powerDigit(flagM,1);
    powerDigit(flagM,2);
    d_h_m(flagM,0);
  }

  if(flag_forM == 1 && millis()-t_for_d>=1000){
    d_h_m(flagM,0);
    lcd.setCursor(13,1);
    if(pw[flagM] == 1) lcd.print("OFF");
    else lcd.print("ON ");
    t_for_d = millis();
  }

  if(flag_forM == 1 && left()){
    flag_forM = 0;
    if(flagM/2==0){
      lcd.setCursor(0,0);
      lcd.print(nm[0]);
      d_h_m(0,0);
      lcd.setCursor(0,1);
      lcd.print(nm[1]);
      d_h_m(1,1);
      lcd.setCursor(7,abs(flagM%2-1));
      lcd.print(" ");
      lcd.setCursor(7,flagM%2);
      lcd.print("<");
    }
    else{
      lcd.setCursor(0,0);
      lcd.print(nm[2]);
      d_h_m(2,0);
      lcd.setCursor(0,1);
      lcd.print(nm[3]);
      d_h_m(3,1);
      lcd.setCursor(7,abs(flagM%2-1));
      lcd.print(" ");
      lcd.setCursor(7,flagM%2);
      lcd.print("<");
    }
  }
  if(butt.isClick() && flag_forM == 0) {
    lcd.setCursor(0,0);
    lcd.print("                ");
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,0);
    lcd.print(nm[flagM]);
    lcd.setCursor(8,0);
    d_h_m(flagM,0);
    lcd.setCursor(0,1);
    lcd.print(l[flagM]);
    lcd.setCursor(3,1);
    lcd.print("L");
    lcd.setCursor(4,1);
    lcd.print("|");
    lcd.print(d[flagM]/3600000/24);
    lcd.print("d");
    lcd.print(d[flagM]/3600000%24);
    lcd.print("h");
    lcd.setCursor(10,1);
    lcd.print("|");
    lcd.print("P:");
    if(pw[flagM] == 1) lcd.print("OFF");
    else lcd.print("ON ");
    flag_forM = 1;
  }
}
//--------------------------------------------------------------------------------------//
bool right(){ // проверка положения джойстика
  int x = analogRead(A3);
  int y = analogRead(A2);
  if(y==0 && x>=400 && x<=600 && millis()-t_for_j>=250){
    Serial.println("right!");
    t_for_j = millis();
    return(true);
  }
  else return(false);
}
bool left(){ // проверка положения джойстика
  int x = analogRead(A2);
  int y = analogRead(A3);
  //Serial.println(x,y);
  if(x>=900 && y>=400 && y<=600 && millis()-t_for_j>=250){
    Serial.println("left!");
    t_for_j = millis();
    return(true);
  }
  else return(false);
}
bool up(){ // проверка положения джойстика
  int x = analogRead(A2);
  int y = analogRead(A3);
  //Serial.println(x,y);
  if(x>=400 && x<=600 && y==0 && millis()-t_for_j>=250){
    Serial.println("up!");
    t_for_j = millis();
    return(true);
  }
  else return(false);
}
bool down(){ // проверка положения джойстика
  int x = analogRead(A2);
  int y = analogRead(A3);
  //Serial.println(x,y);
  if(x>=400 && x<=600 & y>=900 && millis()-t_for_j>=250){
    Serial.println("down!");
    t_for_j = millis();
    return(true);
  }
  else return(false);
}
void d_h_m(int i, int m){
  lcd.setCursor(8,m);
  if(pause[i] == 1) lcd.print(" pause  ");
  else if(millis()-t[i]>=d[i]) lcd.print("watering");
  else if(millis()-t[i]<d[i]){
    lcd.print("00:00:00");
    if((d[i]+t[i]-millis())/3600000/24<=9){
      lcd.setCursor(9,m);
      lcd.print((d[i]+t[i]-millis())/3600000/24);
    }
    else{
      lcd.setCursor(8,m);
      lcd.print((d[i]+t[i]-millis())/3600000/24);
    }
    if((d[i]-millis()+t[i])/3600000%24<=9){
      lcd.setCursor(12,m);
      lcd.print((d[i]+t[i]-millis())/3600000%24);
    }
    else{
      lcd.setCursor(11,m);
      lcd.print((d[i]+t[i]-millis())/3600000%24);
    }
    if((d[i]-millis()+t[i])/60000%60<=9){
      lcd.setCursor(15,m);
      lcd.print((d[i]+t[i]-millis())/60000%60);
    }
    else{
      lcd.setCursor(14,m);
      lcd.print((d[i]+t[i]-millis())/60000%60);
    }
  }
}
//--------------------------------------------------------------------------------------//
void powerDigit(int i, int m){ 
  if(m == 2) checkP[i] = !checkP[i];
  else if(checkP[i] != 1){
    pw[i] = m; 
    digitalWrite(5-i,m);
  }
}