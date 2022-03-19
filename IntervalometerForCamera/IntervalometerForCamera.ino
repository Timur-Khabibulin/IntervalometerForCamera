/*  Intervalometer / Timer for interval shooting.
    Copyright © 2022 Timur Khabibulin.
    All rights reserved.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See thecamera interval timer
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <OneButton.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Fonts/FreeSans9pt7b.h"
#include "Fonts/FreeSans12pt7b.h"
#include "Fonts/SpecialFont13pixels.h"

#define VERSION 1

#define SHUTTER_PIN 5    // Пин для подключение кнопки "Спуск"
#define FOCUS_PIN 6      // Пин для управления фокусировкой
#define POWER_MODE_PIN 4 // Пин для управления спящим режимом камеры

#define FOCUSE_TIME 1000
#define TIME_TO_WAKEUP 1500

OneButton LEFT_BUTTON(A2, true);
OneButton RIGHT_BUTTON(A0, true);
OneButton OK_BUTTON(A1, true);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define MENU_ITEMS_NUM 8
#define ITEMS_NUM_ON_ONE_PAGE 3

const char itemData_1[] PROGMEM = "SS mode";
const char itemData_2[] PROGMEM = "Interval";
const char itemData_3[] PROGMEM = "Frame rate";
const char itemData_4[] PROGMEM = "Shutter speed";
const char itemData_5[] PROGMEM = "Long exp NR";
const char itemData_6[] PROGMEM = "AF control";
const char itemData_7[] PROGMEM = "Cam standby";
const char itemData_8[] PROGMEM = "About device";
const char* const itemDataLinks[] PROGMEM = {
  itemData_1, itemData_2, itemData_3, itemData_4, itemData_5, itemData_6, itemData_7, itemData_8
};

const char item2Data_1[] PROGMEM = "A-Auto SS";
const char item2Data_2[] PROGMEM = "M-SS < 1s";
const char item2Data_3[] PROGMEM = "B-SS > 1s";
const char* const itemData2Links[] PROGMEM = {
  item2Data_1, item2Data_2, item2Data_3
};

const char shutterSpeed_1[] PROGMEM = "1/1000 s";
const char shutterSpeed_2[] PROGMEM = "1/500 s";
const char shutterSpeed_3[] PROGMEM = "1/250 s";
const char shutterSpeed_4[] PROGMEM = "1/125 s";
const char shutterSpeed_5[] PROGMEM = "1/100 s";
const char shutterSpeed_6[] PROGMEM = "1/60 s";
const char shutterSpeed_7[] PROGMEM = "1/30 s";
const char shutterSpeed_8[] PROGMEM = "1/20 s";
const char shutterSpeed_9[] PROGMEM = "1/10 s";
const char shutterSpeed_10[] PROGMEM = "1/8 s";
const char shutterSpeed_11[] PROGMEM = "1/5 s";
const char shutterSpeed_12[] PROGMEM = "1/2 s";
const char* const shutterSpeedLinks[] PROGMEM = {
  shutterSpeed_1, shutterSpeed_2, shutterSpeed_3, shutterSpeed_4, shutterSpeed_5, shutterSpeed_6,
  shutterSpeed_7, shutterSpeed_8, shutterSpeed_9, shutterSpeed_10, shutterSpeed_11, shutterSpeed_12
};
const char* switchNames[] = {"OFF", "ON"};

int shutterSpeedValues[] = {1, 2, 4, 8, 10, 17, 33, 50, 100, 125, 200, 500};
byte lr_button = 0;
byte SSCounter;
bool page2 = false;
bool pos[MENU_ITEMS_NUM] = {false, false, false, false, false, false, false};
bool start = false;

struct Modes {
  byte pos = 0;
  const byte AUTO_SHUTTER_SPEED = 0;
  const byte MANUAL_SHUTTER_SPEED = 1;
  const byte BULB_SHUTTER_SPEED = 2;
};
Modes modes;

struct CamStruct {
  uint32_t interval;
  int frame_rate;
  uint32_t BulbShutterSpeed;
  uint32_t shutter_speed;
  bool canSleep: 1;
  bool hibernationSt: 1;
  bool AFControl: 1;
  bool camStandby: 1;
  bool longExpNR: 1;
};
CamStruct camSettings;

void setup() {
  LEFT_BUTTON.attachClick(clickleft);
  LEFT_BUTTON.attachDuringLongPress(longPressleft);
  RIGHT_BUTTON.attachClick(clickright);
  RIGHT_BUTTON.attachDuringLongPress(longPressright);
  OK_BUTTON.attachClick(click_ok);
  OK_BUTTON.attachLongPressStart(longPressOK);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  pinMode(SHUTTER_PIN, OUTPUT);
  pinMode(FOCUS_PIN, OUTPUT);
  pinMode(POWER_MODE_PIN, OUTPUT);
  analogReference(INTERNAL);
  logo();
  delay(2000);
}

void loop() {
  OK_BUTTON.tick();
  LEFT_BUTTON.tick();
  RIGHT_BUTTON.tick();
  draw();
  if (modes.pos == modes.MANUAL_SHUTTER_SPEED) camSettings.shutter_speed = shutterSpeedValues[SSCounter];
  if (modes.pos == modes.BULB_SHUTTER_SPEED) camSettings.shutter_speed = camSettings.BulbShutterSpeed * 1000;
  if (start == true) Timer();
  for (byte i = 0; i < MENU_ITEMS_NUM; i++) {
    if (pos[i] == true) {
      page2 = true;
      break;
    } else page2 = false;
  }
  if (camSettings.camStandby == true && (camSettings.interval - camSettings.shutter_speed) > 10000) camSettings.hibernationSt = true;
  else camSettings.hibernationSt = false;
  if (camSettings.hibernationSt == true) hibernation();
  if (modes.pos != modes.BULB_SHUTTER_SPEED) camSettings.longExpNR = false;
}

String printFromPGM(const char* const* charMap) {
  char buff[19];      // буфер для хранения строки
  uint16_t ptr = pgm_read_word(charMap); // получаем адрес из таблицы ссылок
  uint8_t i = 0;        // переменная - индекс массива буфера
  do {
    buff[i] = (char)(pgm_read_byte(ptr++)); // прочитать символ из PGM в ячейку буфера, подвинуть указатель
  } while (buff[i++] != 0);              // повторять пока прочитанный символ не нулевой, подвинуть индекс буфера
  return (String)buff;
}
