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

#include "sym_arrow/ast/add_rep.h"
#include "sym_arrow/exception.h"

namespace sym_arrow { namespace ast
{

template<class Item_type>
bool add_rep_info<Item_type>::is_finite() const
{
    if (scal0->is_finite() == false)
        return false;

    for (int i = 0; i < n; ++i)
    {
        if (elems[i].get_value().is_finite() == false)
            return false;
    }

    return true;
};

template<class item_type>
add_rep::add_rep(const add_rep_info<item_type>& pi)
    :base_type(this), m_hash(pi.m_hash_add)
    ,m_size(pi.n), m_data(nullptr)
{
    using context_type  = sym_dag::dag_context<term_tag>;
    context_type& c     = context_type::get();

    bool has_log    = (pi.log_expr != nullptr);

    assertion(m_size + (has_log ? 1 : 0) > 0, "invalid add_rep");

    m_data          = (value_expr*)c.malloc((1+m_size)*sizeof(value_expr));

    if (has_log == true)
    {
        expr_handle h   = pi.log_expr->get_expr_handle();
        new(m_data+0) value_expr(*pi.scal0, h);

        add_symbols(h);

        base_type::set_user_flag<ast_flags::special>(true);
        base_type::set_user_flag<ast_flags::normalized>(true);
    }
    else
    {
        new(m_data+0) value_expr(*pi.scal0);
    };

    for (size_t i = 0; i < m_size; ++i)
    {
        expr_handle h   = pi.elems[i].get_expr_handle();
        new(m_data+i+1) value_expr(pi.elems[i].get_value(), h);

        add_symbols(h);
    };
};

template<class item_type>
size_t add_rep::eval_hash(const add_rep_info<item_type>& pi)
{
    if (pi.m_hash_add != 0)
        return pi.m_hash_add;

    size_t seed = pi.scal0->hash_value(); 

    for (size_t i = 0; i < pi.n; ++i)
    {
        boost::hash_combine(seed,pi.elems[i].get_value().hash_value());
        boost::hash_combine(seed,pi.elems[i].get_expr_handle());
    };

    if (pi.log_expr == nullptr)
        boost::hash_combine(seed, expr_handle());
    else
        boost::hash_combine(seed, pi.log_expr->get_expr_handle());

    pi.m_hash_add = seed;
    return seed;
};

template<class item_type>
bool add_rep::equal(const add_rep_info<item_type>& pi) const
{
    if (V0() != *pi.scal0)
        return false;

    size_t elem_size = size();

    if (elem_size != pi.n)
        return false;

    expr_handle log     = this->has_log()? Log() : nullptr;
    expr_handle log2    = pi.log_expr ? pi.log_expr->get_expr_handle() : nullptr;

    if (log != log2)
        return false;

    for (size_t i = 0; i < elem_size; ++i)
    {
        if (pi.elems[i].get_expr_handle() != this->E(i))
            return false;

        if (pi.elems[i].get_value() != this->V(i))
            return false;
    };

    return true;
};

inline size_t add_rep::hash_value() const
{ 
    return m_hash; 
};

inline size_t add_rep::size() const
{ 
    return m_size; 
};

inline bool add_rep::has_log() const
{ 
    return base_type::get_user_flag<ast_flags::special>(); 
}

inline const value& add_rep::V0() const
{ 
    return m_data[0].m_value; 
};

inline expr_handle add_rep::Log() const
{ 
    return m_data[0].m_expr.get(); 
};

inline expr_handle add_rep::E(size_t i) const
{ 
    return m_data[i+1].m_expr.get(); 
};

inline const value& add_rep::V(size_t i) const
{ 
    return m_data[i+1].m_value; 
};

inline const add_rep::value_expr* add_rep::VE() const
{ 
    return m_data + 1; 
};

inline const add_rep::value_expr& add_rep::VLog() const
{ 
    return m_data[0]; 
};

inline bool add_rep::is_normalized() const
{ 
    return base_type::get_user_flag<ast_flags::normalized>(); 
}

inline void add_rep::set_normalized()
{ 
    base_type::set_user_flag<ast_flags::normalized>(true); 
}

};};
