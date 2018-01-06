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

#include "dag/dag_item.h"
#include "dag/details/dag_context.inl"

namespace sym_dag { namespace details
{

void SYM_DAG_EXPORT error_invalid_cast();
void SYM_DAG_EXPORT error_invalid_item_code(size_t code);
void SYM_DAG_EXPORT error_stack_not_cleared();

};};

namespace sym_dag
{

//-----------------------------------------------------------------
//                      dag_item_base
//-----------------------------------------------------------------
template<class Tag>
template<class Node_type>
inline dag_item_base<Tag>::dag_item_base(const Node_type*)
{
    static const size_t code        = dag_node_to_code<Tag, Node_type>::code;
    static const size_t num_codes   = dag_tag_traits<Tag>::number_codes;

    static_assert(code < num_codes, "invalid code associated to this node");

    m_data.m_code   = code;
    m_data.m_ref    = 1;
    m_data.m_flags  = 0;
};

template<class Tag>
inline dag_item_base<Tag>::dag_item_base(size_t code)
{
    static const size_t num_codes   = dag_tag_traits<Tag>::number_codes;

    #if SYM_DAG_DEBUG_TERMS
        if (code >= num_codes)
            details::error_invalid_item_code(code);
    #endif

    m_data.m_code   = code;
    m_data.m_ref    = 1;
    m_data.m_flags  = 0;
};

template<class Tag>
template<class Term>
inline bool dag_item_base<Tag>::isa() const
{ 
    static const size_t code     = dag_node_to_code<Tag, Term>::code;
    static const size_t num_codes= dag_tag_traits<Tag>::number_codes;

    static_assert(code < num_codes, "invalid code associated to this node");

    return code == m_data.m_code; 
};

template<class Tag>
template<class Term>
inline const Term* dag_item_base<Tag>::static_cast_to() const
{    
    #if SYM_DAG_DEBUG_TERMS
        if (this->isa<Term>() != true)
            details::error_invalid_cast();
    #endif
    
    return static_cast<const Term*>(this); 
};

template<class Tag>
inline size_t dag_item_base<Tag>::refcount() const
{ 
    return m_data.m_ref; 
};

template<class Tag>
inline bool dag_item_base<Tag>::decrease_refcount() const
{
    return --m_data.m_ref == 0;
};

template<class Tag>
inline void dag_item_base<Tag>::increase_refcount() const
{ 
    ++m_data.m_ref; 
};

template<class Tag>
inline size_t dag_item_base<Tag>::get_code() const
{ 
    return m_data.m_code; 
};

template<class Tag>
inline void dag_item_base<Tag>::make_temporary(bool t) const
{ 
    static const size_t temporary_flag = header_type::temporary_flag;

    if (t == true)
        m_data.m_flags |= (1U << temporary_flag); 
    else
        m_data.m_flags &= ~(1U << temporary_flag);
};

template<class Tag>
inline bool dag_item_base<Tag>::is_temporary() const
{ 
    static const size_t temporary_flag = header_type::temporary_flag;

    size_t is_temp  = m_data.m_flags & (1U << temporary_flag);

    return is_temp != 0U && m_data.m_ref == 1; 
};

template<class Tag>
inline bool dag_item_base<Tag>::is_tracked() const
{
    static const size_t track_flag = header_type::track_flag;

    size_t has = m_data.m_flags & (1U << track_flag);
    return has != 0U;
}

template<class Tag>
void dag_item_base<Tag>::add_tracking_function(size_t tag, const track_function& f)
{
    dag_context<Tag>& c = dag_context<Tag>::get();
    return c.add_tracking_function(tag, f);
}

template<class Tag>
void dag_item_base<Tag>::remove_tracking_function(size_t tag)
{
    dag_context<Tag>& c = dag_context<Tag>::get();
    return c.remove_tracking_function(tag);
}

template<class Tag>
inline bool dag_item_base<Tag>::has_assigned_data() const
{
    static const size_t track_flag  = header_type::track_flag;
    static const size_t weak_flag   = header_type::weak_flag;

    static const size_t flags   = (1U << track_flag) | (1U << weak_flag);

    size_t has = m_data.m_flags & flags;
    return has != 0U;
}

template<class Tag>
inline bool dag_item_base<Tag>::has_weak_ptr() const
{
    static const size_t weak_flag       = header_type::weak_flag;

    static const size_t flags   = (1U << weak_flag);

    size_t has = m_data.m_flags & flags;
    return has != 0U;
};

template<class Tag>
inline void dag_item_base<Tag>::set_tracked(bool val) const
{ 
    static const size_t track_flag = header_type::track_flag;

    if (val == true)
        m_data.m_flags |= (1U << track_flag); 
    else
        m_data.m_flags &= ~(1U << track_flag);
};

template<class Tag>
inline void dag_item_base<Tag>::set_has_weak_ptr() const
{
    static const size_t weak_flag = header_type::weak_flag;
    m_data.m_flags |= (1U << weak_flag); 
};

template<class Tag>
template<size_t Bit>
inline void dag_item_base<Tag>::set_user_flag(bool val) const
{ 
    static const size_t MAX = details::calculate_flag_bits<Tag>::value;
    static_assert(Bit < MAX, "invalid bit in the user flag");

    static const size_t reserved_bits = header_type::reserved_bits;

    if (val == true)
        m_data.m_flags |= 1 << (Bit + reserved_bits); 
    else
        m_data.m_flags &= ~ (1 << (Bit + reserved_bits)); 
};

template<class Tag>
template<size_t Bit>
inline bool dag_item_base<Tag>::get_user_flag() const
{ 
    static const size_t MAX = details::calculate_flag_bits<Tag>::value;    
    static_assert(Bit < MAX, "invalid bit in the user flag");

    static const size_t reserved_bits = header_type::reserved_bits;
    return (m_data.m_flags & 1U << (Bit + reserved_bits) ) != 0U; 
};

template<class Tag>
inline typename dag_item_base<Tag>::user_data&
dag_item_base<Tag>::get_user_data() const
{
    return m_data;
};

//-----------------------------------------------------------------
//                      dag_item
//-----------------------------------------------------------------
template<class Derived, class Tag, bool hash_node>
inline dag_item<Derived, Tag, hash_node>::dag_item(const Derived* t)
    :dag_item_base(t)
{};

template<class Derived, class Tag, bool hash_node>
inline dag_item<Derived, Tag, hash_node>::dag_item(size_t code)
    :dag_item_base(code)
{};

template<class Derived, class Tag, bool hash_node>
template<class ... Args>
inline typename dag_item<Derived, Tag, hash_node>::ptr_type 
dag_item<Derived, Tag, hash_node>::make(const Args& ... args)
{
    dag_context<Tag>& c = dag_context<Tag>::get();
    return c.get<Derived, Args...>(args...);
};

//-----------------------------------------------------------------
//                      weak_dag_item
//-----------------------------------------------------------------
template<class Tag>
inline weak_dag_item<Tag>::weak_dag_item()
{};

template<class Tag>
inline weak_dag_item<Tag>::weak_dag_item(weak_dag_item const& r)
    :m_impl(r.m_impl)
{};

template<class Tag>
inline weak_dag_item<Tag>::weak_dag_item(weak_dag_item&& r)
    :m_impl(std::move(r.m_impl))
{};

template<class Tag>
inline weak_dag_item<Tag>&
weak_dag_item<Tag>::operator=(weak_dag_item const & r)
{
    m_impl  = r.m_impl;
    return *this;
}

template<class Tag>
inline weak_dag_item<Tag>&
weak_dag_item<Tag>::operator=(weak_dag_item && r)
{
    m_impl  = std::move(r.m_impl);
    return *this;
}

template<class Tag>
inline weak_dag_item<Tag>::~weak_dag_item()
{};

template<class Tag>
inline weak_dag_item<Tag>::weak_dag_item(dag_ptr const& r)
{
    using context_type  = dag_context<Tag>;
    m_impl = context_type::get().get_weak_ptr(r.get());
};

template<class Tag>
inline bool weak_dag_item<Tag>::expired() const
{
    if (!m_impl)
        return true;
    else
        return m_impl->expired();
}

template<class Tag>
inline typename weak_dag_item<Tag>::dag_ptr 
weak_dag_item<Tag>::lock() const
{
    if (!m_impl)
        return dag_ptr();
    else
        return m_impl->lock();
};

};