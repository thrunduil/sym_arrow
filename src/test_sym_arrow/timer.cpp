/* 
 *  This file is a part of sym_arrow library.
 *
 *  Copyright (c) Pawe³ Kowal 2017
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "timer.h"

#include <windows.h>

namespace sym_arrow { namespace testing 
{

static bool tic_started = false;
static __int64    tic_int64;

void testing::tic(void)
{
    tic_started = true;
    QueryPerformanceCounter((LARGE_INTEGER*) &tic_int64);
}

double testing::toc(void)
{
    double t;
    __int64 toc_int64, fr_int64;

    if (tic_started)
    {
        tic_started = false;
        QueryPerformanceCounter((LARGE_INTEGER*) &toc_int64);
        QueryPerformanceFrequency((LARGE_INTEGER*) &fr_int64);
        t = (double) (toc_int64 - tic_int64) / (double) fr_int64;
        return t;
    }

    return 0.;
}

}};