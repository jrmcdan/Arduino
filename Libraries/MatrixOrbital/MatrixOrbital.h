/*
 * Copyright, 2011 Radek Wierzbicki
 *  
 * This file is part of MatrixOrbital.
 *
 * MatrixOrbital is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MatrixOrbital is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with MatrixOrbital.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MatrixOrbital_h
#define MatrixOrbital_h

#include "WProgram.h"
#include "Print.h"


class MatrixOrbital : public Print {
    public:
        MatrixOrbital(int rows, int columns);
        void turnAutoScrollOn();
        void turnAutoScrollOff();
        void turnAutoLineWrapOn();
        void turnAutoLineWrapOff();
        void turnBlockCursorOn();
        void turnBlockCursorOff();
        void turnUnderlineCursorOn();
        void turnUnderlineCursorOff();
        void clear();
        void newLine();
        void cursorHome();
        void cursorSet(int x, int y);
        void backspace();
        void cursorLeft();
        void cursorRight();
        void setContrast(int contrast);
        void backlightOn(int minutes);
        void backlightOff();
    private:
        virtual void write(uint8_t b);
        int _rows;
        int _columns;
};

#endif

