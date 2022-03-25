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

ISR(TIMER1_COMPA_vect) {
  hwTimer1.count++; // Период 1 с

  static uint32_t intervalTime;
  static bool sleepState = false;

  if (camSettings.longExpNR) intervalTime = camSettings.interval + camSettings.shutter_speed / 1000;
  else intervalTime = camSettings.interval;

  if (camSettings.hibernationSt) hibernation(intervalTime, sleepState);

  if (hwTimer1.count >= intervalTime) {
    if (camSettings.frame_rate > 0) {
      if (camSettings.AFControl) digitalWrite(FOCUS_PIN, HIGH);
      digitalWrite(SHUTTER_PIN, HIGH);
    }
    hw_Timer_1_Stop();
    hw_Timer_2_Start();
  }

}

ISR(TIMER2_COMPA_vect) {
  hwTimer2.count += 2; // Период 2 мс

  if (hwTimer2.count >= camSettings.shutter_speed) {
    if (camSettings.frame_rate > 0) {
      if (camSettings.AFControl) digitalWrite(FOCUS_PIN, LOW);
      digitalWrite(SHUTTER_PIN, LOW);
      camSettings.frame_rate--;
    }
    hw_Timer_2_Stop();
    hw_Timer_1_Start();
  }

}
