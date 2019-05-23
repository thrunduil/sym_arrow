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
#include "sym_arrow/func/func_names.h"
#include "sym_arrow/functions/constructor_functions.h"
#include "sym_arrow/functions/expr_functions.h"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/sema/typing.h"

#include "sym_arrow/functions/global_context_initializers.h"

namespace sym_arrow
{

namespace sd = sym_arrow::details;

expr sym_arrow::delta(const expr& x, const expr& y)
{
    if (x == y)
        return scalar::make_one();

    bool is_const1      = sd::is_constant(x);
    bool is_const2      = sd::is_constant(y);

    if (is_const1 == true && is_const2 == true)
        return ast::scalar_rep::make_zero();

    return function(details::func_name::delta(), x, y);
}

//-------------------------------------------------------------------
//                  evalers
//-------------------------------------------------------------------

struct delta_eval_init : function_evaler_static_initializer<delta_eval_init>
{
    using evaler_type           = std::function<value (size_t n_args, const value* args)>;
    using partial_evaler_type   = std::function<expr (size_t n_args, const expr* args)>;

    virtual void initialize(function_evaler& fe) override
    {
        fe.add_evaler(details::func_name::delta(), 2, evaler_type(&eval_delta));

        fe.add_partial_evaler(details::func_name::delta(), 2, 
                partial_evaler_type(&partial_eval_delta));
    }

    static value eval_delta(size_t n_args, const value* args)
    {
        (void)n_args;

        const value& v1     = args[0];
        const value& v2     = args[1];

        bool res            = (v1 == v2);

        if (res == true)
            return value::make_one();
        else
            return value::make_zero();
    }

    static expr partial_eval_delta(size_t n_args, const expr* args)
    {
        (void)n_args;

        const expr& x       = args[0];
        const expr& y       = args[1];

        if (x == y)
            return scalar::make_one();

        bool is_const1      = sd::is_constant(x);
        bool is_const2      = sd::is_constant(y);

        if (is_const1 == true && is_const2 == true)
            return ast::scalar_rep::make_zero();

        return expr();
    }
};

//-------------------------------------------------------------------
//                  diff rules
//-------------------------------------------------------------------

struct delta_diff_init : diff_context_static_initializer<delta_diff_init>
{
    virtual void initialize(diff_context& dc) override
    {
        diff_rule_delta(dc);
    }

    void diff_rule_delta(diff_context& dc)
    {
        symbol syms[]       = {symbol("x"), symbol("y")};

        scalar zero         = scalar::make_zero();

        dc.add_diff_rule(details::func_name::delta(), 2, syms, 0, zero, true);
        dc.add_diff_rule(details::func_name::delta(), 2, syms, 1, zero, true);
    };
};

};