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

#include "sym_arrow/config.h"
#include "sym_arrow/nodes/expr.h"
#include "dag/dag.h"
#include "process_scalar.h"
#include "compound.h"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/functions/expr_functions.h"

namespace sym_arrow { namespace details
{

struct do_abs : public process_scalar1<do_abs>
{
    using tag_type      = sym_arrow::ast::term_tag;
    using scalar_handle = const ast::scalar_rep*;
    using expr_handle   = const ast::expr_base*;

    static void eval(expr& ret, const value& a)
    {
        ret = expr(abs(a));
    }

    static void eval(expr& ret, ast::expr_handle a)
    {
        return compound::mult_build(ret, expr(a), value::make_one());
    };
};

}};

sym_arrow::expr sym_arrow::abs(const expr& a)
{
    expr ret;
    details::do_abs().make(ret, a);
    return ret;
};

sym_arrow::expr sym_arrow::abs(expr&& a0)
{
    expr a(std::move(a0));

    a.get_ptr()->make_temporary(true);
    
    expr ret = abs(a);
    
    a.get_ptr()->make_temporary(false);
    return ret;
};

size_t sym_arrow::hash_value(const expr& ex)
{
    size_t seed = (size_t)ex.get_ptr().get();    
    return seed;
};

size_t sym_arrow::hash_value(const identifier& ex)
{
    size_t seed = (size_t)ex.get_ptr().get();    
    return seed;
};
