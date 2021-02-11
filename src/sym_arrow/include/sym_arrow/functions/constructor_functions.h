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

#include "sym_arrow/nodes/expr.h"
#include "sym_arrow/nodes/type.h"
#include "sym_arrow/functions/contexts.h"

namespace sym_arrow
{

// create expression for a string representation
expr SYM_ARROW_EXPORT   parse(const std::string& expression_string);

//parse a definition
void SYM_ARROW_EXPORT   parse_def(const std::string& def_string);

//parse a symbol
symbol SYM_ARROW_EXPORT parse_sym(const std::string& sym_string);

// make function 
expr SYM_ARROW_EXPORT   function(const identifier& sym);
expr SYM_ARROW_EXPORT   function(const identifier& sym, const expr& a1);
expr SYM_ARROW_EXPORT   function(const identifier& sym, const expr& a1, const expr& a2);
expr SYM_ARROW_EXPORT   function(const identifier& sym, std::initializer_list<expr> args);
expr SYM_ARROW_EXPORT   function(const identifier& sym, const std::vector<expr>& args);
expr SYM_ARROW_EXPORT   function(const identifier& sym, const expr* arg, size_t n);

// make symbol with name sym, indexers args, type t and const flag is_const;
// if t is not initialized, then type is infered based on existing declarations
symbol SYM_ARROW_EXPORT make_symbol(const identifier& sym, const type& t = type());
symbol SYM_ARROW_EXPORT make_symbol(const identifier& sym, const expr& a1,
                            const type& t = type());
symbol SYM_ARROW_EXPORT make_symbol(const identifier& sym, const expr& a1, const expr& a2,
                            const type& t = type());
symbol SYM_ARROW_EXPORT make_symbol(const identifier& sym, std::initializer_list<expr> args,
                            const type& t = type());
symbol SYM_ARROW_EXPORT make_symbol(const identifier& sym, const std::vector<expr>& args,
                            const type& t = type());
symbol SYM_ARROW_EXPORT make_symbol(const identifier& sym, const expr* arg, size_t n,
                            const type& t = type());

};

