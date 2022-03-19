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

void hibernation() {
  static bool flag = false;
  if (camSettings.canSleep == true && flag == false) {
    powerPinControl();
    flag = true;
  }
  if (camSettings.canSleep == false && flag == true) {
    powerPinControl();
    flag = false;
  }
}
void powerPinControl() {
  static uint32_t timer1;
  timer1 = millis();
  while (millis() - timer1 < 500) {
    digitalWrite(POWER_MODE_PIN, HIGH);
  }
  digitalWrite(POWER_MODE_PIN, LOW);
}
void canSleepFunc() {
  if (camSettings.hibernationSt == true) camSettings.canSleep = true;
}
void sleepCheck() {
  static uint32_t tmr;
  if (camSettings.hibernationSt == true) {
    if (millis() - tmr > (camSettings.interval * 1000 + camSettings.shutter_speed - TIME_TO_WAKEUP)) {
      camSettings.canSleep = false;
    }
  }
}
