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

#pragma once

#include "sym_arrow/nodes/expr_visitor.h"

namespace sym_arrow
{

template<class Derived>
template<class ... Args>
auto expr_visitor<Derived>::visit(const expr& ex, Args&& ... args)
    -> decltype(std::declval<Derived>().eval(ex, std::forward<Args>(args)...))
{
    if (ex.is_null() == true)
        throw std::runtime_error("access to uninitialized expr");

    Derived* d = static_cast<Derived*>(this);

    switch (get_expression_type(ex))
    {
        case ast::term_types::scalar:
            return d->eval(cast_scalar(ex), std::forward<Args>(args)...);
        case ast::term_types::symbol:
            return d->eval(cast_symbol(ex), std::forward<Args>(args)...);
        case ast::term_types::add_rep:
            return d->eval(cast_add(ex), std::forward<Args>(args)...);
        case ast::term_types::mult_rep:
            return d->eval(cast_mult(ex), std::forward<Args>(args)...);
        case ast::term_types::function_rep:
            return d->eval(cast_function(ex), std::forward<Args>(args)...);

        case ast::term_types::add_build:
        case ast::term_types::mult_build:
        {
            ex.cannonize();
            return visit(ex, std::forward<Args>(args)...);
        }
        default:
            assertion(0, "unknown expr type");
            throw std::runtime_error("unknown expr type");
    };
};

template<class Derived>
template<class Type, class ... Args>
auto expr_visitor<Derived>::visit(const Type& ex, Args&& ... args)
    -> decltype(std::declval<Derived>().eval(ex, std::forward<Args>(args)...))
{
    Derived* d = static_cast<Derived*>(this);
    return d->eval(ex, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
inline void expr_traversal_visitor<Derived>::eval(const value&, Args&& ...)
{};

template<class Derived>
template<class ... Args>
inline void expr_traversal_visitor<Derived>::eval(int, Args&& ...)
{};

template<class Derived>
template<class ... Args>
inline void expr_traversal_visitor<Derived>::eval(const sym_arrow::scalar& s, Args&& ... args)
{
    visit(s.get_value(), std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
inline void expr_traversal_visitor<Derived>::eval(const sym_arrow::symbol&, Args&& ...)
{};

template<class Derived>
template<class ... Args>
void expr_traversal_visitor<Derived>::eval(const sym_arrow::add_expr& ex, Args&& ... args)
{
    size_t n = ex.size();

    for (size_t j = 0; j < n; ++j)
    {
        visit(ex.E(j), std::forward<Args>(args)...);
        visit(ex.V(j), std::forward<Args>(args)...);
    }

    if (ex.has_log() == true)
        visit(ex.Log(), std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void expr_traversal_visitor<Derived>::eval(const sym_arrow::mult_expr& ex, Args&& ... args)
{
    size_t in   = ex.isize();
    size_t rn   = ex.rsize();

    for (size_t i = 0; i < in; ++i)
    {
        visit(ex.IE(i), std::forward<Args>(args)...);    
        visit(ex.IV(i), std::forward<Args>(args)...);    
    }

    for (size_t i = 0; i < rn; ++i)
    {
        visit(ex.RE(i), std::forward<Args>(args)...);
        visit(ex.RV(i), std::forward<Args>(args)...);
    };

    if (ex.has_exp())
        visit(ex.Exp(), std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void expr_traversal_visitor<Derived>::eval(const sym_arrow::function_expr& ex, Args&& ... args)
{
    size_t n = ex.size();

    for (size_t j = 0; j < n; ++j)
        visit(ex.arg(j), std::forward<Args>(args)...);
};

}
