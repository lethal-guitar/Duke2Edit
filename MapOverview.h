/** Copyright 2023 Nikolai Wuttke-Hohendorf
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the “Software”),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */


#ifndef MAPOVERVIEW_H
#define MAPOVERVIEW_H

#include <windows.h>

const RGBQUAD TILECOLOR_NULL = {0x66, 0x66, 0x66, 0};
const RGBQUAD TILECOLOR_BG = {0x88, 0x88, 0x88, 0};
const RGBQUAD TILECOLOR_CLIMB = {0x00, 0xAA, 0xAA};
const RGBQUAD TILECOLOR_SOLID = {0xDD, 0xDD, 0xDD, 0};

BOOL CALLBACK
  MapOverviewDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

#endif