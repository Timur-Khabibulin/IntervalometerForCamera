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

void valueUp() {
  if (page2 == false && lr_button < (MENU_ITEMS_NUM - 1)) lr_button++;
  if (pos[0] == true && modes.pos < modes.BULB_SHUTTER_SPEED) modes.pos++;
  if (pos[1] == true) camSettings.interval++;
  if (pos[2] == true) camSettings.frame_rate++;
  if (pos[3] == true) {
    if (modes.pos == modes.MANUAL_SHUTTER_SPEED && SSCounter < 11) SSCounter++;
    if (modes.pos == modes.BULB_SHUTTER_SPEED && camSettings.BulbShutterSpeed < 3600) camSettings.BulbShutterSpeed++;
  }
  if (pos[4] == true && modes.pos == modes.BULB_SHUTTER_SPEED) camSettings.longExpNR = !camSettings.longExpNR;
  if (pos[5] == true) camSettings.AFControl = !camSettings.AFControl;
  if (pos[6] == true) camSettings.camStandby = !camSettings.camStandby;
}
void valueDown() {
  if (page2 == false && lr_button != 0) lr_button--;
  if (pos[0] == true && modes.pos != modes.AUTO_SHUTTER_SPEED) modes.pos--;
  if (pos[1] == true && camSettings.interval != 0) camSettings.interval--;
  if (pos[2] == true && camSettings.frame_rate != 0) camSettings.frame_rate--;
  if (pos[3] == true) {
    if (modes.pos == modes.MANUAL_SHUTTER_SPEED && SSCounter != 0)SSCounter--;
    if (modes.pos == modes.BULB_SHUTTER_SPEED && camSettings.BulbShutterSpeed != 0) camSettings.BulbShutterSpeed--;
  }
  if (pos[4] == true && modes.pos == modes.BULB_SHUTTER_SPEED) camSettings.longExpNR = !camSettings.longExpNR;
  if (pos[5] == true) camSettings.AFControl = !camSettings.AFControl;
  if (pos[6] == true) camSettings.camStandby = !camSettings.camStandby;
}
void clickleft() {
  valueDown();
}
void clickright() {
  valueUp();
}
void click_ok() {
  if (!start) pos[lr_button] = !pos[lr_button];
}
void longPressleft() {
  valueDown();
}
void longPressright() {
  valueUp();
}
void longPressOK() {
  if (!start) {
    if (modes.pos == modes.AUTO_SHUTTER_SPEED) {
      if (camSettings.interval > 0 && camSettings.frame_rate > 0) start = true;
    } else {
      if ((camSettings.interval * 1000) > camSettings.shutter_speed && camSettings.frame_rate > 0 && camSettings.shutter_speed > 0 && camSettings.longExpNR == false) start = true;
      if ((camSettings.interval * 1000) > (camSettings.shutter_speed * 2) && camSettings.frame_rate > 0 && camSettings.shutter_speed > 0 && camSettings.longExpNR == true) start = true;
    }
  } else {
    start = false;
    lr_button = 0;
  }
  draw();
}
