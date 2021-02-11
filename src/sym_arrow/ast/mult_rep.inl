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

#pragma once

#include "sym_arrow/ast/mult_rep.h"
#include "sym_arrow/exception.h"

namespace sym_arrow { namespace ast
{

template<class Iitem_type, class Ritem_type>
mult_rep::mult_rep(const mult_rep_info<Iitem_type, Ritem_type>& pi)
    : base_type(this), m_hash(pi.m_hash_mult)
    , m_int_size(pi.in), m_int_data(nullptr), m_real_data(nullptr)
    , m_real_size(pi.rn)
{
    using context_type  = sym_dag::dag_context<term_tag>;
    context_type& c     = context_type::get();

    bool has_exp    = (pi.exp_expr != nullptr);

    assertion(m_int_size + m_real_size + (has_exp ? 1 : 0) > 0, "invalid mult_rep");

    size_t size_int     = m_int_size * sizeof(int_expr);
    size_t size_real    = m_real_size * sizeof(value_expr);
    size_t size_exp     = (has_exp ? 1 : 0) * sizeof(value_expr);

    void* data          = c.malloc(size_int + size_real + size_exp);

    m_int_data          = reinterpret_cast<int_expr*>(data);
    m_real_data         = reinterpret_cast<value_expr*>((char*)data + size_int);

    for (size_t i = 0; i < m_int_size; ++i)
    {
        expr_handle h   = pi.iexpr[i].get_expr_handle();
        new(m_int_data + i) int_expr(pi.iexpr[i].m_value, h);
        add_symbols(h);
    }

    for (size_t i = 0; i < m_real_size; ++i)
    {
        expr_handle h   = pi.rexpr[i].get_expr_handle();
        new(m_real_data+i) value_expr(pi.rexpr[i].m_value, h);
        add_symbols(h);
    }

    if (has_exp == true)
    {
        expr_handle h   = pi.exp_expr->get_expr_handle();
        new(m_real_data+m_real_size) value_expr(value::make_zero(), h);

        add_symbols(h);
        base_type::set_user_flag<ast_flags::special>(true);
    };
};

template<class Iitem_type, class Ritem_type>
size_t mult_rep::eval_hash(const mult_rep_info<Iitem_type, Ritem_type>& pi)
{
    if (pi.m_hash_mult != 0)
        return pi.m_hash_mult;

    size_t seed = pi.in; 

    for (size_t i = 0; i < pi.in; ++i)
    {
        boost::hash_combine(seed, size_t(pi.iexpr[i].m_value));
        boost::hash_combine(seed,pi.iexpr[i].get_expr_handle());
    };

    if (pi.exp_expr == nullptr)
        boost::hash_combine(seed, expr_handle());
    else
        boost::hash_combine(seed, pi.exp_expr->get_expr_handle());
    
    for (size_t i = 0; i < pi.rn; ++i)
    {
        boost::hash_combine(seed, pi.rexpr[i].m_value.hash_value());
        boost::hash_combine(seed, pi.rexpr[i].get_expr_handle());
    };

    pi.m_hash_mult = seed;

    return seed;
};

template<class Iitem_type, class Ritem_type>
bool mult_rep::equal(const mult_rep_info<Iitem_type,Ritem_type>& pi) const
{
    size_t m_isize = isize();
    size_t m_rsize = rsize();

    if (pi.in != m_isize)
        return false;

    if (pi.rn != m_rsize)
        return false;

    expr_handle exp     = this->has_exp() ? this->Exp() : nullptr;
    expr_handle exp2    = pi.exp_expr ? pi.exp_expr->get_expr_handle() : nullptr;

    if (exp != exp2)
        return false;

    for (size_t i = 0; i < m_isize; ++i)
    {
        if (pi.iexpr[i].get_expr_handle() != this->IE(i) 
            || pi.iexpr[i].m_value != this->IV(i))
        {
            return false;
        };
    };

    for (size_t i = 0; i < m_rsize; ++i)
    {
        if (pi.rexpr[i].get_expr_handle() != this->RE(i) 
            || pi.rexpr[i].m_value != this->RV(i))
        {
            return false;
        };
    };

    return true;
};

inline size_t mult_rep::hash_value() const
{ 
    return m_hash;
};

inline bool mult_rep::has_exp() const
{ 
    return base_type::get_user_flag<ast_flags::special>(); 
}

inline expr_handle mult_rep::Exp() const
{ 
    return m_real_data[m_real_size].m_expr.get(); 
};

inline size_t mult_rep::isize() const
{ 
    return m_int_size; 
};

inline expr_handle mult_rep::IE(size_t i) const
{ 
    return m_int_data[i].m_expr.get(); 
};        

inline int mult_rep::IV(size_t i) const
{ 
    return m_int_data[i].m_value; 
};

inline size_t mult_rep::rsize() const
{ 
    return m_real_size; 
};

inline expr_handle mult_rep::RE(size_t i) const
{ 
    return m_real_data[i].m_expr.get(); 
};

inline const value& mult_rep::RV(size_t i) const
{ 
    return m_real_data[i].m_value; 
};

inline const mult_rep::int_expr* mult_rep::IVE() const
{ 
    return m_int_data + 1; 
};

inline const mult_rep::value_expr* mult_rep::RVE() const
{ 
    return m_real_data + 1; 
};

};};
