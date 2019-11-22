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

#include "dag/details/release_stack.inl"
#include "dag/details/object_table.inl"
#include "dag/details/allocator.h"
#include "dag/details/global_objects.h"

namespace sym_dag { namespace details
{

struct release_stack_tag{};

class stack_arrays_impl
{
    private:
        static const size_t small_array_size    = 50;
        static const size_t max_arrays          = 20;
        static const size_t large_array_size    = 1000;

    private:
        using alloc     = symbolic_allocator<release_stack_tag>;
        using pool_type = object_allocator<alloc>;
        using arr_vec   = std::vector<void**>;

    private:
        pool_type   m_ptr_pool;
        arr_vec     m_small_arrays;

    public:
        stack_arrays_impl();
        ~stack_arrays_impl();

        void         release_array(size_t size, void** arr);
        void**       get_small_array(size_t& size);
        void**       get_large_array(size_t& size);

        void*        malloc_ptr();
        void         free_ptr(void* ptr);
};

stack_arrays_impl::stack_arrays_impl()
    :m_ptr_pool(sizeof(release_stack<void>))
{};

stack_arrays_impl::~stack_arrays_impl()
{
    m_ptr_pool.purge_memory();

    for (size_t i = 0; i < m_small_arrays.size(); ++i)
        alloc::free(m_small_arrays[i]);
};

void* stack_arrays_impl::malloc_ptr()
{
    return m_ptr_pool.malloc();
};

void stack_arrays_impl::free_ptr(void* ptr)
{
    return m_ptr_pool.free(ptr);
};

void** stack_arrays_impl::get_small_array(size_t& size)
{
    if (m_small_arrays.size() > 0)
    {
        size        = small_array_size;
        void** arr  = m_small_arrays.back();
        m_small_arrays.pop_back();

        return arr;
    };

    size        = small_array_size;
    void** arr  = nullptr;
    
    try
    {
        arr     = (void**)alloc::malloc(size * sizeof(void*));
    }
    catch(std::exception&)
    {
        size    = 0;
        return nullptr;
    }

    if (!arr)
        size    = 0;

    return arr;
};

void** stack_arrays_impl::get_large_array(size_t& size)
{
    size        = large_array_size;
    void** arr  = nullptr;
    
    try
    {
        arr     = (void**)alloc::malloc(size * sizeof(void*));
    }
    catch(std::exception&)
    {
        size    = 0;
        return nullptr;
    }

    if (!arr)
        size    = 0;

    return arr;
};

void stack_arrays_impl::release_array(size_t size, void** arr)
{
    if (size > small_array_size)
    {
        alloc::free(arr);
        return;
    }

    if (m_small_arrays.size() > max_arrays)
    {
        alloc::free(arr);
        return;
    }

    m_small_arrays.push_back(arr);
};

stack_arrays_impl* g_arrays = global_objects::make_after<stack_arrays_impl>();

void stack_arrays::release_array(size_t size, void** arr)
{
    return g_arrays->release_array(size, arr);
};

void** stack_arrays::get_small_array(size_t& size)
{
    return g_arrays->get_small_array(size);
}

void** stack_arrays::get_large_array(size_t& size)
{
    return g_arrays->get_large_array(size);
}

void* stack_arrays::malloc_ptr()
{
    return g_arrays->malloc_ptr();
}

void stack_arrays::free_ptr(void* ptr)
{
    return g_arrays->free_ptr(ptr);
}

};};
