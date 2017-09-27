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

// include all required headers from sym_arrow library

#include "sym_arrow/exception.h"
#include "sym_arrow/functions/contexts.h"

#include "sym_arrow/nodes/expr.h"
#include "sym_arrow/nodes/scalar.h"
#include "sym_arrow/nodes/symbol.h"
#include "sym_arrow/nodes/add_expr.h"
#include "sym_arrow/nodes/mult_expr.h"
#include "sym_arrow/nodes/function_expr.h"
#include "sym_arrow/functions/expr_functions.h"
#include "sym_arrow/nodes/expr_visitor.h"
#include "sym_arrow/utils/timer.h"
