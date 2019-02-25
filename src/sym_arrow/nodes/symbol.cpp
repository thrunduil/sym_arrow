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

namespace sym_arrow 
{

namespace sd = sym_arrow::details;

symbol::~symbol()
{};

symbol::symbol(const char* name, size_t num_char)
    :m_expr()
{
    ast::base_symbol_ptr n = ast::base_symbol_rep::make(ast::named_symbol_info(name, num_char));
    m_expr = ast::indexed_symbol_rep::make(ast::indexed_symbol_info(n.get(), 0, nullptr));

    if (num_char == 0 || name[0] == '$')
        throw std::runtime_error("invalid symbol name");
};

const char* symbol::get_name() const
{
    return get_ptr()->get_name()->get_name();
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

expr symbol::operator()() const
{
    return function(*this);
}

expr symbol::operator()(const expr& x1) const
{
    return sym_arrow::function(*this, x1);
}

expr symbol::operator()(const expr& x1, const expr& x2) const
{
    return sym_arrow::function(*this, x1, x2);
}

expr symbol::operator()(std::initializer_list<expr> ex) const
{
    return sym_arrow::function(*this, ex);
}

expr symbol::operator()(const std::vector<expr>& ex) const
{
    return sym_arrow::function(*this, ex);
}

symbol symbol::index(const expr& x1) const
{
    return sym_arrow::index(*this, x1);
}

symbol symbol::index(const expr& x1, const expr& x2) const
{
    return sym_arrow::index(*this, x1, x2);
}

symbol symbol::index(std::initializer_list<expr> ex) const
{
    return sym_arrow::index(*this, ex);
}

symbol symbol::index(const std::vector<expr>& ex) const
{
    return sym_arrow::index(*this, ex);
}


symbol sym_arrow::index(const symbol& sym, const expr& arg1)
{
    expr args[] = {arg1};
    return sym_arrow::index(sym, args, 1);
};

symbol sym_arrow::index(const symbol& sym, const expr& arg1, const expr& arg2)
{
    expr args[] = {arg1, arg2};
    return sym_arrow::index(sym, args, 2);
};

symbol sym_arrow::index(const symbol& sym, const std::vector<expr>& arg)
{
    return sym_arrow::index(sym, arg.data(), arg.size());
};

symbol sym_arrow::index(const symbol& sym, std::initializer_list<expr> arg)
{
    return sym_arrow::index(sym, arg.begin(), arg.size());
};

symbol sym_arrow::index(const symbol& sym, const expr* arg, size_t n)
{
    if (n == 0)
        return sym;

    for (size_t i = 0; i < n; ++i)
        arg[i].cannonize(do_cse_default);

    using info              = ast::indexed_symbol_info;

    if (sym.size() == 0)
    {    
        info f_info         = info(sym.get_ptr()->get_name(), n, arg);

        ast::symbol_ptr ep  = ast::indexed_symbol_rep::make(f_info);
        return symbol(ep);
    };

    size_t n_old            = sym.size();
    size_t n_elem           = n + n_old;

    int size_counter        = 0;
    using expr_pod          =  sd::pod_type<expr>;
    expr_pod::destructor_type d(&size_counter);
    sd::stack_array<expr_pod> buff(n_elem, &d);    

    expr* buff_ptr          = reinterpret_cast<expr*>(buff.get());

    for (size_t i = 0; i < n_old; ++i)
    {
        new(buff_ptr + size_counter) expr(std::move(sym.arg(i)));
        ++size_counter;
    };

    for (size_t i = 0; i < n; ++i)
    {
        new(buff_ptr + size_counter) expr(arg[i]);
        ++size_counter;
    };

    info f_info         = info(sym.get_ptr()->get_name(), n_elem, buff_ptr);

    ast::symbol_ptr ep  = ast::indexed_symbol_rep::make(f_info);
    return symbol(ep);
};

};