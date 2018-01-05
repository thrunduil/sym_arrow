/*
*  Morfa programming language.
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

#include "sym_arrow/config.h"

#if SYM_ARROW_VALUE_TYPE == SYM_ARROW_VALUE_MP
    #include "sym_arrow/nodes/value_mp.h"    
#elif SYM_ARROW_VALUE_TYPE == SYM_ARROW_VALUE_GSLI
    #include "sym_arrow/nodes/value_gsli.h"
#elif SYM_ARROW_VALUE_TYPE == SYM_ARROW_VALUE_DOUBLE
    #include "sym_arrow/nodes/value_double.h"    
#else
    #error "invalid value tye"
#endif
