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

#include "dag/details/hash_equal.h"
#include "dag/details/hash_table/hash_table.h"

#include <boost/pool/pool.hpp>
#include <vector>

namespace sym_dag { namespace details
{

struct memory_stats;

// pool allocator for dag nodes
template<class Allocator>
class object_allocator
{
    private:
        using pool          = boost::pool<Allocator>;

    private:
        pool                m_pool;

    public:
        // costructor of allocator for objects of sizeof = size
        object_allocator(size_t size);
        
        // deallocate previously created object; ptr != nulltpr
        void                free(void* ptr);

        // allocate size bytes; return nullptr if out of memory
        void*               malloc();

        // release all memory
        void                purge_memory();
};

// reference to an element stored in hashed_object_table
template<class Hash_entry>
class hashed_object_handle
{
    public:
        using value_type    = typename Hash_entry::value_type;

    private:
        Hash_entry          m_entry;

    public:
        hashed_object_handle(const Hash_entry& entry);

        template<class Ptr_type, class Allocator, class Storage>
        friend class hashed_object_table;

    public:
        // return stored element
        const value_type*   get() const;
        value_type*         get();

        // return true if stored element is nullptr
        bool                empty() const;

        // access to stored element
        const value_type*   operator->() const;
        value_type*         operator->();

    private:
        // remove object pointed by this handle from the table
        void                remove();

        // assign value ptr to object pointed by this handle
        // if ptr == 0 then existing object is removed from the table
        // if ptr != 0, existing object is replaced by value ptr;
        //  requires then hash value of ptr must be equal to hash
        //  value of existing object (or an object that could be stored
        //  in this place if existing object in nullptr)
        void                assign(value_type* ptr);
};

// hash table and memory allocator for dag nodes, that 
// require hashing
template<class Ptr_type, class Allocator, class Storage = object_allocator<Allocator>>
class hashed_object_table
{
    private:
        using VT0               = typename Ptr_type::value_type;
        using value_type        = typename std::remove_pointer<VT0>::type;
        
        using ptr_type          = Ptr_type;
        using hasher            = obj_hasher<value_type>;
        using equaler           = obj_equaler<value_type>;
        using storage_type      = Storage;
        using hash_table        = hash_table<value_type, hasher, equaler>;
        using hash_entry        = typename hash_table::entry;

    public:
        using handle_type       = hashed_object_handle<hash_entry>;

    private:
        storage_type        m_storage;
        hash_table          m_table;        

        hashed_object_table(const hashed_object_table&) = delete;
        hashed_object_table& operator=(const hashed_object_table&) = delete;

        template<class ... Args>
        value_type*         register_obj(Args&& ... args);

    public:
        // costructor; capacity is the initial capacity of hash table
        hashed_object_table(size_t capacity = 0);

        // destructor; release all memory
        ~hashed_object_table();

        // get existing object or create new one (args are passed to
        // construct); perform hashing
        template<class ... Args>
        ptr_type            get(const Args& ... args);

        // get existing object or return nullptr
        template<class ... Args>
        ptr_type            get_existing(const Args& ... args);

        // get handle to object; this handle can be later passed
        // to other functions
        template<class ... Args>
        handle_type         find(const Args& ... args);

        // destroy previously created object; ptr != nullptr
        void                unregister_obj(value_type* ptr);

        // destroy previously created object; ptr != nullptr
        template<class Stack>
        void                unregister_obj(value_type* ptr, Stack& st);

        // remove object from the table
        template<class Stack>
        void                remove(handle_type h, Stack& st);
        void                remove(handle_type h);

        // set an object at location pointed by a handle h;
        void                set(handle_type h, const value_type& v);
        void                set(handle_type h, value_type&& v);

        // number of allocated objects
        size_t              size() const                    { return m_table.size(); };

        // current capacity of the hash table
        size_t              capacity() const                { return m_table.capacity(); };

        // average refcount of stored objects
        double              reuse_stats() const;

        // percent of collisions during table searches 
        double              collisions() const              { return m_table.collisions();};
        
        // remove all elements; destructors are called
        void                clear();

        template<class Stack>
        void                clear(Stack& st);

        // release all memory; object destructors are not called
        void                close();

        // print different statistics
        void                print_reuse_stats(std::ostream& os);
        void                print_memory_stats(std::ostream& os, memory_stats& stats);
        void                print_collisions(std::ostream& os);

    private:
        void                destroy_obj(value_type* ptr);

        template<class Stack>
        void                destroy_obj(value_type* ptr, Stack& st);
};

// hash table and memory allocator for dag nodes, that 
// do not require hashing
template<class Ptr_type, class Allocator, class Storage = object_allocator<Allocator>>
class unique_object_table
{
    private:
        using VT0               = typename Ptr_type::value_type;
        using value_type        = typename std::remove_pointer<VT0>::type;
        
        using ptr_type          = Ptr_type;
        using storage_type      = Storage;

    private:
        unique_object_table(const unique_object_table&) = delete;
        unique_object_table& operator=(const unique_object_table&) = delete;

        template<class ... Args>
        value_type*         register_obj(const Args& ... args);        

    private:
        storage_type        m_storage;

    public:
        // constructor; capacity argument is not used
        unique_object_table(size_t capacity = 0);

        // destructor; release all memory
        ~unique_object_table();

        // create new object
        template<class ... Args>
        ptr_type            get(const Args& ... args);

        // destroy previously created object; ptr != nullptr
        void                unregister_obj(value_type* ptr);

        // destroy previously created object; ptr != nullptr
        template<class Stack>
        void                unregister_obj(value_type* ptr, Stack& st);

        // release all memory; object destructors are not called
        void                close();

        // these functions have empty implementations
        void                print_reuse_stats(std::ostream& os);
        void                print_memory_stats(std::ostream& os, memory_stats& stats);
        void                print_collisions(std::ostream& os);
};

// helper class; select memory allocator
template<class Ptr_type>
struct need_hash
{
    private:
        using VT0           = typename Ptr_type::value_type;
        using value_type    = typename std::remove_pointer<VT0>::type;

    public:
        static const bool value = value_type::do_hashing;
};

// perform allocations and deallocations of dag nodes
template<class Ptr_type, class Allocator, bool Need_hash = need_hash<Ptr_type>::value>
class object_table;

template<class Ptr_type, class Allocator>
class object_table<Ptr_type, Allocator, false> 
    : public unique_object_table<Ptr_type, Allocator>
{};

template<class Ptr_type, class Allocator>
class object_table<Ptr_type, Allocator, true> 
    : public hashed_object_table<Ptr_type, Allocator>
{};

};};