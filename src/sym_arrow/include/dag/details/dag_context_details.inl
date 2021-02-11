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

#include "dag/details/dag_context_details.h"
#include "dag/details/allocator.h"
#include "dag/details/object_table.inl"

namespace sym_dag { namespace details
{

//-----------------------------------------------------------------
//                      object_tables
//-----------------------------------------------------------------
template<class Tag, int Code>
void object_tables<Tag, Code>::close()
{
    m_table.close();
    object_tables<Tag, Code-1>::close();    
};

template<class Tag, int Code>
void object_tables<Tag, Code>::print_reuse_stats(std::ostream& os)
{
    m_table.print_reuse_stats(os);
    object_tables<Tag, Code-1>::print_reuse_stats(os);
};

template<class Tag, int Code>
void object_tables<Tag, Code>::print_memory_stats(std::ostream& os, memory_stats& stats)
{
    m_table.print_memory_stats(os, stats);
    object_tables<Tag, Code-1>::print_memory_stats(os, stats);
};

template<class Tag, int Code>
void object_tables<Tag, Code>::print_collisions(std::ostream& os)
{
    m_table.print_collisions(os);
    object_tables<Tag, Code-1>::print_collisions(os);
};

//-----------------------------------------------------------------
//                      weak_dag_item
//-----------------------------------------------------------------
template<class Tag>
inline weak_node<Tag>::weak_node(base_type h, create_tag)
    :m_base(h), m_destroyed(false), m_refcount(1)
{};

template<class Tag>
inline size_t weak_node<Tag>::eval_hash(base_type h, create_tag)
{
    return (size_t)h;
}

template<class Tag>
inline size_t weak_node<Tag>::eval_hash(base_type h, destroy_tag)
{
    return (size_t)h;
}

template<class Tag>
inline bool weak_node<Tag>::equal(base_type h, create_tag) const
{
    return m_base == h && m_destroyed == false;
};

template<class Tag>
bool weak_node<Tag>::equal(base_type h, destroy_tag) const
{
    return m_base == h && m_destroyed == false;
};

template<class Tag>
inline size_t weak_node<Tag>::refcount() const
{
    return m_refcount;
}

template<class Tag>
inline void weak_node<Tag>::increase_refcount() const
{ 
    ++m_refcount; 
};

template<class Tag>
inline bool weak_node<Tag>::decrease_refcount() const
{
    return --m_refcount == 0;
};

template<class Tag>
inline size_t weak_node<Tag>::hash_value() const
{
    return (size_t)m_base;
}

template<class Tag>
inline bool weak_node<Tag>::expired() const
{
    return m_destroyed == true;
};

template<class Tag>
inline void weak_node<Tag>::mark_expired() const
{
    m_destroyed = true;
}

template<class Tag>
inline typename weak_node<Tag>::dag_ptr 
weak_node<Tag>::lock() const
{
    if (m_destroyed == true)
        return dag_ptr();
    else
        return dag_ptr::from_this(m_base);
};

//-----------------------------------------------------------------
//                      object_table_weak
//-----------------------------------------------------------------
template<class Tag>
void object_table_weak<Tag>::close()
{
    m_table.close();
}

template<class Tag>
void object_table_weak<Tag>::print_reuse_stats(std::ostream& os)
{
    m_table.print_reuse_stats(os);
}

template<class Tag>
void object_table_weak<Tag>::print_memory_stats(std::ostream& os, memory_stats& stats)
{
    m_table.print_memory_stats(os, stats);
}

template<class Tag>
void object_table_weak<Tag>::print_collisions(std::ostream& os)
{
    m_table.print_collisions(os);
}

};};
