/*
 * Copyright, 2011 Radek Wierzbicki
 *  
 * This file is part of ChronodotLib.
 *
 * ChronodotLib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ChronodotLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with ChronodotLib. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Wire.h"
#include "Chronodot.h"

#define DS3231_CTRL_ID 0x68 

Chronodot::Chronodot() {
    Wire.begin();

    timeDateBCD.seconds = 0;
    timeDateBCD.minutes = 0;
    timeDateBCD.hours   = 0;
    timeDateBCD.weekDay = 0;
    timeDateBCD.day     = 0;
    timeDateBCD.month   = 0;
    timeDateBCD.year    = 0;

    timeDate.seconds = 0;
    timeDate.minutes = 0;
    timeDate.hours   = 0;
    timeDate.weekDay = 0;
    timeDate.day     = 0;
    timeDate.month   = 0;
    timeDate.year    = 0;

    temperature = 0;
}


void Chronodot::readTimeDate() {
    Wire.beginTransmission(DS3231_CTRL_ID);
    Wire.send(0x00);
    Wire.endTransmission();

    Wire.requestFrom(DS3231_CTRL_ID, 7);
    timeDateBCD.seconds = Wire.receive() & 0b01111111; // ignore bit 7
    timeDateBCD.minutes = Wire.receive();
    timeDateBCD.hours   = Wire.receive() & 0b00111111; // ignore bit 6 and 7
    timeDateBCD.weekDay = Wire.receive();
    timeDateBCD.day     = Wire.receive();
    timeDateBCD.month   = Wire.receive() & 0b01111111; // ignore bit 7
    timeDateBCD.year    = Wire.receive();

    timeDate.seconds = bcd2dec(timeDateBCD.seconds);
    timeDate.minutes = bcd2dec(timeDateBCD.minutes);
    timeDate.hours   = bcd2dec(timeDateBCD.hours);
    timeDate.weekDay = bcd2dec(timeDateBCD.weekDay);
    timeDate.day     = bcd2dec(timeDateBCD.day);
    timeDate.month   = bcd2dec(timeDateBCD.month);
    timeDate.year    = bcd2dec(timeDateBCD.year);
}


void Chronodot::readTime() {
    Wire.beginTransmission(DS3231_CTRL_ID);
    Wire.send(0x00);
    Wire.endTransmission();

    Wire.requestFrom(DS3231_CTRL_ID, 3);
    timeDateBCD.seconds = Wire.receive() & 0b01111111; // ignore bit 7
    timeDateBCD.minutes = Wire.receive();
    timeDateBCD.hours   = Wire.receive() & 0b00111111; // ignore bit 6 and 7

    timeDate.seconds = bcd2dec(timeDateBCD.seconds);
    timeDate.minutes = bcd2dec(timeDateBCD.minutes);
    timeDate.hours   = bcd2dec(timeDateBCD.hours);
}


void Chronodot::readDate() {
    Wire.beginTransmission(DS3231_CTRL_ID);
    Wire.send(0x03);
    Wire.endTransmission();

    Wire.requestFrom(DS3231_CTRL_ID, 4);
    timeDateBCD.weekDay = Wire.receive();
    timeDateBCD.day     = Wire.receive();
    timeDateBCD.month   = Wire.receive() & 0b01111111; // ignore bit 7
    timeDateBCD.year    = Wire.receive();

    timeDate.weekDay = bcd2dec(timeDateBCD.weekDay);
    timeDate.day     = bcd2dec(timeDateBCD.day);
    timeDate.month   = bcd2dec(timeDateBCD.month);
    timeDate.year    = bcd2dec(timeDateBCD.year);
}


void Chronodot::readTemperature() {
    //temp registers (11h-12h) get updated automatically every 64s
    Wire.beginTransmission(DS3231_CTRL_ID);
    Wire.send(0x11);
    Wire.endTransmission();
    Wire.requestFrom(DS3231_CTRL_ID, 2);

    uint8_t tempInt = Wire.receive() & 0b01111111; // ignore bit 7
    uint8_t tempFraction = (Wire.receive() >> 6) * 25; // only bit 7 and 8

    temperature = tempInt + (tempFraction / 100);

    temperatureIntBCD      = dec2bcd(tempInt);
    temperatureFractionBCD = dec2bcd(tempFraction);
}


void Chronodot::setSQW(int frequency) {
    // Frequency is stored in register 0x0e in bit 3 and 4
    Wire.beginTransmission(DS3231_CTRL_ID);
    Wire.send(0x0e);
    Wire.endTransmission();
    Wire.requestFrom(DS3231_CTRL_ID, 1);
    uint8_t register0E = Wire.receive();
  
    if (frequency == 1) {
        // clear bits 3 and 4
        register0E &= ~(1 << 3);
        register0E &= ~(1 << 4);
    } else if (frequency == 1024) {
        // bit 3 is 1, bit 4 is 0
        register0E |= 1 << 3;
        register0E &= ~(1 << 4);
    } else if (frequency == 4096) {
        // bit 3 is 0, bit 4 is 1
        register0E &= ~(1 << 3);
        register0E |= 1 << 4;
    } else if (frequency == 8192) {
        // set bits 3 and 4
        register0E |= 1 << 3;
        register0E |= 1 << 4;
    }
  
    // put the value of the register back
    Wire.beginTransmission(DS3231_CTRL_ID);
    Wire.send(0x0e);
    Wire.send(register0E);
    Wire.endTransmission();
}


void Chronodot::setTimeDate(timeDateElements &tE) {
    Wire.beginTransmission(DS3231_CTRL_ID);
    Wire.send(0x00);
    Wire.send(dec2bcd(tE.seconds));
    Wire.send(dec2bcd(tE.minutes));
    Wire.send(dec2bcd(tE.hours));
    Wire.send(dec2bcd(tE.weekDay));
    Wire.send(dec2bcd(tE.day));
    Wire.send(dec2bcd(tE.month));
    Wire.send(dec2bcd(tE.year));
    Wire.endTransmission();
}


void Chronodot::setTime(timeDateElements &tE) {
    Wire.beginTransmission(DS3231_CTRL_ID);
    Wire.send(0x00);
    Wire.send(dec2bcd(tE.seconds));
    Wire.send(dec2bcd(tE.minutes));
    Wire.send(dec2bcd(tE.hours));
    Wire.endTransmission();
}


void Chronodot::setDate(timeDateElements &tE) {
    Wire.beginTransmission(DS3231_CTRL_ID);
    Wire.send(0x03);
    Wire.send(dec2bcd(tE.weekDay));
    Wire.send(dec2bcd(tE.day));
    Wire.send(dec2bcd(tE.month));
    Wire.send(dec2bcd(tE.year));
    Wire.endTransmission();
}


uint8_t Chronodot::dec2bcd(uint8_t num) {
    return ((num / 10) << 4) + (num % 10);
}


uint8_t Chronodot::bcd2dec(uint8_t num) {
    return (((num >> 4) & 0b00001111) * 10) + (num & 0b00001111);
}

