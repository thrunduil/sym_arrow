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

#include "sym_arrow/config.h"
#include "sym_arrow/nodes/expr.h"
#include "dag/dag.h"
#include "sym_arrow/func/process_scalar.h"
#include "sym_arrow/func/compound.h"

#include "sym_arrow/ast/ast.h"
#include "sym_arrow/functions/expr_functions.h"
#include "sym_arrow/functions/constructor_functions.h"
#include "sym_arrow/func/func_names.h"
#include "sym_arrow/functions/global_context_initializers.h"

namespace sym_arrow
{

//-------------------------------------------------------------------
//                  definitions of boolean functions
//-------------------------------------------------------------------

sym_arrow::expr sym_arrow::bool_eq(const expr& a, const expr& b)
{
    if (a.get_ptr()->isa<ast::scalar_rep>() == true 
        && b.get_ptr()->isa<ast::scalar_rep>() == true)
    {
        const value & av    = a.get_ptr()->static_cast_to<ast::scalar_rep>()->get_data();
        const value & bv    = b.get_ptr()->static_cast_to<ast::scalar_rep>()->get_data();

        bool cond   = (av == bv);

        if (cond == true)
            return scalar::make_one();
        else
            return scalar::make_zero();
    };

    return function(details::func_name::bool_eq(), {a, b});
};

sym_arrow::expr sym_arrow::bool_neq(const expr& a, const expr& b)
{
    if (a.get_ptr()->isa<ast::scalar_rep>() == true 
        && b.get_ptr()->isa<ast::scalar_rep>() == true)
    {
        const value & av    = a.get_ptr()->static_cast_to<ast::scalar_rep>()->get_data();
        const value & bv    = b.get_ptr()->static_cast_to<ast::scalar_rep>()->get_data();

        bool cond   = (av != bv);

        if (cond == true)
            return scalar::make_one();
        else
            return scalar::make_zero();
    };

    return function(details::func_name::bool_neq(), {a, b});
};

sym_arrow::expr sym_arrow::bool_gt(const expr& a, const expr& b)
{
    if (a.get_ptr()->isa<ast::scalar_rep>() == true 
        && b.get_ptr()->isa<ast::scalar_rep>() == true)
    {
        const value & av    = a.get_ptr()->static_cast_to<ast::scalar_rep>()->get_data();
        const value & bv    = b.get_ptr()->static_cast_to<ast::scalar_rep>()->get_data();

        bool cond   = (av > bv);

        if (cond == true)
            return scalar::make_one();
        else
            return scalar::make_zero();
    };

    return function(details::func_name::bool_gt(), {a, b});
};

sym_arrow::expr sym_arrow::bool_lt(const expr& a, const expr& b)
{
    if (a.get_ptr()->isa<ast::scalar_rep>() == true 
        && b.get_ptr()->isa<ast::scalar_rep>() == true)
    {
        const value & av    = a.get_ptr()->static_cast_to<ast::scalar_rep>()->get_data();
        const value & bv    = b.get_ptr()->static_cast_to<ast::scalar_rep>()->get_data();

        bool cond   = (av < bv);

        if (cond == true)
            return scalar::make_one();
        else
            return scalar::make_zero();
    };

    return function(details::func_name::bool_lt(), a, b);
};

sym_arrow::expr sym_arrow::bool_leq(const expr& a, const expr& b)
{
    if (a.get_ptr()->isa<ast::scalar_rep>() == true 
        && b.get_ptr()->isa<ast::scalar_rep>() == true)
    {
        const value & av    = a.get_ptr()->static_cast_to<ast::scalar_rep>()->get_data();
        const value & bv    = b.get_ptr()->static_cast_to<ast::scalar_rep>()->get_data();

        bool cond   = (av <= bv);

        if (cond == true)
            return scalar::make_one();
        else
            return scalar::make_zero();
    };

    return function(details::func_name::bool_leq(), a, b);
};

sym_arrow::expr sym_arrow::bool_geq(const expr& a, const expr& b)
{
    if (a.get_ptr()->isa<ast::scalar_rep>() == true 
        && b.get_ptr()->isa<ast::scalar_rep>() == true)
    {
        const value & av    = a.get_ptr()->static_cast_to<ast::scalar_rep>()->get_data();
        const value & bv    = b.get_ptr()->static_cast_to<ast::scalar_rep>()->get_data();

        bool cond   = (av >= bv);

        if (cond == true)
            return scalar::make_one();
        else
            return scalar::make_zero();
    };

    return function(details::func_name::bool_geq(), a, b);
};

sym_arrow::expr sym_arrow::bool_or(const expr& a, const expr& b)
{
    if (a.get_ptr()->isa<ast::scalar_rep>() == true 
        && b.get_ptr()->isa<ast::scalar_rep>() == true)
    {
        const value & av    = a.get_ptr()->static_cast_to<ast::scalar_rep>()->get_data();
        const value & bv    = b.get_ptr()->static_cast_to<ast::scalar_rep>()->get_data();
        value one           = value::make_one();

        bool cond   = (av == one) || (bv == one);

        if (cond == true)
            return scalar::make_one();
        else
            return scalar::make_zero();
    };

    return function(details::func_name::bool_or(), a, b);
};

sym_arrow::expr sym_arrow::bool_and(const expr& a, const expr& b)
{
    if (a.get_ptr()->isa<ast::scalar_rep>() == true 
        && b.get_ptr()->isa<ast::scalar_rep>() == true)
    {
        const value & av    = a.get_ptr()->static_cast_to<ast::scalar_rep>()->get_data();
        const value & bv    = b.get_ptr()->static_cast_to<ast::scalar_rep>()->get_data();
        value one           = value::make_one();

        bool cond   = (av == one) && (bv == one);

        if (cond == true)
            return scalar::make_one();
        else
            return scalar::make_zero();
    };

    return function(details::func_name::bool_and(), a, b);
};

sym_arrow::expr sym_arrow::bool_xor(const expr& a, const expr& b)
{
    if (a.get_ptr()->isa<ast::scalar_rep>() == true 
        && b.get_ptr()->isa<ast::scalar_rep>() == true)
    {
        const value & av    = a.get_ptr()->static_cast_to<ast::scalar_rep>()->get_data();
        const value & bv    = b.get_ptr()->static_cast_to<ast::scalar_rep>()->get_data();
        value one           = value::make_one();

        bool cond   = (av == one) ^ (bv == one);

        if (cond == true)
            return scalar::make_one();
        else
            return scalar::make_zero();
    };

    return function(details::func_name::bool_xor(), a, b);
};

sym_arrow::expr sym_arrow::bool_andnot(const expr& a, const expr& b)
{
    if (a.get_ptr()->isa<ast::scalar_rep>() == true 
        && b.get_ptr()->isa<ast::scalar_rep>() == true)
    {
        const value & av    = a.get_ptr()->static_cast_to<ast::scalar_rep>()->get_data();
        const value & bv    = b.get_ptr()->static_cast_to<ast::scalar_rep>()->get_data();
        value one           = value::make_one();

        bool cond   = !(av == one) && (bv == one);

        if (cond == true)
            return scalar::make_one();
        else
            return scalar::make_zero();
    };

    return function(details::func_name::bool_andnot(), a, b);
};

sym_arrow::expr sym_arrow::bool_not(const expr& a)
{
    if (a.get_ptr()->isa<ast::scalar_rep>() == true)
    {
        const value & av    = a.get_ptr()->static_cast_to<ast::scalar_rep>()->get_data();
        value one           = value::make_one();

        bool cond   = !(av == one);

        if (cond == true)
            return scalar::make_one();
        else
            return scalar::make_zero();
    };

    return function(details::func_name::bool_not(), a);
};

expr sym_arrow::if_then(const expr& cond, const expr& ex)
{
    if (cond.get_ptr()->isa<ast::scalar_rep>() == true)
    {
        const value & av    = cond.get_ptr()->static_cast_to<ast::scalar_rep>()->get_data();
        value one           = value::make_one();

        bool res            = (av == one);

        if (res == true)
            return ex;
        else
            return scalar::make_nan();
    };

    return function(details::func_name::if_then(), cond, ex);
}

expr sym_arrow::if_then_else(const expr& cond, const expr& ex_true, const expr& ex_false)
{
    if (cond.get_ptr()->isa<ast::scalar_rep>() == true)
    {
        const value & av    = cond.get_ptr()->static_cast_to<ast::scalar_rep>()->get_data();
        value one           = value::make_one();

        bool res            = (av == one);

        if (res == true)
            return ex_true;
        else
            return ex_false;
    };

    return function(details::func_name::if_then_else(), {cond, ex_true, ex_false});
}

//-------------------------------------------------------------------
//                  evalers of boolean functions
//-------------------------------------------------------------------

struct boolean_func_eval_init : function_evaler_static_initializer<boolean_func_eval_init>
{
    using evaler_type   = std::function<value (size_t n_args, const value* args)>;

    virtual void initialize(function_evaler& fe) override
    {
        fe.add_evaler(details::func_name::bool_eq(), 2, evaler_type(&eval_bool_eq));
        fe.add_evaler(details::func_name::bool_neq(), 2, evaler_type(&eval_bool_neq));
        fe.add_evaler(details::func_name::bool_leq(), 2, evaler_type(&eval_bool_leq));
        fe.add_evaler(details::func_name::bool_geq(), 2, evaler_type(&eval_bool_geq));
        fe.add_evaler(details::func_name::bool_lt(), 2, evaler_type(&eval_bool_lt));
        fe.add_evaler(details::func_name::bool_gt(), 2, evaler_type(&eval_bool_gt));

        fe.add_evaler(details::func_name::bool_or(), 2, evaler_type(&eval_bool_or));
        fe.add_evaler(details::func_name::bool_and(), 2, evaler_type(&eval_bool_and));
        fe.add_evaler(details::func_name::bool_xor(), 2, evaler_type(&eval_bool_xor));
        fe.add_evaler(details::func_name::bool_andnot(), 2, evaler_type(&eval_bool_andnot));

        fe.add_evaler(details::func_name::bool_not(), 1, evaler_type(&eval_bool_not));
    }

    static value eval_bool_eq(size_t n_args, const value* args)
    {
        (void)n_args;

        const value& av = args[0];
        const value& bv = args[1];

        bool cond   = (av == bv);

        if (cond == true)
            return value::make_one();
        else
            return value::make_zero();
    }
    
    static value eval_bool_neq(size_t n_args, const value* args)
    {
        (void)n_args;

        const value& av = args[0];
        const value& bv = args[1];

        bool cond   = (av != bv);

        if (cond == true)
            return value::make_one();
        else
            return value::make_zero();
    }

    static value eval_bool_leq(size_t n_args, const value* args)
    {
        (void)n_args;

        const value& av = args[0];
        const value& bv = args[1];

        bool cond   = (av <= bv);

        if (cond == true)
            return value::make_one();
        else
            return value::make_zero();
    }

    static value eval_bool_geq(size_t n_args, const value* args)
    {
        (void)n_args;

        const value& av = args[0];
        const value& bv = args[1];

        bool cond   = (av >= bv);

        if (cond == true)
            return value::make_one();
        else
            return value::make_zero();
    }

    static value eval_bool_lt(size_t n_args, const value* args)
    {
        (void)n_args;

        const value& av = args[0];
        const value& bv = args[1];

        bool cond   = (av < bv);

        if (cond == true)
            return value::make_one();
        else
            return value::make_zero();
    }

    static value eval_bool_gt(size_t n_args, const value* args)
    {
        (void)n_args;

        const value& av = args[0];
        const value& bv = args[1];

        bool cond   = (av > bv);

        if (cond == true)
            return value::make_one();
        else
            return value::make_zero();
    }

    static value eval_bool_or(size_t n_args, const value* args)
    {
        (void)n_args;

        value one       = value::make_one();
        const value& av = args[0];
        const value& bv = args[1];

        bool cond   = (av == one) || (bv == one);

        if (cond == true)
            return value::make_one();
        else
            return value::make_zero();
    }

    static value eval_bool_and(size_t n_args, const value* args)
    {
        (void)n_args;

        value one       = value::make_one();
        const value& av = args[0];
        const value& bv = args[1];

        bool cond   = (av == one) && (bv == one);

        if (cond == true)
            return value::make_one();
        else
            return value::make_zero();
    }

    static value eval_bool_xor(size_t n_args, const value* args)
    {
        (void)n_args;

        value one       = value::make_one();
        const value& av = args[0];
        const value& bv = args[1];

        bool cond   = (av == one) ^ (bv == one);

        if (cond == true)
            return value::make_one();
        else
            return value::make_zero();
    }

    static value eval_bool_andnot(size_t n_args, const value* args)
    {
        (void)n_args;

        value one       = value::make_one();
        const value& av = args[0];
        const value& bv = args[1];

        bool cond   = !(av == one) || (bv == one);

        if (cond == true)
            return value::make_one();
        else
            return value::make_zero();
    }

    static value eval_bool_not(size_t n_args, const value* args)
    {
        (void)n_args;

        value one       = value::make_one();
        const value& av = args[0];

        bool cond   = !(av == one);

        if (cond == true)
            return value::make_one();
        else
            return value::make_zero();
    }
};

//-------------------------------------------------------------------
//                  evalers of if functions
//-------------------------------------------------------------------

struct if_func_eval_init : function_evaler_static_initializer<if_func_eval_init>
{
    using evaler_type           = std::function<value (size_t n_args, const value* args)>;
    using partial_evaler_type   = std::function<expr (size_t n_args, const expr* args)>;

    virtual void initialize(function_evaler& fe) override
    {
        fe.add_evaler(details::func_name::if_then(), 2, evaler_type(&eval_if_then));
        fe.add_evaler(details::func_name::if_then_else(), 3, evaler_type(&eval_if_then_else));

        fe.add_partial_evaler(details::func_name::if_then(), 2, 
                partial_evaler_type(&partial_eval_if_then));
        fe.add_partial_evaler(details::func_name::if_then_else(), 3, 
                partial_evaler_type(&partial_eval_if_then_else));
    }

    static value eval_if_then(size_t n_args, const value* args)
    {
        (void)n_args;

        const value& cond   = args[0];
        const value& ex     = args[1];

        bool res            = (cond == value::make_one());

        if (res == true)
            return ex;
        else
            return value::make_nan();
    }

    static value eval_if_then_else(size_t n_args, const value* args)
    {
        (void)n_args;

        const value& cond   = args[0];
        const value& ex_t   = args[1];
        const value& ex_f   = args[2];

        bool res            = (cond == value::make_one());

        if (res == true)
            return ex_t;
        else
            return ex_f;
    }

    static expr partial_eval_if_then(size_t n_args, const expr* args)
    {
        (void)n_args;

        const expr& cond    = args[0];
        const expr& ex      = args[1];

        if (cond.get_expr_handle()->isa<ast::scalar_rep>() == false)
            return expr();

        const value& cond_v = cond.get_expr_handle()->static_cast_to<ast::scalar_rep>()
                                ->get_data();

        bool res            = (cond_v == value::make_one());

        if (res == true)
            return ex;
        else
            return scalar::make_nan();
    }

    static expr partial_eval_if_then_else(size_t n_args, const expr* args)
    {
        (void)n_args;

        const expr& cond    = args[0];
        const expr& ex_t    = args[1];
        const expr& ex_f    = args[2];

        if (cond.get_expr_handle()->isa<ast::scalar_rep>() == false)
            return expr();

        const value& cond_v = cond.get_expr_handle()->static_cast_to<ast::scalar_rep>()
                                ->get_data();

        bool res            = (cond_v == value::make_one());

        if (res == true)
            return ex_t;
        else
            return ex_f;
    }
};

//-------------------------------------------------------------------
//                  diff rules of if functions
//-------------------------------------------------------------------

struct func_diff_init : diff_context_static_initializer<func_diff_init>
{
    virtual void initialize(diff_context& dc) override
    {
        diff_rule_if_then(dc);
        diff_rule_if_then_else(dc);
    }

    void diff_rule_if_then(diff_context& dc)
    {
        symbol cond         = symbol("cond");
        symbol ex           = symbol("ex");        
        symbol syms[]       = {cond, ex};

        scalar one          = scalar::make_one();
        expr if_then_1      = if_then(cond, one);

        dc.add_diff_rule(details::func_name::if_then(), 2, syms, 0, one, false);
        dc.add_diff_rule(details::func_name::if_then(), 2, syms, 1, if_then_1, true);        
    };

    void diff_rule_if_then_else(diff_context& dc)
    {
        symbol cond         = symbol("cond");
        symbol ex_true      = symbol("ex_true");
        symbol ex_false     = symbol("ex_false");
        symbol syms[]       = {cond, ex_true, ex_false};

        scalar one          = scalar::make_one();
        scalar zero         = scalar::make_zero();

        expr ite_1          = if_then_else(cond, one, zero);
        expr ite_2          = if_then_else(cond, zero, one);

        dc.add_diff_rule(details::func_name::if_then_else(), 3, syms, 0, one, false);
        dc.add_diff_rule(details::func_name::if_then_else(), 3, syms, 1, ite_1, true);
        dc.add_diff_rule(details::func_name::if_then_else(), 3, syms, 2, ite_2, true);
    };
};

};