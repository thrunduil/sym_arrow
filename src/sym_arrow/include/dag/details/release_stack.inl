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

#include "dag/details/release_stack.h"

namespace sym_dag { namespace details
{

class SYM_DAG_EXPORT stack_arrays
{
    public:
        static void         release_array(size_t size, void** arr);
        static void**       get_small_array(size_t& size);
        static void**       get_large_array(size_t& size);

        static void*        malloc_ptr();
        static void         free_ptr(void* ptr);
};

template<class Item>
release_stack<Item>::release_stack(size_t capacity, Item** array_ptr)
    : m_capacity(capacity), m_pos(0), m_array(array_ptr)
    , m_previous(nullptr)
{};

template<class Item>
release_stack<Item>::release_stack(const release_stack& other)
    : m_capacity(other.m_capacity), m_pos(other.m_pos), m_array(other.m_array)
    , m_previous(other.m_previous)
{};

template<class Item>
release_stack<Item>::release_stack(release_stack&& other)
    : m_capacity(other.m_capacity), m_pos(other.m_pos), m_array(other.m_array)
    , m_previous(other.m_previous)
{
    other.m_pos     = 0;
    other.m_array   = nullptr;
    other.m_previous= nullptr;
}

template<class Item>
release_stack<Item>& release_stack<Item>::operator=(release_stack&& other)
{
    std::swap(m_capacity , other.m_capacity);
    std::swap(m_pos, other.m_pos);
    std::swap(m_array, other.m_array);
    std::swap(m_previous, other.m_previous);

    return *this;
}

template<class Item>
void details::swap(release_stack<Item>& h1, release_stack<Item>& h2)
{
    std::swap(h1.m_capacity, h2.m_capacity);
    std::swap(h1.m_pos, h2.m_pos);
    std::swap(h1.m_array, h2.m_array);
    std::swap(h1.m_previous, h2.m_previous);
}

template<class Item>
void release_stack<Item>::pop_level()
{
    release_stack* prev = m_previous;

    m_capacity  = m_previous->m_capacity;
    m_pos       = m_previous->m_pos;
    m_array     = m_previous->m_array;    
    m_previous  = m_previous->m_previous;

    handle_type::free_ptr(prev);
};

template<class Item>
void release_stack<Item>::push_level(size_t size, Item** arr)
{
    release_stack* prev = handle_type::malloc_ptr(*this);

    m_capacity  = size;
    m_pos       = 0;
    m_array     = arr;
    m_previous  = prev;
};

template<class Item>
release_stack<Item>::~release_stack()
{
    #if SYM_DAG_DEBUG_TERMS
        if (empty() == false || m_array != nullptr)
            details::error_stack_not_cleared();
    #endif
};

template<class Item>
inline bool release_stack<Item>::empty() const
{
    if (m_pos != 0 || m_previous != nullptr)
        return false;
    else
        return true;
};

template<class Item>
inline Item** release_stack<Item>::this_level_data()
{
    return m_array;
}

template<class Item>
inline size_t release_stack<Item>::this_level_size() const
{
    return m_pos;
}

template<class Item>
release_stack<Item>* release_stack<Item>::get_previous() const
{
    return m_previous;
}

template<class Item>
inline void release_stack<Item>::push_back(const Item* it)
{
    if (m_pos == m_capacity)
        return expand(it);
    else
        m_array[m_pos++] = const_cast<Item*>(it);
};

template<class Item>
void release_stack<Item>::expand(const Item* it)
{
    using tag_type  = typename Item::tag_type;

    if (m_capacity == 0)
    {
        dag_item_traits<tag_type>::check_destroy(it, *this);
        return;
    };

    release_stack_handle<Item>::expand(*this);
    push_back(it);
};

template<class Item>
inline release_stack_handle<Item>::release_stack_handle(stack_type&& st)
    :m_stack(std::move(st))
{};

template<class Item>
inline release_stack_handle<Item>::release_stack_handle(release_stack_handle&& other)
    :m_stack(std::move(other.m_stack))
{};

template<class Item>
release_stack_handle<Item>& 
release_stack_handle<Item>::operator=(release_stack_handle&& other)
{
    stack_type&& tmp    = std::move(this->m_stack);
    this->m_stack       = std::move(other.m_stack);
    other.m_stack       = std::move(tmp);

    return *this;
}

template<class Item>
inline release_stack_handle<Item>::~release_stack_handle()
{
    release();
}

template<class Item>
inline typename release_stack_handle<Item>::stack_type& 
release_stack_handle<Item>::get()
{
    return m_stack;
}

template<class Item>
inline release_stack_handle<Item> release_stack_handle<Item>::get_handle()
{
    size_t size;
    Item** arr  = get_small_array(size);

    return release_stack_handle(release_stack<Item>(size, arr));
};

template<class Item>
inline release_stack_handle<Item> release_stack_handle<Item>::get_empty_handle()
{
    return release_stack_handle(release_stack<Item>(0, nullptr));
};

template<class Item>
void swap(release_stack_handle<Item>& h1, release_stack_handle<Item>& h2)
{
    swap(h1.m_stack, h2.m_stack);
};

template<class Item>
void release_stack_handle<Item>::release()
{
    using tag_type          = typename Item::tag_type;
    using traits_type       = dag_item_traits<tag_type>;

    bool empty  = m_stack.empty();
    if (empty == true)
    {
        release_array(m_stack);
        return;
    };

    release_stack_handle work   = release_stack_handle::get_handle();    

    for (;;)
    {                
        stack_type& work_stack  = work.get();
        empty                   = m_stack.empty();

        if (empty == true)
            break;

        while(empty == false)
        {
            size_t size = m_stack.this_level_size();
            Item** arr  = m_stack.this_level_data();

            for (size_t i = 0; i < size; ++i)
                traits_type::check_destroy(arr[i], work_stack);

            empty       = pop_level(m_stack);
        };

        swap(*this, work);
    };

    release_array(m_stack);
    release_array(work.m_stack);
};

template<class Item>
bool release_stack_handle<Item>::pop_level(release_stack<Item>& stack)
{
    if (stack.get_previous() != nullptr)
    {
        stack.pop_level();
        return false;
    }
    else
    {
        stack.m_pos = 0;
        return true;
    }
};

template<class Item>
void release_stack_handle<Item>::release_array(release_stack<Item>& stack)
{
    if (stack.m_array == nullptr)
        return;

    stack_arrays::release_array(stack.m_capacity, (void**)stack.m_array);
    stack.m_array = nullptr;
};

template<class Item>
void release_stack_handle<Item>::expand(release_stack<Item>& st)
{
    size_t size;
    Item** arr  = get_large_array(size);

    st.push_level(size, arr);
};

template<class Item>
inline Item** release_stack_handle<Item>::get_small_array(size_t& size)
{
    return (Item**)stack_arrays::get_small_array(size);
};

template<class Item>
inline Item** release_stack_handle<Item>::get_large_array(size_t& size)
{
    return (Item**)stack_arrays::get_large_array(size);
};


template<class Item>
inline typename release_stack_handle<Item>::stack_type* 
release_stack_handle<Item>::malloc_ptr(const stack_type& st)
{
    stack_type* ptr   = (stack_type*)stack_arrays::malloc_ptr();
    new(ptr) stack_type(st);

    return ptr;
};

template<class Item>
inline void release_stack_handle<Item>::free_ptr(stack_type* ptr)
{
    stack_arrays::free_ptr(ptr);
};

};};