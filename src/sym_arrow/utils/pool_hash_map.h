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

#include "dag/refptr.h"
#include <boost/pool/object_pool.hpp>

namespace sym_arrow { namespace utils { namespace details
{

template<class Key, class Value, class Hash_equal>
class hash_map_data
{
    private:
        Key             m_key;
        Value           m_value;

    public:
        hash_map_data(const Key& key, const Value& v);
        hash_map_data(const Key& key, Value&& v);

        size_t          hash_value() const;
        bool            equal(const Key& other) const;
        static size_t   eval_hash(const Key& k);
        const Value&    get_value() const;
        Value&          get_value();

        template<class Stack>
        void            release(Stack& st);
};

}}}

namespace sym_arrow { namespace utils
{

struct expr_hash_equal
{
    static size_t hash_value(ast::expr_handle h)
    { 
        return (size_t)h;
    };

    static bool equal(ast::expr_handle h1, ast::expr_handle h2)
    {
        return h1 == h2;
    }
};

template<class Key, class Value, class Hash_equal>
class pool_hash_map
{
    public:
        using value_type    = Value;
        using key_type      = Key;

    private:
        using impl_type     = details::hash_map_data<Key, Value, Hash_equal>;
        using track_traits  = sym_dag::details::not_tracked_traits<impl_type>;
        using ptr_type      = sym_dag::refptr<impl_type, track_traits>;
        using alloc_type    = sym_dag::details::symbolic_allocator<ast::term_tag>;

        using table         = sym_dag::details::hashed_object_table
                                <ptr_type, alloc_type>;

    public:
        using handle_type   = typename table::handle_type;

    private:
        table               m_table;

    private:
        pool_hash_map(const pool_hash_map&) = delete;
        pool_hash_map& operator=(const pool_hash_map&) = delete;

    public:
        pool_hash_map();
        ~pool_hash_map();

        // return number of elements in the table
        size_t              size() const;

        // get handle to an object given by key; this handle can be
        // later passed to other functions
        handle_type         find(const key_type& key);

        // insert new element to the table
        void                insert(const key_type& k, const value_type& v);
        void                insert(const key_type& k, value_type&& v);

        // insert new element to the table at location pointed by h;
        // h must be returned by previous call to find(key), with key = k
        // and no other operations on the table was performed
        void                insert(handle_type h, const key_type& k, 
                                const value_type& v);
        void                insert(handle_type h, const key_type& k, 
                                value_type&& v);

        void                remove(handle_type h);

        template<class Stack_type>
        void                remove(handle_type h, Stack_type& st);

        // remove all elements; destructors are called
        void                clear();

        template<class Stack>
        void                clear(Stack& st);

        // release all memory; object destructors are not called
        void                close();
};

};};

#include "sym_arrow/utils/pool_hash_map.inl"
