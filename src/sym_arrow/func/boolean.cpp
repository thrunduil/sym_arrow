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
#include "process_scalar.h"
#include "compound.h"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/functions/expr_functions.h"
#include "sym_arrow/func/func_names.h"
#include "sym_arrow/functions/global_context_initializers.h"

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

    return function(details::func_name::bool_eq(), a, b);
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

    return function(details::func_name::bool_neq(), a, b);
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

    return function(details::func_name::bool_gt(), a, b);
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

namespace sym_arrow
{

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

};