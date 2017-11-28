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

#include "sym_arrow/config.h"
#include "sym_arrow/nodes/expr.h"
#include "dag/dag.h"

namespace sym_arrow { namespace details
{

template<class Derived>
struct process_scalar1
{
    using tag_type      = sym_arrow::ast::term_tag;
    using scalar_handle = const ast::scalar_rep*;

    template<class ... Args>
    void make(expr& ret, const expr& a, Args ... args)
    {
        const ast::expr_base* a_h       = a.get_ptr().get();

        if (a_h->get_code() == (size_t)ast::term_types::scalar)
        {
            scalar_handle as_h          = static_cast<scalar_handle>(a_h);
            return get_derived()->eval(ret, as_h->get_data(), std::forward<Args>(args)...);
        }
        else
        {
            return get_derived()->eval(ret, a_h, std::forward<Args>(args)...);
        };
    };

    Derived* get_derived()
    {
        return static_cast<Derived*>(this);
    }
};

template<class Derived>
struct process_scalar2
{
    using tag_type      = sym_arrow::ast::term_tag;
    using scalar_handle = const ast::scalar_rep*;

    template<class ... Args>
    void make(expr& ret, const expr& a, const expr& b, Args ... args)
    {
        const ast::expr_base* a_h       = a.get_ptr().get();

        const ast::expr_base* b_h       = b.get_ptr().get();

        if (a_h->get_code() == (size_t)ast::term_types::scalar)
        {
            scalar_handle as_h          = static_cast<scalar_handle>(a_h);

            if (b_h->get_code() == (size_t)ast::term_types::scalar)
            {
                scalar_handle bs_h      = static_cast<scalar_handle>(b_h);

                return get_derived()->eval(ret, as_h->get_data(), bs_h->get_data(),
                                           std::forward<Args>(args)...);
            }
            else
            {
                return get_derived()->eval(ret, as_h->get_data(), b_h,
                                           std::forward<Args>(args)...);
            };
        }
        else
        {
            if (b_h->get_code() == (size_t)ast::term_types::scalar)
            {
                scalar_handle bs_h      = static_cast<scalar_handle>(b_h);

                return get_derived()->eval(ret, a_h, bs_h->get_data(),
                                           std::forward<Args>(args)...);
            }
            else
            {
                return get_derived()->eval(ret, a_h, b_h, std::forward<Args>(args)...);
            };
        };
    };

    template<class ... Args>
    void make(expr& ret, const value& a, const expr& b, Args ... args)
    {
        const ast::expr_base* b_h       = b.get_ptr().get();

        if (b_h->get_code() == (size_t)ast::term_types::scalar)
        {
            scalar_handle bs_h      = static_cast<scalar_handle>(b_h);

            return get_derived()->eval(ret, a, bs_h->get_data(), 
                                       std::forward<Args>(args)...);
        }
        else
        {
            return get_derived()->eval(ret, a, b_h, std::forward<Args>(args)...);
        };
    };

    template<class ... Args>
    void make(expr& ret, const expr& a, const value& b, Args ... args)
    {
        const ast::expr_base* a_h       = a.get_ptr().get();

        if (a_h->get_code() == (size_t)ast::term_types::scalar)
        {
            scalar_handle as_h          = static_cast<scalar_handle>(a_h);
            
            return get_derived()->eval(ret, as_h->get_data(), b, 
                                       std::forward<Args>(args)...);
        }
        else
        {
            return get_derived()->eval(ret, a_h, b, std::forward<Args>(args)...);
        };
    };

    Derived* get_derived()
    {
        return static_cast<Derived*>(this);
    }
};

}};
