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

#ifdef _MSC_VER
    #ifdef  SYM_DAG_EXPORTS
        #define SYM_DAG_EXPORT __declspec(dllexport)
    #else
        #define SYM_DAG_EXPORT __declspec(dllimport)
    #endif
#else
    #define SYM_DAG_EXPORT 
#endif

// compiler specifier to force function inlining
#define SYM_DAG_FORCE_INLINE __forceinline

// maximum number of codes that can be set in 
// dag_tag_traits<Some_tag>::number_codes
#define SYM_DAG_MAX_NUMBER_CODES 10

#ifdef _DEBUG

    // when this macro is defined as 1, then additional memory debugging routines
    // are enabled
    #define SYM_DAG_DEBUG_MEMORY 1

    // when this macro is definde as 1, then virtual destructor is added
    // to dag_item_base class; this is useful for debugging purpose only
    #define SYM_DAG_POLYMORPHIC 1

    // when this macro is defined as 1, then additional debugging routines
    // are enabled
    #define SYM_DAG_DEBUG_TERMS 1

#else

    // when this macro is defined as 1, then additional memory debugging routines
    // are enabled
    #define SYM_DAG_DEBUG_MEMORY 0

    // when this macro is definde as 1, then virtual destructor is added
    // to dag_item_base class; this is useful for debugging purpose only
    #define SYM_DAG_POLYMORPHIC 0

    // when this macro is defined as 1, then additional debugging routines
    // are enabled
    #define SYM_DAG_DEBUG_TERMS 0

#endif