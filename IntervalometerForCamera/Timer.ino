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

void Timer() {
  static uint32_t timer, timer2, waitTime = 0;
  if (modes.pos == modes.MANUAL_SHUTTER_SPEED) {
    waitTime = camSettings.interval * 1000;
    if (millis() - timer > waitTime) {
      timer = millis();
      take_photo(false);
      if (camSettings.frame_rate > 0) {
        camSettings.frame_rate--;
      }
    }
  } else {
    sleepCheck();
    if (camSettings.longExpNR == false) waitTime = camSettings.interval * 1000 + camSettings.shutter_speed;
    else waitTime = camSettings.interval * 1000 + camSettings.shutter_speed * 2;
    if (millis() - timer > waitTime) {
      timer = millis();
      draw();
      take_photo(true);
      timer2 = millis();
      while (camSettings.longExpNR == true && millis() - timer2 < camSettings.shutter_speed) {
      }
      canSleepFunc();
      if (camSettings.frame_rate > 0) {
        camSettings.frame_rate--;
      }
    }
  }
}

void take_photo(bool SS) {
  if (camSettings.frame_rate > 0) {
    if (SS == true) photoPinsControl(camSettings.shutter_speed);
    else photoPinsControl(500);
  }
}

void photoPinsControl(uint32_t shutSpeed) {
  static uint32_t timer, timer2, waitTime = 0;
  if (camSettings.AFControl == true) waitTime = shutSpeed + FOCUSE_TIME;
  else waitTime = shutSpeed;
  timer = millis();
  while (millis() - timer < waitTime) {
    if (camSettings.AFControl == true) {
      if ( millis() - timer2 > FOCUSE_TIME) {
        timer2 = millis();
        digitalWrite(FOCUS_PIN, HIGH);
      }
    }
    digitalWrite(SHUTTER_PIN, HIGH);
  }
  if (camSettings.AFControl == true) digitalWrite(FOCUS_PIN, LOW);
  digitalWrite(SHUTTER_PIN, LOW);
}
