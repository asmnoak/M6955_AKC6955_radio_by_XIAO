/*
  DSP Radio M6955(AKC6955) control program on Arduino IDE
      This sketch is specializing in xiao.
   Created on: Dec. 25 , 2019
       Author: asmnoak
       support rotary encoder Rev. 2021/4/20
       support OLED    2020/1/12
       support new SW band 2022/2/4
*/
#include <Rotary.h>
//#include "U8glib.h"
#include "U8g2lib.h"
#include <Wire.h>
//U8GLIB_SSD1306_128X32 u8g(U8G_I2C_OPT_NONE);  // I2C / TWI OLED
//U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
#define RADIO 0x10    // i2c address
int read_byte;
float freq;  // frequency
int channel_num, initial_num;
char s_upper2, s_lower2; // freq register
int terminal_1  = 7;  // digital pin no. of rotary encoder
int terminal_2  = 6;  // digital pin no. of rotary encoder
volatile int encorder_val  = 0;
Rotary rot = Rotary(6,7);
//
volatile long value = 0;
//
int mode_set; // selected band
int mode;     // FM/AM
int hoge;     // temp value
int station = 0; // preset value
int ct, pt, ct2, pt2, event; // interval time
float listen_freq; // frequency
//
int oledx, oledy; // position
char *s1, *s2;
char szbuf[10];  // string to display
char szbuf0[10]; // string to display

//
void draw(char *s) {
  // graphic commands to redraw the complete screen should be placed here
  if (oledy == 0)  s1 = s; else s2 = s;
  u8g2.setFont(u8g_font_unifont);
  //u8g.setFont(u8g_font_osb21);
  u8g2.drawStr( 0, 16, s1);
  u8g2.drawStr( 0, 32, s2);
}
void lcd_print(const char *s) {
  u8g2.firstPage();
  do {
    draw((char *)s);
  } while ( u8g2.nextPage() );
  delay(50);
}
void lcd_setCursor(int x, int y) {
  oledx = x;
  oledy = y;
}
char* convertfloatchar(float value) {
  int whl, frac;
  whl = (int)value;                // convert to integer
  frac = (int)(value * 100) % 100; // get decimal point digit
  if (mode != 1) { // KHz
    sprintf(&szbuf[0], "%d", whl);
  }
  else { // MHz
    sprintf(&szbuf[0], "%d.%02d", whl, frac);
  }
  return szbuf;
}
void i2c_write(int device_address, int memory_address, int value)
{
  Wire.beginTransmission(device_address);
  Wire.write(memory_address);
  Wire.write(value);
  Wire.endTransmission();
  delay(3);
}

void i2c_read(int device_address, int memory_address)
{
  Wire.beginTransmission(device_address);
  Wire.write(memory_address);
  Wire.endTransmission(false);
  Wire.requestFrom(device_address, 1, false);
  read_byte = Wire.read();
  Wire.endTransmission(true);
  delay(2);
}

void setup()
{
  Serial.begin(115200);
  Wire.begin() ;
  u8g2.begin();
  pinMode(2, INPUT_PULLUP);  // mode_setting
  pinMode(3, INPUT_PULLUP);  // station_setting
  pinMode(1, INPUT_PULLUP);  // reset_setting
  digitalWrite(2, HIGH);
  digitalWrite(3, HIGH);
  digitalWrite(1, HIGH);
  attachInterrupt(2, mode_setting, FALLING); // xiao same as pin 2 , 0 -> pin 2 (uno)
  attachInterrupt(3, station_setting, FALLING); // xiao same as pin 3 , 1 -> pin 3 (uno)
  attachInterrupt(1, reset_setting, FALLING); // xiao same as pin 1 
  lcd_setCursor(0, 0);
  lcd_print("Hello!");
  lcd_setCursor(0, 1);
  lcd_print("FM/AM/SW RADIO");
  delay(1000); // display 1 sec
  pinMode(terminal_1, INPUT);
  pinMode(terminal_2, INPUT);
  Serial.println("Step1");
  //
  i2c_write(RADIO, 0x01, 0b00010001); ///FM-band Japan    WIDE
  Serial.println("Step1.1");
  i2c_write(RADIO, 0x00, 0b11000000); ///power_on,FM, tune0,seek0,seek_down,non_mute,00
  i2c_write(RADIO, 0x06, 0b01100000); ///VOLUME:24, DUAL SPEAKER
  listen_freq = 80.4; ////frequency MHz for FM, kHz for AM   ex. AIR-G
  initial_num = (listen_freq - 30) * 40;
  channel_num = initial_num + encorder_val * 4;
  s_upper2 = channel_num / 256 | 0b01100000;
  s_lower2 = channel_num & 0b0000011111111;
  i2c_write(RADIO, 0x03, s_lower2);
  i2c_write(RADIO, 0x02, s_upper2);
  i2c_write(RADIO, 0x00, 0b11100000); ///power_on,FM, tune1,seek0,seek_down,non_mute,00
  i2c_write(RADIO, 0x00, 0b11000000); ///power_on,FM, tune0,seek0,seek_down,non_mute,00
  mode = 1;  // FM/AM
  mode_set = 1; // Band MW=0,FM=1,SW=2,...
  hoge = channel_num;
  delay(100);
  Serial.println("Step2");
  lcd_setCursor(0, 0);
  //mode==1:FM, mode==0:AM
  if (mode == 1) {
    lcd_print("FM");
    Serial.println("FM");
    freq = int(hoge) * 0.025 + 30.0;
  } else {
    if (mode_set == 2) {
      lcd_print("SW");
      Serial.println("SW");
      freq = int(hoge) * 5;
    } else {
      lcd_print("AM");
      Serial.println("AM");
      freq = int(hoge) * 3;
    }
  }
  Serial.print(freq);
  lcd_setCursor(0, 1);
  if (mode == 1) {
    Serial.println("MHz");
    lcd_print(strcat(convertfloatchar(freq), "MHz"));
  } else {
    Serial.println("kHz");
    lcd_print(strcat(convertfloatchar(freq), "kHz"));
  }
}///end of setup

void loop()
{
  unsigned char result = rot.process();
  int ms;
  //long counter;
  if (result == DIR_CW) {
    encorder_val++;
    Serial.print("RR:");
    Serial.println(encorder_val);
    event = 1;
  } else if (result == DIR_CCW) {
    encorder_val--;
    Serial.print("RR:");
    Serial.println(encorder_val);
    event = 1;
  }
  //
  if (event == 1) {
    Serial.print("event:");
    Serial.println(mode_set);
    switch (mode_set) {
      case 0://AM
        i2c_write(RADIO, 0x00, 0b10000000); ///power_on,AM, tune0,seek0,seek_down,non_mute,00
        i2c_write(RADIO, 0x01, 0b00010001); ///AM-band wide
        switch (station) {
          case 0:
            listen_freq = 567.0; ////frequency MHz for FM, kHz for AM ex. NHK 1
            break;
          case 1:
            listen_freq = 747.0; ////frequency MHz for FM, kHz for AM ex. NHK 2
            break;
          case 2:
            listen_freq = 960.0; ////frequency MHz for FM, kHz for AM 
            break;
          case 3:
            listen_freq = 1134.0; ////frequency MHz for FM, kHz for AM 
            break;
          case 4:
            listen_freq = 1287.0; ////frequency MHz for FM, kHz for AM 
            break;
          case 5:
            listen_freq = 1440.0; ////frequency MHz for FM, kHz for AM 
            break;
          default:
            listen_freq = 567.0; ////frequency MHz for FM, kHz for AM 
            break;
        }
        initial_num = listen_freq / 3;
        channel_num = initial_num + encorder_val * 3; 
        s_upper2 = channel_num / 256 | 0b01100000;
        s_lower2 = channel_num & 0b0000011111111;
        i2c_write(RADIO, 0x03, s_lower2);
        i2c_write(RADIO, 0x02, s_upper2);
        i2c_write(RADIO, 0x00, 0b10100000);
        i2c_write(RADIO, 0x00, 0b10000000);
        mode = 0;
        break;
      case 1://FM
        i2c_write(RADIO, 0x01, 0b00010001); ///FM-band Japan wide
        i2c_write(RADIO, 0x00, 0b11000000); ///power_on,FM, tune0,seek0,seek_down,non_mute,00
        switch (station) {
          case 0:
            listen_freq = 80.4; ////frequency MHz for FM, kHz for AM ex. airg
            break;
          case 1:
            listen_freq = 82.5; ////frequency MHz for FM, kHz for AM ex. nw
            break;
          case 2:
            listen_freq = 85.2; ////frequency MHz for FM, kHz for AM ex. NHK
            break;
          case 3:
            listen_freq = 90.4; ////frequency MHz for FM, kHz for AM ex. STV
            break;
          case 4:
            listen_freq = 91.5; ////frequency MHz for FM, kHz for AM ex. HBC
            break;
          case 5:
            listen_freq = 76.22; ////frequency MHz for FM, kHz for AM ex. sankaku
            break;
          default:
            listen_freq = 80.4; ////frequency MHz for FM, kHz for AM
            break;
        }
        initial_num = (listen_freq - 30) * 40;
        channel_num = initial_num + encorder_val * 4; 
        s_upper2 = channel_num / 256 | 0b01100000;
        s_lower2 = channel_num & 0b0000011111111;
        i2c_write(RADIO, 0x03, s_lower2);
        i2c_write(RADIO, 0x02, s_upper2);
        i2c_write(RADIO, 0x00, 0b11100000);
        i2c_write(RADIO, 0x00, 0b11000000);
        mode = 1;
        break;
      case 2://SW2
        ms = 2;
        i2c_write(RADIO, 0x01, 0b00101011); ///SW2
        i2c_write(RADIO, 0x00, 0b10000000); ///power_on,AM, tune0,seek0,seek_down,non_mute,00
        listen_freq = 3200.0; ////frequency  kHz for SW2
        set_SW_band();
        //initial_num = listen_freq / 5;
        //channel_num = initial_num + (encorder_val / 3) * 3; // #### use encorder lib
        //channel_num = initial_num + encorder_val;
        //s_upper2 = channel_num / 256 | 0b01000000; // AM, 5kHz step
        //s_lower2 = channel_num & 0b0000011111111;
        //i2c_write(RADIO, 0x03, s_lower2);
        //i2c_write(RADIO, 0x02, s_upper2);
        //i2c_write(RADIO, 0x00, 0b10100000); // trigger tune=1
        //i2c_write(RADIO, 0x00, 0b10000000);
        //mode = 0;
        break;
      case 3://SW4
        ms = 4;
        i2c_write(RADIO, 0x01, 0b00111011); ///SW4
        i2c_write(RADIO, 0x00, 0b10000000); ///power_on,AM, tune0,seek0,seek_down,non_mute,00
        listen_freq = 5700.0; ////frequency , kHz for SW4
        set_SW_band();
        break;
      case 4://SW5
        ms = 5;
        i2c_write(RADIO, 0x01, 0b01000011); ///SW5
        i2c_write(RADIO, 0x00, 0b10000000); ///power_on,AM, tune0,seek0,seek_down,non_mute,00
        listen_freq = 6800.0; ////frequency , kHz for SW5
        set_SW_band();
        break;
      case 5://SW6
        ms = 6;
        i2c_write(RADIO, 0x01, 0b01001011); ///SW6
        i2c_write(RADIO, 0x00, 0b10000000); ///power_on,AM, tune0,seek0,seek_down,non_mute,00
        listen_freq = 9200.0; ////frequency , kHz for SW6
        set_SW_band();
        break;
      case 6://SW7
        ms = 7;
        i2c_write(RADIO, 0x01, 0b01010011); ///SW7
        i2c_write(RADIO, 0x00, 0b10000000); ///power_on,AM, tune0,seek0,seek_down,non_mute,00
        listen_freq = 11400.0; ////frequency , kHz for SW7
        set_SW_band();
        break;
      case 7://LW
        i2c_write(RADIO, 0x01, 0b00000011); ///LW
        i2c_write(RADIO, 0x00, 0b10000000); ///power_on,AM, tune0,seek0,seek_down,non_mute,00
        listen_freq = 190.0; ////frequency MHz for FM, kHz for AM ADR
        initial_num = listen_freq / 3;
        //channel_num = initial_num + (encorder_val / 3) * 3; // #### use encorder lib
        channel_num = initial_num + encorder_val;
        s_upper2 = channel_num / 256 | 0b01100000; // AM, 3kHz step
        s_lower2 = channel_num & 0b0000011111111;
        i2c_write(RADIO, 0x03, s_lower2);
        i2c_write(RADIO, 0x02, s_upper2);
        i2c_write(RADIO, 0x00, 0b10100000);
        i2c_write(RADIO, 0x00, 0b10000000);
        mode = 0;
        break;
    }
    delay(100);
    lcd_setCursor(0, 0);
    hoge = channel_num;
    if (mode == 1) { //FM
      lcd_print("FM");
      freq = int(hoge) * 0.025 + 30.0;
    } else {
      //AM
      if (mode_set == 7) { // WHEN LW=7
        lcd_print("LW");
        freq = int(hoge) * 3;
      } else {
        if (mode_set >= 2) { // WHEN SW=2-6
          sprintf(szbuf0, "%s%d", "SW", ms);
          lcd_print(szbuf0);
          //lcd_print("SW");
          freq = int(hoge) * 5;
        } else { // WHEN AM=0
          lcd_print("AM");
          freq = int(hoge) * 3;
        }
      }
    }
    lcd_setCursor(0, 1);
    Serial.print(freq);
    //lcd_setCursor(5,1);
    if (mode == 1) {
      lcd_print(strcat(convertfloatchar(freq), "MHz"));
      Serial.println("MHz");
    } else {
      lcd_print(strcat(convertfloatchar(freq), "KHz"));
      Serial.println("kHz");
    }
    event = 0;
    //value = 0;
  }  // event==1
}

void set_SW_band() {
        //i2c_write(RADIO, 0x01, 0b00111011); ///SW4
        //i2c_write(RADIO, 0x00, 0b10000000); ///power_on,AM, tune0,seek0,seek_down,non_mute,00
        switch (station) {
          case 0:
            break;
          case 1:
            listen_freq = listen_freq + 200.0; //add frequency KHz
            break;
          case 2:
            listen_freq = listen_freq + 300.0; //add frequency KHz
            break;
          case 3:
            listen_freq = listen_freq + 400.0; //add frequency KHz
            break;
          case 4:
            listen_freq = listen_freq + 500.0; //add frequency KHz
            break;
          case 5:
            listen_freq = listen_freq + 600.0; //add frequency KHz
            break;
          default:
            listen_freq = listen_freq; //
            break;
        }
        initial_num = listen_freq / 5;
        channel_num = initial_num + encorder_val;
        s_upper2 = channel_num / 256 | 0b01000000; // AM, 5kHz step
        s_lower2 = channel_num & 0b0000011111111;
        i2c_write(RADIO, 0x03, s_lower2);
        i2c_write(RADIO, 0x02, s_upper2);
        i2c_write(RADIO, 0x00, 0b10100000); // trigger tune=1
        i2c_write(RADIO, 0x00, 0b10000000);
        mode = 0;
}

void mode_setting() {
  // mode 0-7
  int sw;
  Serial.println("md");
  ct = millis();
  delay(10);  // no effect here
  sw = digitalRead(2);
  if ((ct - pt) > 250) {
    mode_set = mode_set + 1;
  }
  pt = ct;
  if (mode_set >= 8) {
    mode_set = 0;
  }
  event = 1;
  encorder_val = 0;
  value = 0;
  //myEnc.readAndReset();
}

void station_setting() {
  // station 0-5
  int sw;
  Serial.print("st:");
  Serial.println(station);
  ct2 = millis();
  delay(10);  // no effect here
  sw = digitalRead(3);
  if ((ct2 - pt2) > 250) {
    station = station + 1;
  }
  pt2 = ct2;
  if (station >= 6) {
    station = 0;
  }
  event = 1;
  encorder_val = 0;
  value = 0;
  //myEnc.readAndReset();
}

void reset_setting() {
  // reset
  Serial.println("rst:");
  station = 0; 
  mode = 1;    // FM
  mode_set = 1; // FM
  encorder_val = 0;
  value = 0;
  event = 1;
}
