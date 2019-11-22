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

#include "sym_arrow/functions/expr_functions.h"
#include "sym_arrow/functions/constructor_functions.h"
#include "sym_arrow/func/func_names.h"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/functions/global_context_initializers.h"
#include "sym_arrow/sema/typing.h"
#include "sym_arrow/error/sema_error.h"
#include "sym_arrow/sema/symbol_table_impl.h"
#include "sym_arrow/ast/symbol_context_data.h"

namespace sym_arrow
{

namespace sd = sym_arrow::details;
namespace se = sym_arrow::error;

expr sym_arrow::function(const identifier& sym)
{
    return sym_arrow::function(sym, nullptr, 0);
};

expr sym_arrow::function(const identifier& sym, const expr& arg1)
{
    expr args[] = {arg1};
    return sym_arrow::function(sym, args, 1);
};

expr sym_arrow::function(const identifier& sym, const expr& arg1, const expr& arg2)
{
    expr args[] = {arg1, arg2};
    return sym_arrow::function(sym, args, 2);
};

expr sym_arrow::function(const identifier& sym, const std::vector<expr>& arg)
{
    return sym_arrow::function(sym, arg.data(), arg.size());
};

expr sym_arrow::function(const identifier& sym, std::initializer_list<expr> arg)
{
    return sym_arrow::function(sym, arg.begin(), arg.size());
};

expr sym_arrow::function(const identifier& sym, const expr* arg, size_t n)
{
    for (size_t i = 0; i < n; ++i)
        arg[i].cannonize(do_cse_default);

    // TODO
    //bool defined        = sd::sym_table_impl::get()->get_function_definition(sym, arg, n);

    {
        expr res;
        bool evaled         = global_function_evaler()
                                .eval_function(sym, arg, n, res);

        if (evaled == true)
            return res;
    }

    using info              = ast::function_rep_info;
    info f_info             = info(sym.get_ptr().get(), n, arg);

    ast::expr_ptr ep        = ast::function_rep::make(f_info);
    return expr(ep);
};

symbol sym_arrow::make_symbol(const identifier& sym, const type& t)
{
    return sym_arrow::make_symbol(sym, nullptr, 0, t);
};

symbol sym_arrow::make_symbol(const identifier& sym, const expr& arg1, const type& t)
{
    expr args[] = {arg1};
    return sym_arrow::make_symbol(sym, args, 1, t);
};

symbol sym_arrow::make_symbol(const identifier& sym, const expr& arg1, const expr& arg2,
                        const type& t)
{
    expr args[] = {arg1, arg2};
    return sym_arrow::make_symbol(sym, args, 2, t);
};

symbol sym_arrow::make_symbol(const identifier& sym, const std::vector<expr>& arg, 
                        const type& t)
{
    return sym_arrow::make_symbol(sym, arg.data(), arg.size(), t);
};

symbol sym_arrow::make_symbol(const identifier& sym, std::initializer_list<expr> arg,
                        const type& t)
{
    return sym_arrow::make_symbol(sym, arg.begin(), arg.size(), t);
};

symbol sym_arrow::make_symbol(const identifier& sym, const expr* arg, size_t n,
                        const type& t)
{
    for (size_t i = 0; i < n; ++i)
        arg[i].cannonize(do_cse_default);

    using info          = ast::symbol_info;

    std::vector<identifier> def_args;
    type                    t_def;

    bool defined        = sd::sym_table_impl::get()->get_symbol_definition(sym, def_args, t_def);

    // TODO: simplify this 
    if (defined == false)
    {
        if (t.is_null() == true)
            t_def       = sym_dag::dag_context<ast::unique_nodes_tag>::get()
                            .get_context_data().default_type();
        else
            t_def       = t;

        if (n == 0)
            sd::sym_table_impl::get()->define_symbol(sym, def_args, t_def);
        else
            se::sema_error().undefined_symbol(sym, n);
    }
    else
    {
        // check indices
        if (def_args.size() != n)
        {
            error::sema_error().invalid_symbol_args(sym, n, def_args, t_def);
        }
        else
        {
            for (size_t i = 0; i < n; ++i)
            {
                identifier t_arg    = details::get_type(arg[i]);
                bool match          = details::is_convertible(t_arg, def_args[i]);

                if (match == false)
                    error::sema_error().invalid_symbol_arg(sym, i, t_arg, def_args, t_def);
            }
        }
    }
    
    type t_fin;

    if (t.is_null() == true)
        t_fin           = t_def;
    else
        t_fin           = t;
    
    info f_info         = info(sym.get_ptr().get(), n, arg, &t_fin);

    ast::symbol_ptr ep  = ast::symbol_rep::make(f_info);
    symbol ret          = symbol(ep);

    if (t_fin.type_name() != t_def.type_name())
        se::sema_error().invalid_explicit_symbol_type(sym, def_args, t_def, t_fin);

    if (t_def.is_const() != t_fin.is_const())
    {
        if (t_def.is_const() == true)
        {
            // if existing definition defines const symbol and local definition
            // defins nonconst symbol, then const symbol is created
        }
        else
        {
            if (t_fin.is_const() == true)
                se::sema_error().invalid_symbol_nonconst_def(sym, def_args, t_def);
        }
    }
    
    return ret;
};

};
