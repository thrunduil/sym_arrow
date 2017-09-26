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

#include <malloc.h>

namespace sym_dag 
{
    
namespace details
{

template<class T>
struct mark_delete
{
    static const T value;
};

template<class T>
const T mark_delete<T>::value = (T)1;

//--------------------------------------------------------------------------
//                         ht_capacity_prime
//--------------------------------------------------------------------------
inline ht_capacity_prime::ht_capacity_prime(size_t size)
{
    m_pos = 0;

    while(get_value(m_pos) < size)
        ++m_pos;

    m_value = get_value(m_pos);
};

inline size_t ht_capacity_prime::value() const
{
    return m_value;
};

inline size_t ht_capacity_prime::project(size_t val) const
{
    return val % m_value;
};

inline ht_capacity_prime ht_capacity_prime::next_size() const
{
    size_t val = get_value(m_pos+1);
    return ht_capacity_prime(val);
};

inline ht_capacity_prime ht_capacity_prime::previous_size() const
{
    size_t val = get_value(m_pos > 0 ? m_pos-1 : 0);
    return ht_capacity_prime(val);
};

inline size_t ht_capacity_prime::get_value(size_t pos)
{
    static const size_t primes[] = 
    {
        3, 7, 11, 19, 37, 67, 131, 263, 521, 1031, 2053, 4099, 8209, 16411, 32771, 65539,
        131101, 262147, 524309, 1048583, 2097169, 4194319, 8388617, 16777259, 33554467, 
        67108879,  134217757, 268435459, 536870923, 1073741827, 2147483659, 4294967279 
    };

    return primes[pos];
};

//--------------------------------------------------------------------------
//                         ht_capacity_pow2
//--------------------------------------------------------------------------
inline ht_capacity_pow2::ht_capacity_pow2(size_t size)
{
    m_value = 4;

    while(m_value < size)
        m_value *= 2;

    m_mask = m_value - 1;
};

inline ht_capacity_pow2::ht_capacity_pow2(size_t val, int)
    :m_value(val), m_mask(val - 1)
{};

inline size_t ht_capacity_pow2::value() const
{
    return m_value;
};

inline size_t ht_capacity_pow2::project(size_t val) const
{
    size_t ret = val & m_mask;
    return ret;
};

inline ht_capacity_pow2 ht_capacity_pow2::next_size() const
{
    return ht_capacity_pow2(m_value * 2, 0);
};

inline ht_capacity_pow2 ht_capacity_pow2::previous_size() const
{
    if (m_value > min_size)
        return ht_capacity_pow2(m_value / 2, 0);
    else
        return ht_capacity_pow2(min_size, 0);
};

//--------------------------------------------------------------------------
//                         helpers 
//--------------------------------------------------------------------------
template<bool check_type>   
struct get_check_type{};

template<>
struct get_check_type<true>
{ 
    static check_assign_true eval() 
    { 
        return check_assign_true(); 
    };
};

template<>
struct get_check_type<false>
{ 
    static check_assign_false eval() 
    { 
        return check_assign_false(); 
    };
};

};

//--------------------------------------------------------------------------
//                         hash_entry 
//--------------------------------------------------------------------------
template<class V,class HT, class TV>
inline hash_entry<V,HT,TV>::hash_entry(HT& value,details::ht_base* owner)
:m_value(&value), m_owner(owner)
{};

template<class V,class HT,class TV>
inline hash_entry<V,HT,TV>& 
hash_entry<V,HT,TV>::operator=(const hash_entry& other)
{
    m_value		= other.m_value;
    m_owner	    = other.m_owner;
    return *this;
};

template<class V,class HT,class TV>
inline hash_entry<V,HT,TV>::hash_entry(const hash_entry& other)
:m_value(other.m_value), m_owner(other.m_owner)
{};

template<class V,class HT,class TV>
inline V* hash_entry<V,HT,TV>::operator*() const
{
    return *m_value;
};

template<class V,class HT,class TV>
inline V* hash_entry<V,HT,TV>::operator->() const
{
    return *m_value;
};

template<class V,class HT,class TV>
inline bool hash_entry<V,HT,TV>::empty() const
{
    if (*m_value <= details::mark_delete<HT>::value )
        return true;

    return false;
};

template<class V,class HT,class TV>
inline V* hash_entry<V,HT,TV>::get() const
{
    return *m_value;
}

template<class V,class HT,class TV>
template<class T>
void hash_entry<V,HT,TV>::assign(T* ptr,typename details::assign_enabler<T,HT>::type)
{
    if ( ptr )
    {
        if (*m_value == nullptr)
        {
            ++m_owner->n_elements;
            TV::copy(ptr);
        }
        else if (*m_value == details::mark_delete<HT>::value)
        {
            ++m_owner->n_elements;
            --m_owner->n_removed;
            TV::copy(ptr);
        }
        else
        {
            TV::assign(*m_value,ptr);
        };

        *m_value	= ptr;	
    }
    else
    {
        if (*m_value > details::mark_delete<HT>::value)
        {
            TV::free(*m_value);

            *m_value = details::mark_delete<HT>::value;
            ++m_owner->n_removed;
            --m_owner->n_elements;
            m_owner->check_shrink();
        };
    };
};

//--------------------------------------------------------------------------
//                         hash_table 
//--------------------------------------------------------------------------
template<class T,class H,class E,class TV,class A>
hash_table<T,H,E,TV,A>::hash_table(size_t size1, const H& hash_func, const E& eq_func)
:hash_functor(hash_func),eq_functor(eq_func), m_capacity(size1)
{
    size_t size     = m_capacity.value();

    //additional 1 element for addressing one element behind table in remove function
    //required by application verifier
    void* ptr       = allocator_type::malloc((size+1) * sizeof (value_type*));
    entries			= reinterpret_cast<value_type**>(ptr);
    
    n_elements		= 0;
    n_collisions	= 0;
    n_searches		= 0;
    n_removed		= 0;

    ::memset(entries, 0, size * sizeof (value_type*));
};

template<class T,class H,class E,class TV, class A>
inline hash_table<T,H,E,TV,A>::~hash_table()
{
    clear(true);
    allocator_type::free(entries);
};

template<class T,class H,class E,class TV,class A>
void hash_table<T,H,E,TV,A>::clear(bool call_destructors)
{	
    if (entries == nullptr)
        return;

    n_elements		    = 0;
    n_searches		    = 0;
    n_collisions	    = 0;
    n_removed		    = 0;

    value_type** entry_ptr;
    value_type** end    = entries + m_capacity.value();

    if (call_destructors == false)
    {
        for (entry_ptr = entries; entry_ptr < end; ++entry_ptr)
            *entry_ptr = nullptr;
    }
    else
    {
        for (entry_ptr = entries; entry_ptr < end; ++entry_ptr)
        {
            if (*entry_ptr > details::mark_delete<value_type*>::value )
                TV::free(*entry_ptr);

            *entry_ptr = nullptr;
        };
    };
}

template<class T,class H,class E,class TV,class A>
void hash_table<T,H,E,TV,A>::close(bool call_destructors)
{
    clear(call_destructors);
    allocator_type::free(entries);
    entries     = nullptr;
    m_capacity  = 0;

    *this = hash_table(0, hash_functor, eq_functor);
};

template<class T,class H,class E,class TV,class A>
void hash_table<T,H,E,TV,A>::traverse_items(const const_traverse_func& f) const
{
    if (entries == nullptr)
        return;

    value_type** entry_ptr;
    value_type** end    = entries + m_capacity.value();

    for (entry_ptr = entries; entry_ptr < end; ++entry_ptr)
    {
        if (*entry_ptr > details::mark_delete<value_type*>::value )
            f(*entry_ptr);

        *entry_ptr = nullptr;
    };
};

template<class T,class H,class E,class TV,class A>
void hash_table<T,H,E,TV,A>::traverse_items(const traverse_func& f)
{
    if (entries == nullptr)
        return;

    value_type** entry_ptr;
    value_type** end    = entries + m_capacity.value();

    for (entry_ptr = entries; entry_ptr < end; ++entry_ptr)
    {
        if (*entry_ptr > details::mark_delete<value_type*>::value )
            f(*entry_ptr);

        *entry_ptr = nullptr;
    };
}

template<class T,class H,class E,class TV,class A>
hash_table<T,H,E,TV,A>& hash_table<T,H,E,TV,A>::operator=(const hash_table& other)
{
    if (entries == other.entries)
        return *this;

    clear();
    allocator_type::free(entries);

    m_capacity      = other.m_capacity;
    n_elements      = other.n_elements;
    n_searches      = other.n_searches;
    n_collisions    = other.n_collisions;
    n_removed       = other.n_removed;

    hash_functor    = other.hash_functor;
    eq_functor      = other.eq_functor;

    //additional 1 element for addressing one element behind table in remove function
    //required by application verifier
    void* ptr       = allocator_type::malloc((m_capacity.value()+1)*sizeof(value_type*));
    entries         = reinterpret_cast<value_type**>(ptr);

    for(size_t i = 0; i < other.m_capacity.value();++i)
    {
        if (other.entries[i] > details::mark_delete<value_type*>::value )
            TV::copy(other.entries[i]);

        entries[i] = other.entries[i];
    };

    return *this;
};

template<class T,class H,class E,class TV,class A>
hash_table<T,H,E,TV,A>::hash_table(const hash_table& other)
{
    this->n_capacity      = other.n_capacity;
    this->n_elements      = other.n_elements;
    this->n_searches      = other.n_searches;
    this->n_collisions    = other.n_collisions;
    this->n_removed       = other.n_removed;

    this->hash_functor    = other.hash_functor;
    this->eq_functor      = other.eq_functor;

    //additional 1 element for addressing one element behind table in remove function
    //required by application verifier
    void* ptr             = allocator_type::malloc((this->n_capacity+1)*sizeof(value_type*));
    this->entries         = reinterpret_cast<value_type**>(ptr);

    for(size_t i = 0; i<this->n_capacity; ++i)
    {		
        if (other.entries[i] > details::mark_delete<value_type*>::value )
            TV::copy(this->entries[i],other.entries[i]);

        this->entries[i] = other.entries[i];
    };
};

template<class T,class H,class E,class TV,class A>
void hash_table<T,H,E,TV,A>::change_size(bool expand)
{
    size_t new_size;
    hash_table* new_htab;

    if (expand)
    {
        ht_capacity ns = this->m_capacity.next_size();
        new_size    = ns.value();

        void* ptr   = allocator_type::malloc(sizeof(hash_table)); 
        new_htab    = reinterpret_cast<hash_table*>(ptr);

        new(new_htab) hash_table(new_size,hash_functor, eq_functor);
    }
    else
    {
        ht_capacity ns = this->m_capacity.previous_size();
        new_size    = ns.value();
        void* ptr   = nullptr;

        try
        {
            ptr         = allocator_type::malloc(sizeof(hash_table)); 
            new_htab    = reinterpret_cast<hash_table*>(ptr);

            new(new_htab) hash_table(new_size,hash_functor, eq_functor);
        }
        catch(std::exception& )
        {
            // hash table is shrinking; if malloc fails, then we can stil
            // work on this table
            if (ptr)
                allocator_type::free(ptr);

            return;
        };
    };    

    new_htab->n_collisions	= n_collisions;
    new_htab->n_searches	= n_searches;

    size_t loc_delete   = 0;
    size_t loc_elems    = 0;
    value_type** end    = entries + m_capacity.value();

    for (value_type** entry_ptr = entries; entry_ptr < end; ++entry_ptr)
    {
        if (*entry_ptr == details::mark_delete<value_type*>::value)
        {
            ++loc_delete;
        }
        else if (*entry_ptr > details::mark_delete<value_type*>::value)
        {
            ++loc_elems;

            value_type*& new_entry_ptr	= new_htab->find_entry_impl<false,const value_type*>(*entry_ptr);
            new_entry_ptr = *entry_ptr;
            new_htab->n_elements++;
        };
    };

    #ifdef _DEBUG
        if (this->n_elements != loc_elems || this->n_removed != loc_delete)
        {
            assert(0);
        };
    #endif

    allocator_type::free(entries);

    m_capacity      = new_htab->m_capacity;
    n_elements      = new_htab->n_elements;
    n_searches      = new_htab->n_searches;
    n_collisions    = new_htab->n_collisions;
    n_removed       = new_htab->n_removed;

    entries         = new_htab->entries;
    hash_functor    = new_htab->hash_functor;
    eq_functor      = new_htab->eq_functor;

    allocator_type::free(new_htab);
};

template<class T,class H,class E,class TV,class A>
template<class ... Key_args>
bool hash_table<T,H,E,TV,A>::eval_eq_functor(details::check_assign_true, const value_type* elem, 
                                    size_t hash, const Key_args& ... key) const
{
    return eq_functor(elem,hash, key ...);
};

template<class T,class H,class E,class TV,class A>
bool hash_table<T,H,E,TV,A>::eval_eq_functor(details::check_assign_false, const value_type* elem, 
                                            size_t hash, const value_type* element) const
{
    (void)hash;
    (void)elem;
    (void)element;
    assert(elem != element);
    return false;
};

template<class T,class H,class E,class TV,class A>
template<bool check_equality, class ... Key_args>
typename hash_table<T,H,E,TV,A>::value_type*& 
hash_table<T,H,E,TV,A>::find_entry_impl(const Key_args& ... key) const
{
    size_t base_hash_value	= hash_functor(key ...);
    size_t hash_value		= m_capacity.project(base_hash_value);

    n_searches++;

    value_type** first_deleted  = nullptr;
    value_type** begin          = entries + hash_value;
    value_type** end            = entries + m_capacity.value();

    value_type** pos;

    for (pos = begin; ;++pos, ++n_collisions)
    {
        if (pos == end)
        {
            //move to first element
            pos = entries;
        };

        if (*pos == nullptr)
        {
            if (first_deleted != nullptr)
            {
                remove_delete_marks(pos,first_deleted);
                pos = first_deleted;
            }
            break;
        }
        else if (*pos != details::mark_delete<value_type*>::value)
        {
            if (eval_eq_functor(details::get_check_type<check_equality>::eval(), 
                                *pos,base_hash_value, key...))
            {
                break;
            }
        }
        else if (first_deleted == nullptr)
        {
            first_deleted = pos;
        };
    };

    return *pos;
};

template<class T,class H,class E,class TV,class A>
void hash_table<T,H,E,TV,A>::remove_delete_marks(value_type** pos,value_type** first_deleted) const
{
    value_type** begin = entries;

    for (;;)
    {
        if (pos == begin)
        {
            //move to last element
            pos = entries + m_capacity.value();
        };

        --pos;

        if (*pos != details::mark_delete<value_type*>::value)
            return;

        *pos = nullptr;
        --n_removed;

        if (pos == first_deleted)
            return;
    };
};

template<class T,class H,class E,class TV,class A>
void hash_table<T,H,E,TV,A>::remove_delete_marks(value_type** pos) const
{
    value_type** end    = entries + m_capacity.value();

    //*pos != nullptr
    if (pos + 1 < end && pos[1] == nullptr)
    {
        while (pos[0] == details::mark_delete<value_type*>::value)
        {
            --this->n_removed;
            *pos = nullptr;
            --pos;

            if (pos == entries)
                return;
        };

        return;
    };
};

template<class T,class H,class E,class TV,class A>
template<class ... Key_args>
inline typename hash_table<T,H,E,TV,A>::const_value_type const*
hash_table<T,H,E,TV,A>::find(const Key_args& ... key) const
{
    return find_entry_impl<true>(key ...);
}

template<class T,class H,class E,class TV,class A>
template<class ... Key_args>
inline typename hash_table<T,H,E,TV,A>::value_type*
hash_table<T,H,E,TV,A>::find(const Key_args& ... key)
{
    return find_entry_impl<true>(key ...);
};

template<class T,class H,class E,class TV,class A>
inline void hash_table<T,H,E,TV,A>::check_expand()
{
    if (m_capacity.value() <= n_elements*2 + n_removed )
        change_size(true);
};

template<class T,class H,class E,class TV,class A>
void hash_table<T,H,E,TV,A>::check_shrink()
{
    if (m_capacity.value() >= n_elements*4 && m_capacity.value() > ht_capacity::min_size)
        change_size(false);
};

template<class T,class H,class E,class TV,class A>
void hash_table<T,H,E,TV,A>::insert(value_type* element)
{
    check_expand();

    value_type* & ref = find_entry_impl<true>(element);

    if (ref == nullptr)
    {
        ++n_elements;
        TV::copy(element);
    }
    else if (ref == details::mark_delete<value_type*>::value)
    {
        ++n_elements;
        --n_removed;
        TV::copy(element);
    }
    else
    {
        TV::assign(ref,element);
    };

    ref	= element;
};

template<class T,class H,class E,class TV,class A>
template<class ... Key_args>
inline typename hash_table<T,H,E,TV,A>::entry 
hash_table<T,H,E,TV,A>::get(const Key_args& ... key)
{
    check_expand();
    value_type*& ref = find_entry_impl<true>(key ...);
    return entry(ref,(details::ht_base*)this);
};

template<class T,class H,class E,class TV,class A>
template<class ... Key_args>
inline typename hash_table<T,H,E,TV,A>::const_entry 
hash_table<T,H,E,TV,A>::get(const Key_args& ... key) const
{
    const_value_type* const& ref = find_entry_impl<true>(key ...);
    return const_entry(ref,(details::ht_base*)this);
};

template<class T,class H,class E,class TV,class A>
template<class ... Key_args>
inline void hash_table<T,H,E,TV,A>::remove(const Key_args& ... key)
{
    value_type*& ptr = find_entry_impl<true>(key ...);
    
    if (ptr)
    {
        n_removed++;
        n_elements--;
        TV::free(ptr);
        ptr = details::mark_delete<value_type*>::value;
        remove_delete_marks(&ptr);
        check_shrink();
    };
};

template<class T,class H,class E,class TV,class A>
template<class ... Key_args>
inline bool hash_table<T,H,E,TV,A>::exist(const Key_args& ... key) const
{
    value_type*& ptr = find_entry_impl<true>(key ...);
    return ptr? true : false;
};

template<class T,class H,class E,class TV,class A>
inline size_t hash_table<T,H,E,TV,A>::size() const
{
    return n_elements;
};

template<class T,class H,class E,class TV,class A>
inline double hash_table<T,H,E,TV,A>::collisions() const
{
    double searches	= double(n_searches);

    if (searches == 0)
        searches++;

    return n_collisions / searches;
};

};