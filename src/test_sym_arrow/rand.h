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

#pragma once

namespace sym_arrow { namespace testing
{

// Initialize Mersenne twister with a seed.
void init_genrand(unsigned long s);

// Mersenne twister generator of unifomly distributed unsigned 32-bit integers.
unsigned long genrand_int32(void);

// Mersenne twister generator of unifomly distributed numbers on [0,1].
double genrand_real1(void);

// Mersenne twister generator of unifomly distributed numbers on [0,1).
double genrand_real2(void);

// Mersenne twister generator of unifomly distributed numbers on (0,1).
double genrand_real3(void);

// Standard normally distributed random variables.
double gen_norm();

};};