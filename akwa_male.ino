/*

zegarek (DS3231) bez wyswietlacza (ew. kiedys jakis maly LCD)
sterowanie oswietleniem - 3 kanały: dzienne, roślinne, nocne (czasowe, ew. nocne z czujnikiem zbliżeniowym, może tez jakiś przycisk włączający niezależnie od pory dnia)
może kiedyś odczyt temperatury w akwarium i ew. sterowanie grzałką (przekaźnik)

  
*************************************  
I2C Scanning...
I2C device found at address 0x57  !
I2C device found at address 0x68  !
*/

#include <Wire.h>
//#include <time.h>
#include <DS3231.h>
//#include <dht.h>
 
DS3231 clock;
RTCDateTime dt;

// ************* piny wyświetlaczy ****************
int led_day = 5;     //pwm pin 0
int led_rb = 6;      //pwm pin 1
int led_night = 9;  //pwm pin 2
int heat = 11;       //digital pin 11

int dark_on = 1000;     //  1:00
int dark_off = 5000;    //  5:00
int day_on = 9000;      //  9:00
int rb_on = 10000;      // 10:00
int rb_off = 20000;     // 20:00
int day_off = 21000;    // 21:00
int mid_on = 13000;     // 13:00 - podział dnia - do obsługi jeszcze
int mid_off = 15000;    // 15:00

//int night_on = 21000;
//int night_off = 9000;

int szybkosc_swiatla = 4;
int pora = 0;

int day_lvl = 255;
int rb_lvl = 255;
int night_lvl = 255;

unsigned long dispTime;
int aktCzas = 0; // aktualny czas w formacie HHMM
int aktData = 0; // aktualny czas w formacie HHMM

int ix = 0;


void setup() {          
  pinMode(led_day, OUTPUT);
  pinMode(led_rb, OUTPUT);
  pinMode(led_night, OUTPUT);
  pinMode(heat, OUTPUT);
  
//  setTime(0,0,0,21,6,2018);
//  time_t dt = now();
  
  Serial.begin(115200);     //inicjalizacja monitora szeregowego
  clock.begin();            // Inicjalizacja DS3231
// następna linia do ustawienia zegarka, normalnie wyremowana
//  clock.setDateTime(__DATE__, __TIME__);
  dispTime=millis();
  
  _day_on();  //dyskoteka
  _day_off();
  _rb_on();
  _rb_off();
  _night_on();
  _night_off();
}

void loop() {
  
  dt = clock.getDateTime();
  aktCzas = 1000 * dt.hour + 10 * dt.minute + dt.second / 10; 
  Serial.println(aktCzas);
// ustalam day_lvl
  day_lvl = aktCzas - day_on;
  Serial.println(day_lvl);
  if (day_lvl < 0) {
    day_lvl = 0;
  }
  if (day_lvl > 255) {
    day_lvl = 255;
    if (aktCzas - day_off > 0) {
      day_lvl = 255 - (aktCzas - day_off);
      if (day_lvl < 0) {
        day_lvl = 0;
      }
    }
    if (aktCzas - mid_on > 0 && aktCzas - mid_off < 0) {  // przerwa dzienna w bialym swietle
      day_lvl = 50;
    }  
  }
  Serial.println(day_lvl);
  
// ustalam rb_lvl
  rb_lvl = aktCzas - rb_on;
  if (rb_lvl < 0) {
    rb_lvl = 0;
  }
  if (rb_lvl > 255) {
    rb_lvl = 255;
    if (aktCzas - rb_off > 0) {
      rb_lvl = 255 - (aktCzas - rb_off);
      if (rb_lvl < 0) {
        rb_lvl = 0;
      }
    }
    if (aktCzas - mid_on > 0 && aktCzas - mid_off < 0) {  // przerwa dzienna w rb swietle
      rb_lvl = 30;
    }  
  }

// night 

  night_lvl = 255 - (aktCzas - dark_on);
  if (night_lvl > 255) {
    night_lvl = 255;
  }
  if (night_lvl < 0) {
    night_lvl = 0;
  }
  if (aktCzas > dark_off) {
    night_lvl = aktCzas - dark_off;
    if (night_lvl > 255) {
      night_lvl = 255;
    }
    if (aktCzas > day_on) {
      night_lvl = 255 - (aktCzas - day_on);
      if (night_lvl < 0) {
        night_lvl = 0;
      }
      if (aktCzas > day_off) {
        night_lvl = aktCzas - day_off;
        if (night_lvl > 255) {
          night_lvl = 255;
        }
      }
    }
  } 
  
  
  Serial.println(day_lvl);
  Serial.println(rb_lvl);
  Serial.println(night_lvl);
  
  analogWrite(led_day, day_lvl);
  analogWrite(led_rb, rb_lvl);
  analogWrite(led_night, night_lvl);
  
  dispTime = millis();
  
  delay(1000);
} 

void _day_on(void)
{
  for (ix=0; ix<=255; ix++){  
    analogWrite(led_day, ix);
    Serial.print(".");
    delay(szybkosc_swiatla);
  }
  Serial.println();
  Serial.println("day on");
}

void _day_off(void)
{
  for (ix=255; ix>=0; ix--){  
    analogWrite(led_day, ix);
    Serial.print(".");
    delay(szybkosc_swiatla);
  }
  Serial.println();
  Serial.println("day off");
}
void _rb_on(void)
{
  for (ix=0; ix<=255; ix++){  
    analogWrite(led_rb, ix);
    Serial.print(".");
    delay(szybkosc_swiatla);
  }
  Serial.println();
  Serial.println("rb on");
}

void _rb_off(void)
{
  for (ix=255; ix>=0; ix--){  
    analogWrite(led_rb, ix);
    Serial.print(".");
    delay(szybkosc_swiatla);
  }
  Serial.println();
  Serial.println("rb off");
}
void _night_on(void)
{
  for (ix=0; ix<=255; ix++){  
    analogWrite(led_night, ix);
    Serial.print(".");
    delay(szybkosc_swiatla);
  }
  Serial.println();
  Serial.println("night on");
}

void _night_off(void)
{
  for (ix=255; ix>=0; ix--){  
    analogWrite(led_night, ix);
    Serial.print(".");
    delay(szybkosc_swiatla);
  }
  Serial.println();
  Serial.println("night off");
}

