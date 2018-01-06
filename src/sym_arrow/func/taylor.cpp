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

#include "sym_arrow/functions/sym_functions.h"
#include "sym_arrow/functions/expr_functions.h"
#include "sym_arrow/ast/ast_tags.h"

namespace sym_arrow
{

void sym_arrow::taylor_coef(const expr& ex, const symbol& sym, const value& x0,
                            int order, std::vector<expr>& coef)
{
    return taylor_coef(ex, sym, expr(x0), order, coef);
}

void sym_arrow::taylor_coef(const expr& ex_in, const symbol& sym, const expr& x0,
                            int order, std::vector<expr>& coef)
{
    order           = std::max(order, 0);
    expr ex         = ex_in;
    value fact      = value::make_one();

    coef.reserve(coef.size() + order + 1);
        
    for (int i = 0; i < order; ++i)
    {
        expr res    = subs(ex, sym, x0);
        res         = res / expr(fact);

        coef.push_back(res);

        //TODO: context, limits
        ex          = diff(ex, sym);
        fact        = fact * value::make_value(double(i + 1));
    }

    {
        expr res    = subs(ex, sym, x0);
        res         = res / fact;
        coef.push_back(res);
    }
}

};