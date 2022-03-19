/*  Intervalometer / Timer for interval shooting.
    Copyright © 2022 Timur Khabibulin.
    All rights reserved.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

void draw() {
  if (start == false) {
    if (page2 == false) page_1();
    else page_2();
  } else if (camSettings.frame_rate > 0)  recWindow(); else endMessgae();
  display.display();
}

void page_1() {
  byte scrollBarY0 = map(lr_button, 0, (MENU_ITEMS_NUM - 1), 15, 51);
  const int8_t itemsYpos[] = {27, 43, 59};
  byte pointerYpos;

  if (lr_button < ITEMS_NUM_ON_ONE_PAGE) {
    pointerYpos = map(lr_button, 0, ITEMS_NUM_ON_ONE_PAGE, 15, 64);
  } else pointerYpos = 48;

  display.clearDisplay();
  statusBar();
  display.setFont(&SpecialFont13pixels);
  for (byte i = 0; i < ITEMS_NUM_ON_ONE_PAGE; i++) {
    if (lr_button < ITEMS_NUM_ON_ONE_PAGE) {
      display.setCursor(5, itemsYpos[i]);
      display.print(printFromPGM(&itemDataLinks[i]));
    } else {
      display.setCursor(5, itemsYpos[i]);
      display.print(printFromPGM(&itemDataLinks[i + (lr_button - 2)]));
    }
  }
  display.drawRoundRect(1, pointerYpos, 120, 15, 4, 1);
  display.drawRect(122, 15, 4, 49, 1);
  display.fillRect(123, scrollBarY0, 3, 13, 1);
  display.display();
}

void page_2() {
  display.clearDisplay();
  if (lr_button != 0 && lr_button != 7) {
    display.setFont(&FreeSans9pt7b);
    display.setCursor(5, 15);
    display.print(printFromPGM(&itemDataLinks[lr_button]));
  }

  switch (lr_button) {
    case 0: menuItem_0(); break;
    case 1:
      display.setCursor(37, 40);
      display.print(camSettings.interval);
      break;
    case 2:
      display.setCursor(37, 40);
      display.print(camSettings.frame_rate);
      break;
    case 3:
      display.setCursor(37, 40);
      if (modes.pos != modes.AUTO_SHUTTER_SPEED) {
        if (modes.pos == modes.MANUAL_SHUTTER_SPEED) display.print(printFromPGM(&shutterSpeedLinks[SSCounter]));
        else if (modes.pos == modes.BULB_SHUTTER_SPEED) display.print(camSettings.shutter_speed / 1000);
      } else errorSSMessgae();
      break;
    case 4:
      if (modes.pos == modes.BULB_SHUTTER_SPEED) menuItemWithSwitch(camSettings.longExpNR);
      else errorNRMessage();
      break;
    case 5: menuItemWithSwitch(camSettings.AFControl); break;
    case 6: menuItemWithSwitch(camSettings.camStandby); break;
    case 7: deviceInfo(); break;
  }
}

void recWindow() {
  display.clearDisplay();
  display.drawLine(0, 38, 128, 38, 1);
  display.drawLine(42, 0, 42, 38, 1);
  // display.drawLine(95, 0, 95, 38, 1);
  display.drawLine(64, 38, 64, 64, 1);
  display.setFont(&FreeSans9pt7b);

  if (modes.pos == modes.AUTO_SHUTTER_SPEED) {
    display.setCursor(2, 18);
    display.print("A");
    display.setCursor(50, 20);
    display.setFont(&SpecialFont13pixels);
    display.print(F("AUTO SS"));
  }
  if (modes.pos == modes.MANUAL_SHUTTER_SPEED) {
    display.setCursor(2, 18);
    display.print("M");
    display.setCursor(50, 20);
    display.print(printFromPGM(&shutterSpeedLinks[SSCounter]));
  }
  if (modes.pos == modes.BULB_SHUTTER_SPEED) {
    display.setCursor(2, 18);
    display.print("B");
    display.setCursor(50, 20);
    display.print(String(camSettings.shutter_speed / 1000) + " s");
  }
  if (camSettings.AFControl == true) {
    display.setFont(&SpecialFont13pixels);
    display.setCursor(20, 12);
    display.print("AF");
  }
  if (camSettings.longExpNR == true) {
    display.setFont(&SpecialFont13pixels);
    display.setCursor(20, 24);
    display.print("NR");
  }
  if (camSettings.camStandby == true) {
    display.setFont(&SpecialFont13pixels);
    display.setCursor(0, 35);
    display.print("stby");
  }
  display.setFont(&SpecialFont13pixels);
  display.setCursor(5, 60);
  display.print("Fr:");
  display.setCursor(70, 60);
  display.print("I:");
  display.setFont(&FreeSans9pt7b);
  display.setCursor(80, 60);
  display.print(String(camSettings.interval) + " s");
  display.setCursor(25, 60);
  display.print(camSettings.frame_rate);
  display.setFont(&SpecialFont13pixels);
  /* display.setCursor(100, 14);
    display.print(BatteryLevel());
    display.setCursor(100, 29);
    display.print("%");
    if (chargeStatus == true) display.drawBitmap(116, 17, BatCharge, BatCharge_width, BatCharge_height, 1);*/
}
void endMessgae() {
  display.clearDisplay();
  display.setCursor(30, 13);
  display.print(F("Shooting"));
  display.setCursor(40, 28);
  display.print(F("is over"));
}
void errorSSMessgae() {
  display.clearDisplay();
  display.setCursor(25, 15);
  display.print(F("Disable"));
  display.setCursor(10, 40);
  display.print(F("AutoSS mode"));
  display.display();
  delay(1000);
  pos[lr_button] = false;
  page2 = false;
}
void errorNRMessage() {
  display.clearDisplay();
  display.setCursor(25, 15);
  display.print(F("Turn on"));
  display.setCursor(5, 40);
  display.print(F("BULBSS mode"));
  display.display();
  delay(1000);
  pos[lr_button] = false;
  page2 = false;
}
void statusBar() {
  /* byte posX;
    if (BatteryLevel() < 100)posX = 107; else posX = 115;
    display.drawLine(0, 13, 128, 13, 1);
    display.setFont(&SpecialFont13pixels);
    display.setCursor(87, 12);
    display.print(BatteryLevel());
    display.setCursor(posX, 12);
    display.print("%");
    if (chargeStatus == true) display.drawBitmap(78, 1, BatCharge, BatCharge_width, BatCharge_height, 1);*/
  display.drawLine(0, 13, 128, 13, 1);
  display.setTextSize(1);
  display.setCursor(2, 12);
  display.print("v-");
  display.setCursor(17, 12);
  display.print(VERSION);
}
void logo() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(2, 50);
  display.print(F("by Timur Khabibulin"));
  display.setFont(&SpecialFont13pixels);
  display.setCursor(0, 20);
  display.print(F("INTERVALOMETER"));
  display.setCursor(40, 40);
  display.print("v -");
  display.setCursor(63, 40);
  display.print(VERSION);
  display.display();
}

void menuItem_0() {
  byte pointerYpos = map(modes.pos, 0, 2, 0, 37);
  display.setFont(&FreeSans9pt7b);
  byte item2Ypos[3];
  for (byte i = 0; i < 3; i++) {
    if (i == 0) item2Ypos[i] = 14; else item2Ypos[i] = item2Ypos[i - 1] + 19;
  }
  for (byte k = 0; k < 3; k++) {
    display.setCursor(5, item2Ypos[k]);
    display.print(printFromPGM(&itemData2Links[k]));
  }
  display.drawRoundRect(1, pointerYpos, 100, 18, 4, 1);
  display.fillCircle(110, (pointerYpos + 8), 4, 1);

  byte circleYpos[3];
  for (byte h = 0; h < 3; h++) {
    if (h == 0) circleYpos[h] = 8; else circleYpos[h] = circleYpos[h - 1] + 18;
  }
  for (byte l = 0; l < 3; l++) {
    display.drawCircle(110, circleYpos[l], 8, 1);
  }
}

void menuItemWithSwitch(bool mode) {
  byte pointerYpos[] = {22, 42};
  byte itemYpos[] = {37, 57};
  display.setFont(&FreeSans9pt7b);
  for (byte i = 0; i < 2; i++) {
    display.setCursor(5, itemYpos[i]);
    display.print(switchNames[i]);
  }
  display.drawRoundRect(2, pointerYpos[mode], 70, 18, 4, 1);
}
void deviceInfo() {
  display.drawLine(0, 18, 128, 18, 1);
  display.drawLine(0, 19, 128, 19, 1);
  display.drawLine(0, 20, 128, 20, 1);
  display.setFont(&SpecialFont13pixels);
  display.setCursor(0, 16);
  display.print(F("INTERVALOMETER"));
  display.setCursor(2, 33);
  display.print(F("Version  -"));
  display.setCursor(80, 33);
  display.print(VERSION);
  display.setCursor(2, 49 );
  display.print(F("Timur Khabibulin"));
  display.setCursor(2, 63);
  display.print(F("2022"));
}
