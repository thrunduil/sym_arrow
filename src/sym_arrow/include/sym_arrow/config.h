/*
*  This file is a part of sym_arrow library.
*
*  Copyright (c) Pawe³ Kowal 2017 - 2021
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

#ifdef _MSC_VER
    #ifdef  SYM_ARROW_EXPORTS
        #define SYM_ARROW_EXPORT _declspec(dllexport)
    #else
        #define SYM_ARROW_EXPORT _declspec(dllimport)
    #endif
#else
    #define SYM_ARROW_EXPORT 
#endif

#define SYM_ARROW_FORCE_INLINE __forceinline

#ifdef _DEBUG
    // when this macro is defined, then additional debugging routines
    // are enabled
    #define SYM_ARROW_DEBUG_EXPR

    // when this macro is defined, then additional memory debugging routines
    // are enabled
    #define SYM_ARROW_DEBUG_MEMORY
#endif

#define SYM_ARROW_VALUE_DOUBLE  0
#define SYM_ARROW_VALUE_GSLI    1
#define SYM_ARROW_VALUE_MP      2

// if this macro is set to SYM_ARROW_VALUE_DOUBLE, then value is represented 
// by the double type; 
// if this macro is set to SYM_ARROW_VALUE_GSLI, then value is represented 
// by the gsli_double type
// if this macro is set to SYM_ARROW_VALUE_MP, then value is represented 
// by the mp_float type (multiprecision floating point type)
// precision ordering: 
//      precision(gsli_double) <= precision(double) < precision(mp_float)
// exponent range ordering: 
//      exp(double) < exp(mp_float) < exp(gsli_double)
// performance ordering: 
//      perf(mp_Float) < perf(gsli_double) <= perf(double)
#define SYM_ARROW_VALUE_TYPE SYM_ARROW_VALUE_MP

