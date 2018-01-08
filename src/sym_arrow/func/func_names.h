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
#include "sym_arrow/nodes/expr.h"

namespace sym_arrow { namespace details { namespace func_name
{

symbol  bool_eq();
symbol  bool_neq();
symbol  bool_lt();
symbol  bool_gt();
symbol  bool_leq();
symbol  bool_geq();

symbol  bool_or();
symbol  bool_and();
symbol  bool_xor();
symbol  bool_andnot();
symbol  bool_not();

symbol  if_then();
symbol  if_then_else();

}}}