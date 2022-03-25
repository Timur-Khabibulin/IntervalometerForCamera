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

void Start() {
  if (modes.pos == modes.AUTO_SHUTTER_SPEED) camSettings.shutter_speed = 500;
  if (modes.pos == modes.MANUAL_SHUTTER_SPEED) camSettings.shutter_speed = shutterSpeedValues[SSCounter];
  if (modes.pos == modes.BULB_SHUTTER_SPEED) camSettings.shutter_speed = camSettings.BulbShutterSpeed * 1000; // to ms
  if (modes.pos != modes.BULB_SHUTTER_SPEED) camSettings.longExpNR = false;

  if (camSettings.AFControl) digitalWrite(FOCUS_PIN, HIGH);
  digitalWrite(SHUTTER_PIN, HIGH);
  hw_Timer_2_Start();
  startFlag = true;
}

void Stop() {
  startFlag = false;
  hw_Timer_1_Stop();
  hw_Timer_2_Stop();
}

void hw_Timer_1_Stop() {
  hwTimer1.count = 0;
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  sei();
}

void hw_Timer_2_Stop() {
  hwTimer2.count = 0;
  cli();
  TCCR2A = 0;
  TCCR2B = 0;
  sei();
}

void hw_Timer_1_Start() {
  // ==== Timer 1 ====
  hwTimer1.count = 0;
  cli();

  hwTimer1.timerVal = hwTimer1.PERIOD_SEC * hwTimer1.CPU_FREQ_HZ / hwTimer1.PRESCALER - 1;

  hw_Timer_1_Stop();

  OCR1A = hwTimer1.timerVal; // установка регистров совпадения

  TCCR1B |= (1 << WGM12);  // включить CTC режим
  TCCR1B |= (1 << CS10); // Установить биты на коэффициент деления 1024
  TCCR1B |= (1 << CS12);
  TIMSK1 |= (1 << OCIE1A);  // включить прерывание по совпадению таймера
  // ================
  sei();
}

void hw_Timer_2_Start() {
  hwTimer2.count = 0;
  // ==== Timer 2 =====
  cli();
  hwTimer2.timerVal = hwTimer2.PERIOD_SEC * hwTimer2.CPU_FREQ_HZ / hwTimer2.PRESCALER - 1;

  hw_Timer_2_Stop();

  OCR2A = hwTimer2.timerVal;

  TCCR2A |= (1 << WGM21);  // включить CTC режим
  TCCR2B = (1 << CS22) | (1 << CS21) | (0 << CS20); // Установить биты на коэффициент деления 256
  TIMSK2 |= (1 << OCIE2A);  // включить прерывание по совпадению таймера
  hwTimer2.state = true;
  sei();
}

void hibernation(uint32_t intervalTime, bool sleepState) {
  if (!sleepState) {
    if (camSettings.longExpNR) {
      if ((hwTimer1.count >= (camSettings.shutter_speed / 1000 + 2)) && (hwTimer1.count <= (camSettings.shutter_speed / 1000 + 4)))
        digitalWrite(POWER_MODE_PIN, HIGH);
      else {
        digitalWrite(POWER_MODE_PIN, LOW);
        sleepState = true;
      }
    } else if ((hwTimer1.count > 2) && (hwTimer1.count < 4)) digitalWrite(POWER_MODE_PIN, HIGH);
    else {
      digitalWrite(POWER_MODE_PIN, LOW);
      sleepState = true;
    }
  } else {
    if ((hwTimer1.count > (intervalTime - TIME_TO_WAKEUP_SEC)) && (hwTimer1.count < intervalTime)) digitalWrite(POWER_MODE_PIN, HIGH);
    else {
      digitalWrite(POWER_MODE_PIN, LOW);
      sleepState = false;
    }
  }
}
