/* 
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

#include "sym_arrow/ast/builder/vlist.h"
#include "sym_arrow/exception.h"

namespace sym_arrow { namespace ast
{

//-------------------------------------------------------------------
//                        vlist_stack
//-------------------------------------------------------------------
inline void vlist_stack::push_back(expr_handle ex)
{
    if (m_parent_stack)
        m_parent_stack->push_back(ex);
    else
        m_expr_stack.get().push_back(ex);
};

//-------------------------------------------------------------------
//                        vlist_base
//-------------------------------------------------------------------
template<class Item_type, class Derived_type>
inline vlist_base<Item_type,Derived_type>::vlist_base()
    :m_header(init_size, 0U, 0U)
{
    using context_type  = sym_dag::dag_context<term_tag>;
    context_type& c     = context_type::get();

    m_vector = (Item_type*)c.malloc(sizeof(Item_type) * init_size);
};

template<class Item_type, class Derived_type>
inline vlist_base<Item_type,Derived_type>::vlist_base(vlist_base&& other)
    :m_header(other.m_header), m_vector(other.m_vector)
{
    other.m_vector = nullptr;
};

template<class Item_type, class Derived_type>
inline vlist_base<Item_type,Derived_type>::~vlist_base()
{
    close();
};

template<class Item_type, class Derived_type>
void vlist_base<Item_type,Derived_type>::close()
{
    if (m_vector == nullptr)
        return;

    using context_type  = sym_dag::dag_context<term_tag>;
    context_type& c     = context_type::get();

    vlist_stack stack;

    size_t pos_size = m_header.m_pos;

    for(size_t i = 0; i < pos_size; ++i)
    {
        m_vector[i].release(stack);
        m_vector[i].~Item_type();
    }

    size_t size = m_header.m_capacity * sizeof(Item_type);

    if (has_previous())
    {
        get_previous()->close(stack);
        get_previous()->~Derived_type();
        c.free(get_previous(), sizeof(Derived_type));

        size += sizeof(Derived_type**);

        Derived_type** tmp = reinterpret_cast<Derived_type**>(m_vector);
        --tmp;
        c.free(tmp, size);
        m_vector = nullptr;
    }
    else
    {
        c.free(m_vector, size);
        m_vector = nullptr;
    };
};

template<class Item_type, class Derived_type>
void vlist_base<Item_type,Derived_type>::close(stack_type& stack)
{
    vlist_stack st(stack);
    close(st);
};

template<class Item_type, class Derived_type>
void vlist_base<Item_type,Derived_type>::close(vlist_stack& stack)
{    
    if (m_vector == nullptr)
        return;

    using context_type  = sym_dag::dag_context<term_tag>;
    context_type& c     = context_type::get();

    size_t pos  = m_header.m_pos;

    for(size_t i = 0; i < pos; ++i)
    {
        m_vector[i].release(stack);
        m_vector[i].~Item_type();
    };

    size_t size = m_header.m_capacity * sizeof(Item_type);

    if (has_previous())
    {
        get_previous()->close(stack);
        get_previous()->~Derived_type();

        c.free(get_previous(), sizeof(Derived_type));

        size += sizeof(Derived_type**);

        Derived_type** tmp = reinterpret_cast<Derived_type**>(m_vector);
        --tmp;
        
        c.free(tmp, size);
        m_vector = nullptr;
    }
    else
    {
        c.free(m_vector, size);
        m_vector = nullptr;
    };
};

template<class Item_type, class Derived_type>
inline Item_type& vlist_base<Item_type,Derived_type>::elem(size_t pos)
{
    return m_vector[pos];
};

template<class Item_type, class Derived_type>
inline const Item_type& vlist_base<Item_type,Derived_type>::elem(size_t pos) const
{
    return m_vector[pos];
};

template<class Item_type, class Derived_type>
inline size_t vlist_base<Item_type,Derived_type>::current_size() const
{
    return m_header.m_pos;
};

template<class Item_type, class Derived_type>
inline bool vlist_base<Item_type,Derived_type>::empty() const
{
    return current_size() == 0 && has_previous() == false;
};

template<class Item_type, class Derived_type>
bool vlist_base<Item_type,Derived_type>::is_null() const
{
    return m_vector == nullptr;
}

template<class Item_type, class Derived_type>
inline bool vlist_base<Item_type,Derived_type>::has_previous() const
{
    return m_header.m_has_prev != 0U;
};

template<class Item_type, class Derived_type>
inline void vlist_base<Item_type,Derived_type>::push_back(const value_type& val, 
                                                expr_handle ex)
{
    if (m_header.m_pos >= m_header.m_capacity)
        finish_this();

    new(m_vector + m_header.m_pos) Item_type(val, ex);
    ++m_header.m_pos;
};

template<class Item_type, class Derived_type>
inline void vlist_base<Item_type,Derived_type>::push_back(const Item_type& it)
{
    if (m_header.m_pos >= m_header.m_capacity)
        finish_this();

    new(m_vector + m_header.m_pos) Item_type(it);
    ++m_header.m_pos;
};

template<class Item_type, class Derived_type>
inline void vlist_base<Item_type,Derived_type>::push_back_special(const value_type& val, 
                                                        expr_handle ex)
{
    if (m_header.m_pos >= m_header.m_capacity)
        finish_this();

    new(m_vector + m_header.m_pos) Item_type(Item_type::special_t(), val, ex);
    ++m_header.m_pos;
};

template<class Item_type, class Derived_type>
inline const typename vlist_base<Item_type,Derived_type>::vlist_type* 
    vlist_base<Item_type,Derived_type>::get_previous() const
{
    const Derived_type** tmp = reinterpret_cast<const Derived_type**>(m_vector);
    return tmp[-1];
};

template<class Item_type, class Derived_type>
inline typename vlist_base<Item_type,Derived_type>::vlist_type* 
vlist_base<Item_type,Derived_type>::get_previous()
{
    Derived_type** tmp = reinterpret_cast<Derived_type**>(m_vector);
    return tmp[-1];
};

template<class Item_type, class Derived_type>
void vlist_base<Item_type,Derived_type>::finish_this()
{
    using context_type  = sym_dag::dag_context<term_tag>;
    context_type& c     = context_type::get();

    Derived_type* new_previous = (Derived_type*)c.malloc(sizeof(Derived_type));;
    new(new_previous) Derived_type(std::move(*get_derived()));
    
    m_header.m_capacity  = m_header.m_capacity * 2;
    m_header.m_pos       = 0;
    m_header.m_has_prev  = 1U;
    
    size_t size         = sizeof(Item_type)*m_header.m_capacity 
                        + sizeof(Derived_type**);

    Derived_type** tmp  = (Derived_type**)c.malloc(size);
    *tmp                = new_previous;
    ++tmp;
    
    m_vector            = (Item_type*)tmp;

    get_derived()->set_default_values();
};

template<class Item_type, class Derived_type>
inline Derived_type* vlist_base<Item_type,Derived_type>::get_derived()
{
    return static_cast<Derived_type*>(this);
};

template<class Item_type, class Derived_type>
inline bool vlist_base<Item_type,Derived_type>::is_simple() const
{
    if (current_size() == 1 && has_previous() == false 
        && elem(0).is_special() == false)
    {
        return true;
    }
    else
    {
        return false;
    };
};

};};
