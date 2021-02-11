/*
*  This file is a part of sym_arrow library.
*
*  Copyright (c) Pawe� Kowal 2017 - 2021
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

identifier  bool_eq();
identifier  bool_neq();
identifier  bool_lt();
identifier  bool_gt();
identifier  bool_leq();
identifier  bool_geq();

identifier  bool_or();
identifier  bool_and();
identifier  bool_xor();
identifier  bool_andnot();
identifier  bool_not();

identifier  if_then();
identifier  if_then_else();
identifier  delta();

}}}