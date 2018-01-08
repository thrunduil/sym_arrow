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

#include "dag/dag_context.h"
#include "dag/details/allocator.h"
#include "dag/details/object_table.inl"
#include "dag/details/dag_context_details.inl"
#include "dag/details/leak_detector.h"

namespace sym_dag
{

//-----------------------------------------------------------------
//                      dag_context
//-----------------------------------------------------------------

template<class Tag>
dag_context<Tag>::dag_context()
{
    registered_dag_context::get().register_dag(this);    
    m_allocated     = 0;
    m_context_data  = nullptr;
};

template<class Tag>
dag_context<Tag>::~dag_context()
{};

template<class Tag>
void dag_context<Tag>::close()
{
    m_tables.close();
    m_table_weak.close();
    m_mem_manager.purge_memory();
};

template<class Tag>
void dag_context<Tag>::close_context_data()
{
    if (m_context_data)
    {
        m_context_data->close();
        delete m_context_data;
        m_context_data = nullptr;
    };
};

template<class Tag>
inline typename dag_context<Tag>::context_data& 
dag_context<Tag>::get_context_data()
{
    if (m_context_data == nullptr)
    {
        m_context_data  = new context_data();
        m_context_data->initialize();  
    };

    return *static_cast<context_data*>(m_context_data);
}

template<class Tag>
inline typename dag_context<Tag>::context_data const& 
dag_context<Tag>::get_context_data() const
{
    if (m_context_data == nullptr)
    {
        m_context_data  = new context_data();
        m_context_data->initialize();  
    };

    return *static_cast<const context_data*>(m_context_data);
}

template<class Tag>
template<class Node_type>
details::object_table<dag_ptr<Node_type>, details::symbolic_allocator<Tag>>&      
dag_context<Tag>::get_object_table()
{
    using table             = details::object_table<dag_ptr<Node_type>, 
                                details::symbolic_allocator<dag_context>>;
    static const int code   = dag_node_to_code<Tag, Node_type>::code;
    using table_container   = details::object_tables<Tag, code>;

    return static_cast<table_container&>(m_tables).m_table;
};

template<class Tag>
typename dag_context<Tag>::stack_handle
inline dag_context<Tag>::get_stack()
{
    return stack_handle::get_handle();
};

template<class Tag>
template<class Node_type>
void dag_context<Tag>::unregister(Node_type* h)
{
    stack_handle sh = this->get_stack();
    unregister(h, sh.get());
};

template<class Tag>
template<class Node_type>
inline void dag_context<Tag>::unregister(Node_type* h, stack_type& stack)
{
    using alloc     = details::symbolic_allocator<Tag>;
    using ptr_type  = dag_ptr<Node_type>;
    using table     = details::object_table<ptr_type, alloc>;

    table& t        = get_object_table<Node_type>();

    if (h->has_assigned_data() == true)
        remove_assigned_data(h, stack);

    t.unregister_obj(h, stack);
};

template<class Tag>
template<class Node_type>
inline void dag_context<Tag>::unregister_without_stack(Node_type* h)
{
    using alloc     = details::symbolic_allocator<Tag>;
    using ptr_type  = dag_ptr<Node_type>;
    using table     = details::object_table<ptr_type, alloc>;

    table& t        = get_object_table<Node_type>();

    if (h->has_assigned_data() == true)
        remove_assigned_data(h);

    t.unregister_obj(h);
};

template<class Tag>
inline void dag_context<Tag>::remove_assigned_data(handle_type h, stack_type& st)
{
    if (h->is_tracked() == true)
        call_track_functions(h, st);

    if (h->has_weak_ptr() == true)
        remove_weak_ptr(h);
}

template<class Tag>
inline void dag_context<Tag>::remove_assigned_data(handle_type h)
{
    if (h->is_tracked() == true)
        call_track_functions(h);

    if (h->has_weak_ptr() == true)
        remove_weak_ptr(h);
}

template<class Tag>
void dag_context<Tag>::call_track_functions(handle_type h, stack_type& st)
{
    for (const auto& elem : m_track_functions)
        elem.second(h, st);
}

template<class Tag>
void dag_context<Tag>::call_track_functions(handle_type h)
{
    for (const auto& elem : m_track_functions)
        elem.second(h);
}

template<class Tag>
void dag_context<Tag>::add_tracking_function(size_t tag, const track_function& f)
{
    m_track_functions[tag] = f;
}

template<class Tag>
void dag_context<Tag>::remove_tracking_function(size_t tag)
{
    m_track_functions.erase(tag);
};

template<class Tag>
inline void dag_context<Tag>::unregister_weak(details::weak_node<Tag>* h)
{
    m_table_weak.m_table.unregister_obj(h);
};

template<class Tag>
inline typename dag_context<Tag>::weak_node_ptr 
dag_context<Tag>::get_weak_ptr(handle_type h)
{
    if (!h)
        return weak_node_ptr();

    h->set_has_weak_ptr();
    weak_node_ptr ptr = m_table_weak.m_table.get(h, details::weak_node<Tag>::create_tag());
    return ptr;
};

template<class Tag>
void dag_context<Tag>::remove_weak_ptr(handle_type h)
{
    weak_node_ptr ptr = m_table_weak.m_table.get_existing(h, details::weak_node<Tag>::destroy_tag());
    if (!ptr)
        return;

    ptr->mark_expired();
};

template<class Tag>
template<class Node_type, class ... Args>
dag_ptr<Node_type> dag_context<Tag>::get(const Args& ... args)
{
    using alloc     = details::symbolic_allocator<Tag>;
    using ptr_type  = dag_ptr<Node_type>;
    using table     = details::object_table<ptr_type, alloc>;

    table& t        = get_object_table<Node_type>();
    return t.get(args...);
};

template<class Tag>
inline dag_context<Tag>& dag_context<Tag>::get()
{    
    return *m_global;
};

template<class Tag>
dag_context<Tag>*
dag_context<Tag>::m_global = global_objects::make_context<dag_context<Tag>>();

template<class Tag>
void dag_context<Tag>::print_reuse_stats(std::ostream& os)
{
    os << std::string(4,' ') << "tag: " << typeid(Tag).name() << "\n";
    os << std::string(4,' ') << std::string(40,'-') << "\n";
    
    m_tables.print_reuse_stats(os);
    m_table_weak.print_reuse_stats(os);
    
    os << "\n";
}

template<class Tag>
void dag_context<Tag>::print_memory_stats(std::ostream& os, details::memory_stats& stats)
{
    os << std::string(4,' ') << "tag: " << typeid(Tag).name() << "\n";
    
    #if SYM_DAG_DEBUG_MEMORY
        os << std::string(4,' ') << "alloc: " << double(m_allocated)/double(2e6) << "MB" << "\n";
        stats.m_bytes_mem += m_allocated;
    #endif
    
    os << std::string(4,' ') << std::string(40,'-') << "\n";
    
    m_tables.print_memory_stats(os, stats);
    m_table_weak.print_memory_stats(os, stats);

    os << "\n";
};

template<class Tag>
void dag_context<Tag>::print_collisions(std::ostream& os)
{
    os << std::string(4,' ') << "tag: " << typeid(Tag).name() << "\n";
    os << std::string(4,' ') << std::string(40,'-') << "\n";

    m_tables.print_collisions(os);
    m_table_weak.print_collisions(os);

    os << "\n";
};

template<class Tag>
void* dag_context<Tag>::malloc(size_t bytes)
{
    size_t words    = (bytes + sizeof(size_t) - 1) / sizeof(size_t);
    using alloc     = details::symbolic_allocator<Tag>;

    void* ptr;

    if (words > DAG_MALLOC_MAX_SIZE)
        ptr = alloc::malloc(bytes);
    else
        ptr = m_mem_manager.malloc(words);

    #if SYM_DAG_DEBUG_MEMORY
        details::leak_detector::report_malloc(ptr);
        m_allocated += bytes;
    #endif  

    return ptr;
};

template<class Tag>
void dag_context<Tag>::free(void* ptr, size_t bytes)
{
    #if SYM_DAG_DEBUG_MEMORY
        m_allocated -= bytes;
        details::leak_detector::report_free(ptr);
    #endif

    size_t size     = (bytes + sizeof(size_t) - 1) / sizeof(size_t);
    using alloc     = details::symbolic_allocator<Tag>;

    if (size > DAG_MALLOC_MAX_SIZE)
        alloc::free(ptr);
    else
        return m_mem_manager.free(ptr,size); 
};

};
