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

#include "sym_arrow/nodes/symbol.inl"
#include "sym_arrow/ast/symbol_rep.h"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/functions/expr_functions.h"
#include "sym_arrow/utils/stack_array.h"
#include "sym_arrow/sema/symbol_table_impl.h"
#include "sym_arrow/sema/typing.h"
#include "sym_arrow/error/sema_error.h"

namespace sym_arrow 
{

namespace sd = sym_arrow::details;
namespace se = sym_arrow::error;

//-------------------------------------------------------------------
//                  identifier
//-------------------------------------------------------------------
identifier::~identifier()
{};

identifier::identifier(const char* name, size_t num_char)
    :m_expr()
{
    ast::identifier_ptr n = ast::identifier_rep::make(ast::identifier_info(name, num_char));
    m_expr = n;

    if (num_char == 0 || name[0] == '$')
        throw std::runtime_error("invalid symbol name");
};

const char* identifier::get_name() const
{
    return get_ptr()->get_name();
}

size_t identifier::get_base_symbol_code() const
{
    return get_ptr()->get_base_symbol_code();
};

expr identifier::operator()() const
{
    return function(*this);
}

expr identifier::operator()(const expr& x1) const
{
    return sym_arrow::function(*this, x1);
}

expr identifier::operator()(const expr& x1, const expr& x2) const
{
    return sym_arrow::function(*this, x1, x2);
}

expr identifier::operator()(std::initializer_list<expr> ex) const
{
    return sym_arrow::function(*this, ex);
}

expr identifier::operator()(const std::vector<expr>& ex) const
{
    return sym_arrow::function(*this, ex);
}

symbol identifier::index(const identifier& t) const
{
    return sym_arrow::make_symbol(*this, t);
}

symbol identifier::index(const expr& x1, const identifier& t) const
{
    return sym_arrow::make_symbol(*this, x1, t);
}

symbol identifier::index(const expr& x1, const expr& x2, const identifier& t) const
{
    return sym_arrow::make_symbol(*this, x1, x2, t);
}

symbol identifier::index(std::initializer_list<expr> ex, const identifier& t) const
{
    return sym_arrow::make_symbol(*this, ex, t);
}

symbol identifier::index(const std::vector<expr>& ex, const identifier& t) const
{
    return sym_arrow::make_symbol(*this, ex, t);
}

symbol sym_arrow::make_symbol(const identifier& sym, const identifier& t)
{
    return sym_arrow::make_symbol(sym, nullptr, 0, t);
};

symbol sym_arrow::make_symbol(const identifier& sym, const expr& arg1, const identifier& t)
{
    expr args[] = {arg1};
    return sym_arrow::make_symbol(sym, args, 1, t);
};

symbol sym_arrow::make_symbol(const identifier& sym, const expr& arg1, const expr& arg2,
                        const identifier& t)
{
    expr args[] = {arg1, arg2};
    return sym_arrow::make_symbol(sym, args, 2, t);
};

symbol sym_arrow::make_symbol(const identifier& sym, const std::vector<expr>& arg, 
                        const identifier& t)
{
    return sym_arrow::make_symbol(sym, arg.data(), arg.size(), t);
};

symbol sym_arrow::make_symbol(const identifier& sym, std::initializer_list<expr> arg,
                        const identifier& t)
{
    return sym_arrow::make_symbol(sym, arg.begin(), arg.size(), t);
};

symbol sym_arrow::make_symbol(const identifier& sym, const expr* arg, size_t n,
                        const identifier& t)
{
    for (size_t i = 0; i < n; ++i)
        arg[i].cannonize(do_cse_default);

    using info          = ast::indexed_symbol_info;

    std::vector<identifier> def_args;
    identifier t_def;

    bool defined        = sd::sym_table_impl::get()->get_symbol_definition(sym, def_args, t_def);

    if (defined == false)
    {
        if (n == 0)
        {
            sd::sym_table_impl::get()->define_symbol(sym, def_args, t);
            t_def       = t;
        }
        else
        {
            se::sema_error().undefined_symbol(sym, n);
        }
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
    
    identifier t_fin;

    if (t.is_null() == true)
        t_fin           = t_def;
    else
        t_fin           = t;
    
    info f_info         = info(sym.get_ptr().get(), n, arg, t_fin.get_ptr().get());

    ast::symbol_ptr ep  = ast::indexed_symbol_rep::make(f_info);
    symbol ret          = symbol(ep);

    if (t_fin != t_def)
        se::sema_error().invalid_explicit_symbol_type(sym, def_args, t_def, t);
    
    return ret;
};


//-------------------------------------------------------------------
//                  symbol
//-------------------------------------------------------------------

symbol::~symbol()
{};

symbol::symbol(const char* name, size_t num_char)
    :m_expr()
{
    ast::identifier_ptr n = ast::identifier_rep::make(ast::identifier_info(name, num_char));
    m_expr = make_symbol(identifier(n)).get_ptr();
};

identifier symbol::get_name() const
{
    return identifier(get_ptr()->get_name());
}

identifier symbol::get_type() const
{
    return identifier(get_ptr()->get_type());
}

size_t symbol::size() const
{
    return get_ptr()->size();
}

expr symbol::arg(size_t i) const
{
    return expr(get_ptr()->arg(i));
}

size_t symbol::get_indexed_symbol_code() const
{
    return get_ptr()->get_indexed_symbol_code();
};

size_t symbol::get_base_symbol_code() const
{
    return get_ptr()->get_base_symbol_code();
};

};