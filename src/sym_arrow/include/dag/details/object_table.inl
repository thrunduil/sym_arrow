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

#include "dag/details/object_table.h"
#include "dag/details/hash_equal.inl"
#include "dag/details/leak_detector.h"

#include <iostream>

namespace sym_dag { namespace details
{

//-----------------------------------------------------------------
//                      object_allocator
//-----------------------------------------------------------------
template<class Alloc>
inline object_allocator<Alloc>::object_allocator(size_t size)
    :m_pool(size)
{};

template<class Alloc>
inline void object_allocator<Alloc>::free(void* ptr)
{
    #if SYM_DAG_DEBUG_MEMORY
        if(m_pool.is_from(ptr) == false)
            std::cerr << "invalid free" << "\n";

        leak_detector::report_free(ptr);
    #endif

    m_pool.free(ptr);
};

template<class Alloc>
inline void* object_allocator<Alloc>::malloc()
{    
    void* ptr = m_pool.malloc();

    #if SYM_DAG_DEBUG_MEMORY
        leak_detector::report_malloc(ptr);
    #endif  

    return ptr;
};

template<class Alloc>
inline void object_allocator<Alloc>::purge_memory()
{
    m_pool.purge_memory();
};

//-----------------------------------------------------------------
//                      hashed_object_handle
//-----------------------------------------------------------------
template<class Hash_entry>
inline hashed_object_handle<Hash_entry>::hashed_object_handle(const Hash_entry& entry)
    :m_entry(entry)
{}

template<class Hash_entry>
inline typename hashed_object_handle<Hash_entry>::value_type const*
hashed_object_handle<Hash_entry>::get() const
{
    return m_entry.get();
}

template<class Hash_entry>
inline typename hashed_object_handle<Hash_entry>::value_type*
hashed_object_handle<Hash_entry>::get()
{
    return m_entry.get();
}

template<class Hash_entry>
inline typename hashed_object_handle<Hash_entry>::value_type const*
hashed_object_handle<Hash_entry>::operator->() const
{
    return m_entry.get();
}

template<class Hash_entry>
inline typename hashed_object_handle<Hash_entry>::value_type*
hashed_object_handle<Hash_entry>::operator->()
{
    return m_entry.get();
}

template<class Hash_entry>
inline bool hashed_object_handle<Hash_entry>::empty() const
{
    return m_entry.empty();
}

template<class Hash_entry>
inline void hashed_object_handle<Hash_entry>::remove()
{
    value_type* ptr = nullptr;
    m_entry.assign(ptr);
}

template<class Hash_entry>
inline void hashed_object_handle<Hash_entry>::assign(value_type* ptr)
{
    m_entry.assign(ptr);
};

//-----------------------------------------------------------------
//                      hashed_object_table
//-----------------------------------------------------------------
template<class V, class Alloc, class Storage>
inline hashed_object_table<V, Alloc, Storage>::hashed_object_table(size_t capacity)
 : m_storage(sizeof(value_type)), m_table(capacity)
{};

template<class V, class Alloc, class Storage>
inline hashed_object_table<V, Alloc, Storage>::~hashed_object_table()
{
    close();
};

template<class V, class Alloc, class Storage>
inline void hashed_object_table<V, Alloc, Storage>::close()
{
    m_table.close(false);
    m_storage.purge_memory();
};

template<class V, class Alloc, class Storage>
inline void hashed_object_table<V, Alloc, Storage>::clear()
{
    using func = typename hash_table::traverse_func;

    func f = [=](value_type* ptr) { this->destroy_obj(ptr); };
    m_table.traverse_items(f);

    m_table.close(false);
    m_storage.purge_memory();
};

template<class V, class Alloc, class Storage>
template<class Stack>
inline void hashed_object_table<V, Alloc, Storage>::clear(Stack& st)
{
    using func = typename hash_table::traverse_func;

    func f = [this, &st](value_type* ptr) { this->destroy_obj(ptr, st); };
    m_table.traverse_items(f);

    m_table.close(false);
    m_storage.purge_memory();
};

template<class V, class Alloc, class Storage>
template<class ... Args>
inline V hashed_object_table<V, Alloc, Storage>::get(const Args& ... args)
{
    using entry = typename hash_table::entry;
    entry ptr = m_table.get(args ...);

    if (!ptr.empty())
    {
        V::type_traits::copy(*ptr);
        return V::make(*ptr);
    }
    else
    {
        value_type* m_str = register_obj(args ...);
        ptr.assign(m_str);
        return V::make(*ptr);
    };
};

template<class V, class Alloc, class Storage>
template<class ... Args>
inline V hashed_object_table<V, Alloc, Storage>::get_existing(const Args& ... args)
{
    using entry = typename hash_table::entry;
    entry ptr = m_table.get(args ...);

    if (!ptr.empty())
    {
        V::type_traits::copy(*ptr);
        return V::make(*ptr);
    }
    else
    {
        return V();
    };
};

template<class V, class Alloc, class Storage>
template<class ... Args>
inline typename hashed_object_table<V, Alloc, Storage>::handle_type 
hashed_object_table<V, Alloc, Storage>::find(const Args& ... args)
{
    using entry = typename hash_table::entry;
    entry ptr = m_table.get(args ...);

    return handle_type(ptr);
};

template<class V, class Alloc, class Storage>
template<class ... Args>
inline typename hashed_object_table<V, Alloc, Storage>::value_type* 
hashed_object_table<V, Alloc, Storage>::register_obj(Args&& ... args)
{   
    // ptr is not null; otherwise Alloc would throw
    void* ptr       = m_storage.malloc();

    new(ptr) value_type(std::forward<Args>(args) ...);
    return reinterpret_cast<value_type*>(ptr);
};

template<class V, class Alloc, class Storage>
inline void 
hashed_object_table<V, Alloc, Storage>::unregister_obj(value_type* ptr)
{
    using VT_nc = typename std::remove_const<value_type>::type;

    m_table.remove(ptr);
    const_cast<VT_nc*>(ptr)->~value_type();

    m_storage.free(const_cast<void*>(static_cast<const void*>(ptr)));
};

template<class V, class Alloc, class Storage>
template<class Stack>
inline void 
hashed_object_table<V, Alloc, Storage>::unregister_obj(value_type* ptr, Stack& st)
{
    using VT_nc = typename std::remove_const<value_type>::type;

    m_table.remove(ptr);
    const_cast<VT_nc*>(ptr)->release(st);    
    const_cast<VT_nc*>(ptr)->~value_type();

    m_storage.free(const_cast<void*>(static_cast<const void*>(ptr)));
};

template<class V, class Alloc, class Storage>
inline void hashed_object_table<V, Alloc, Storage>::destroy_obj(value_type* ptr)
{
    using VT_nc = typename std::remove_const<value_type>::type;

    const_cast<VT_nc*>(ptr)->~value_type();

    m_storage.free(const_cast<void*>(static_cast<const void*>(ptr)));
};

template<class V, class Alloc, class Storage>
template<class Stack>
inline void hashed_object_table<V, Alloc, Storage>::destroy_obj(value_type* ptr, Stack& st)
{
    using VT_nc = typename std::remove_const<value_type>::type;

    const_cast<VT_nc*>(ptr)->release(st);
    const_cast<VT_nc*>(ptr)->~value_type();

    m_storage.free(const_cast<void*>(static_cast<const void*>(ptr)));
};

template<class V, class Alloc, class Storage>
template<class Stack>
void hashed_object_table<V, Alloc, Storage>::remove(handle_type h, Stack& st)
{
    using VT_nc = typename std::remove_const<value_type>::type;

    if (h.empty() == true)
        return;

    const value_type* ptr   = h.get();

    h.remove();    

    const_cast<VT_nc*>(ptr)->release(st);
    const_cast<VT_nc*>(ptr)->~value_type();

    m_storage.free(const_cast<void*>(static_cast<const void*>(ptr)));
};

template<class V, class Alloc, class Storage>
void hashed_object_table<V, Alloc, Storage>::remove(handle_type h)
{
    using VT_nc = typename std::remove_const<value_type>::type;

    if (h.empty() == true)
        return;

    const value_type* ptr   = h.get();

    h.remove();    

    const_cast<VT_nc*>(ptr)->~value_type();

    m_storage.free(const_cast<void*>(static_cast<const void*>(ptr)));
};

template<class V, class Alloc, class Storage>
void hashed_object_table<V, Alloc, Storage>::set(handle_type h, const value_type& v)
{
    if (!h.empty())
    {        
        //delete exisitng node
        using VT_nc             = typename std::remove_const<value_type>::type;
        const value_type* ptr   = h.get();

        const_cast<VT_nc*>(ptr)->~value_type();
        m_storage.free(const_cast<void*>(static_cast<const void*>(ptr)));
    };

    value_type* m_str = register_obj(v);
    
    #if SYM_DAG_DEBUG_TERMS
        if (m_str == nullptr)
            throw std::runtime_error("error in hashed_object_table");
    #endif

    h.assign(m_str);
    return;
};

template<class V, class Alloc, class Storage>
void hashed_object_table<V, Alloc, Storage>::set(handle_type h, value_type&& v)
{
    if (!h.empty())
    {        
        //delete exisitng node
        using VT_nc             = typename std::remove_const<value_type>::type;
        const value_type* ptr   = h.get();

        const_cast<VT_nc*>(ptr)->~value_type();
        m_storage.free(const_cast<void*>(static_cast<const void*>(ptr)));
    };

    value_type* m_str = register_obj(std::move(v));
    
    #if SYM_DAG_DEBUG_TERMS
        if (m_str == nullptr)
            throw std::runtime_error("error in hashed_object_table");
    #endif

    h.assign(m_str);
    return;
};

template<class V, class Alloc, class Storage>
inline double hashed_object_table<V, Alloc, Storage>::reuse_stats() const
{
    size_t N                        = m_table.capacity();
    const value_type* const* ptr    = m_table.get_entries();

    double M = 0;

    double K = 0;
    for (size_t i = 0; i < N; ++i)
    {
        const value_type* elem = ptr[i];
        if (elem > (const value_type*)1)
        {
            K += elem->refcount();
            M += 1;
        };
    };
    return K/(M+1e-5);
};

template<class V, class Alloc, class Storage>
void hashed_object_table<V, Alloc, Storage>::print_reuse_stats(std::ostream& os)
{
    double k = this->reuse_stats();
    os << std::string(4,' ') << "tag: " << typeid(value_type).name() << "\n";
    os << std::string(8,' ') << "value: " << k << "\n";
};

template<class V, class Alloc, class Storage>
void hashed_object_table<V, Alloc, Storage>::print_memory_stats
                            (std::ostream& os, memory_stats& stats)
{
    size_t size = this->size();
    size_t cap  = this->capacity();    

    stats.m_bytes_hash  += size * sizeof(V);

    os << std::string(4,' ') << "tag: " << typeid(value_type).name() << "\n";
    os << std::string(8,' ') << "size: " << size << " capacity: " << cap << "\n";
};

template<class V, class Alloc, class Storage>
void hashed_object_table<V, Alloc, Storage>::print_collisions(std::ostream& os)
{
    double k = this->collisions();
    os << std::string(4,' ') << "tag: " << typeid(value_type).name() << "\n";
    os << std::string(8,' ') << "value: " << k << "\n";
};

//-----------------------------------------------------------------
//                      unique_object_table
//-----------------------------------------------------------------
template<class V, class Alloc, class Storage>
inline unique_object_table<V, Alloc, Storage>::unique_object_table(size_t capacity)
    :m_storage(sizeof(value_type))
{
    (void)capacity;
};

template<class V, class Alloc, class Storage>
inline unique_object_table<V, Alloc, Storage>::~unique_object_table()
{
    close();
};

template<class V, class Alloc, class Storage>
inline void unique_object_table<V, Alloc, Storage>::close()
{
    m_storage.purge_memory();
};

template<class V, class Alloc, class Storage>
template<class ... Args>
inline V unique_object_table<V, Alloc, Storage>::get(const Args& ... args)
{
    value_type* ptr = register_obj(args ...);
    return V::make(ptr);
};

template<class V, class Alloc, class Storage>
template<class ... Args>
inline typename unique_object_table<V, Alloc, Storage>::value_type* 
unique_object_table<V, Alloc, Storage>::register_obj(const Args& ... args)
{   
    // ptr is not null; otherwise Alloc would throw
    void* ptr   = m_storage.malloc();
    
    new(ptr) value_type(args ...);
    return reinterpret_cast<value_type*>(ptr);
};

template<class V, class Alloc, class Storage>
inline void unique_object_table<V, Alloc, Storage>::unregister_obj(value_type* ptr)
{
    using VT_nc = typename std::remove_const<value_type>::type;

    const_cast<VT_nc*>(ptr)->~value_type();    
    allocator_type::free(const_cast<void*>(static_cast<const void*>(ptr)));
};

template<class V, class Alloc, class Storage>
template<class Stack>
inline void 
unique_object_table<V, Alloc, Storage>::unregister_obj(value_type* ptr, Stack& st)
{
    using VT_nc = typename std::remove_const<value_type>::type;

    const_cast<VT_nc*>(ptr)->release(st);    
    const_cast<VT_nc*>(ptr)->~value_type();    

    m_storage.free(const_cast<void*>(static_cast<const void*>(ptr)));
};

template<class V, class Alloc, class Storage>
void unique_object_table<V, Alloc, Storage>::print_reuse_stats(std::ostream& os)
{
    (void)os;
};

template<class V, class Alloc, class Storage>
void unique_object_table<V, Alloc, Storage>::print_memory_stats
                                (std::ostream& os, memory_stats&)
{
    (void)os;
};

template<class V, class Alloc, class Storage>
void unique_object_table<V, Alloc, Storage>::print_collisions(std::ostream& os)
{
    (void)os;
};

};};