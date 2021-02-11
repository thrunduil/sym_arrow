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
#include "sym_arrow/functions/contexts.h"

namespace sym_arrow
{

// differentiation with respect a symbol sym
expr SYM_ARROW_EXPORT   diff(const expr& ex, const symbol& sym, 
                            const diff_context& dif = global_diff_context());

// n-time differentiation with respect a symbol sym
expr SYM_ARROW_EXPORT   diff(const expr& ex, const symbol& sym, int n,
                            const diff_context& dif = global_diff_context());

// substitute a symbol sym by an expression sub
expr SYM_ARROW_EXPORT   subs(const expr& ex, const symbol& sym, const expr& sub);

// substitute symbols in expression ex by substitutions
// stored in sub context
expr SYM_ARROW_EXPORT   subs(const expr& ex, const subs_context& sub);

// evaluate and expression
value SYM_ARROW_EXPORT  eval(const expr& ex, const data_provider& dp);

// perform simplifications
expr SYM_ARROW_EXPORT   simplify(const expr& ex);

// expand expression ex into the Taylor series around the point x0 with respect to
// symbol x:
//      ex = c0 + sum_{i=1}^order ci * (sym - x0)^i
// where c0 = ex(x0), ci = 1/i! * diff(ex, x, i)(x0), for i = 1,..., order
// and e(x0) denotes substitution subs(e, x, x0); coefficients are appended to the
// vector coef in order [c0, c1, ..., c_order];
// note that this function does not compute limits lim_{x -> x0} e, therefore may
// return NaN values even if given Taylor series expansion exists
void SYM_ARROW_EXPORT   taylor_coef(const expr& ex, const symbol& x, const expr& x0,
                            int order, std::vector<expr>& coef);
void SYM_ARROW_EXPORT   taylor_coef(const expr& ex, const symbol& x, const value& x0,
                            int order, std::vector<expr>& coef);


// function evaluates to 1 if expressions x and y are equal as expressions
// and to 0 is x and y are constant (cannot change after substitution) and 
// different; notice the difference to bool_eq function, which evaluates to
// value only when both x and y evaluates to value
expr SYM_ARROW_EXPORT   delta(const expr& x, const expr& y);

};

