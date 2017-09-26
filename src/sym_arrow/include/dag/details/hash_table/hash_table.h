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

#include <new>
#include "dag/details/hash_table/hash_table_details.h"

namespace sym_dag
{

// default implementation of equality test; call operator == defined
// for compared elements
struct default_equaler
{
    template<class T1, class T2>
    bool operator()(const T1* ptr1, size_t hash_value, const T2* ptr2) const
    {
        (void)hash_value;
        return *ptr1 == *ptr2;
    };
};

// default implementation of hashing operator; call boost::hash
struct default_hasher
{
    template<class T>
    size_t operator()(const T* ptr) const
    { 
        return boost::hash<T>()(*ptr);
    };
};

// implements memory operations required by hash table on stored elements;
// on default ignore all operations; 
template<class T>
struct default_track_value
{
    static void     free(T* ptr)            {(void)ptr;};
    static void     copy(T* ptr)            {(void)ptr;};
    static void     assign(T* to, T* from)  {(void)from;(void)to;};
};

// allocator used by hash table to manage internal memory
struct default_allocator
{
    static void* malloc(size_t bytes)
    { 
        void * ptr = ::malloc(bytes); 
        if (ptr == nullptr)
            throw std::bad_alloc();

        return ptr;
    }
    
    static void free(void* ptr)
    { 
        ::free(ptr); 
    };
};

// reference to an element stored in hash table; VT is the type
// of elements stored in the table (hash table stores elements of type VT*);
template<class VT, class HT, class track_value>
class hash_entry
{
    public:
        using value_type        = VT;

    private:
        hash_entry(HT& value,details::ht_base*);

        HT*	                    m_value;
        details::ht_base*       m_owner;
        
        template<class S,class A1,class A2,class A3, class A4> 
        friend class hash_table;

    public:
        // trivial copy constructor and assignment
        hash_entry(const hash_entry& other);		
        hash_entry& operator=(const hash_entry& other);

        // dereference; return stored element;
        // require empty() == false
        VT*		        operator*() const;
        VT*		        operator->() const;

        // return true if stored element is nullptr
        bool			empty() const;

        // return stored element; require empty() == false
        VT*             get() const;

        // assign is allowed only for nonconst objects, T == VT
        // if ptr == 0 then object is removed
        // if ptr != 0, then hash value of ptr must be equal to hash 
        // value of given object
        template<class T>
        void			assign(T* ptr, typename details::assign_enabler<T,HT>::type = 0);
};


template<class T,class hasher,class equaler,class track_value,class allocator> 
class hash_table;

// hash table storing pointers to T; memory allocations must be performed
// by a user, template argument track_value helps to do this
template<class T,class hasher_ = default_hasher,class equaler_ = default_equaler,
         class track_value = default_track_value<T>, class allocator = default_allocator>
class hash_table : private details::ht_base
{
    public:
        using value_type        = T;
        using const_value_type  = const T;
        using hasher            = hasher_;
        using equaler           = equaler_;
        using entry             = hash_entry<T,T*,track_value>;
        using const_entry       = hash_entry<const T,const T* const,track_value>;
        using allocator_type    = allocator;

        using const_traverse_func   = std::function<void (const_value_type*)>;
        using traverse_func     = std::function<void (value_type*)>;

    private:
        using ht_capacity       = details::ht_capacity_pow2;

    private:
        ht_capacity             m_capacity;
        mutable size_t			n_searches;
        mutable size_t			n_collisions;

        value_type**			entries;
        hasher					hash_functor;
        equaler					eq_functor;
        
        void					change_size(bool expand);
        void					check_expand();
        virtual void            check_shrink() override;

        template<bool check_equality, class ... Key_args>
        value_type*&	        find_entry_impl(const Key_args& ... key) const;

        template<class ... Key_args>
        bool        	        eval_eq_functor(details::check_assign_true, const value_type* elem, 
                                    size_t hash, const Key_args& ... key) const;
        
        bool        	        eval_eq_functor(details::check_assign_false, const value_type* elem, 
                                    size_t hash, const value_type* other) const;

        void                    remove_delete_marks(value_type** pos,value_type** first_deleted) const;
        void                    remove_delete_marks(value_type** pos) const;        

    public:
        // create a hash table with initial capacity size
        hash_table(size_t size = 10, const hasher& hash_func = hasher(), const equaler& eq_func = equaler());

        // destructor; the free method from track_value type is called
        // on each stored element
        ~hash_table();

        // copy constructor; content is copied
        hash_table(const hash_table& other);

        // assignment; this table is closed and filled with content from other
        hash_table& operator=(const hash_table& other);

        // remove all entries but keep capacity unchanged; if call_destructors 
        // is true, then the free method from track_value type will be called
        // on each stored element
        void					clear(bool call_destructors = true);        
        
        // remove all entries from the table and free memory
        void					close(bool call_destructors = true);

        // finds element with specific key;
        template<class ... Key_args>
        const value_type*		find(const Key_args& ... key) const;
        
        // finds element with specific key (non const access)
        template<class ... Key_args>
        value_type*				find(const Key_args& ... key);
        
        // get reference to element with specified key; throw this
        // reference this element can be modified, or removed
        template<class ... Key_args>
        entry					get(const Key_args& ... key);
        
        template<class ... Key_args>
        const_entry				get(const Key_args& ... key) const;

        // check if element with specified key exists in the table
        template<class ... Key_args>
        bool					exist(const Key_args& ... key) const;
        
        // remove element with specified key
        template<class ... Key_args>
        void					remove(const Key_args& ... key);
        
        // insert new value to the table
        void					insert(value_type* key);		

        // return number of elements in the table
        size_t					size() const;
        
        // return table capacity i.e. number of elements that
        // can be stored without allocating additional memory
        size_t                  capacity() const        { return m_capacity.value(); };               
        
        // percent of collisions during table searches 
        double					collisions() const;

        // call function f for each element in the table
        void                    traverse_items(const const_traverse_func& f) const;
        void                    traverse_items(const traverse_func& f);

    public:
        // internal use only
        const value_type* const*get_entries() const     { return entries; };
};

};

#include "dag/details/hash_table/hash_table.inl"