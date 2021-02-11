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

#include "sym_arrow/ast/ast.h"
#include "sym_arrow/fwd_decls.h"

namespace sym_arrow { namespace details
{

namespace sa = sym_arrow::ast;

// return true if type of ex is constant, i.e. ex cannot change after 
// substitution; it is assumed, that ex is cannonized
bool        is_constant(const expr& ex);
bool        is_constant(const symbol& ex);

// return type of an expression
identifier  get_type(const expr& ex);

// check if there is a convertion from type 'from' to type 'to'
bool        is_convertible(const identifier& from, const identifier& to);

}};