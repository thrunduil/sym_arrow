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

namespace sym_dag { namespace details
{

// memory use statistics
struct memory_stats
{
    // total memory allocated by hashtables
    size_t      m_bytes_hash;

    // total memory allocated by memory managers
    size_t      m_bytes_mem;

    // total memory in use
    size_t      m_bytes_reserved;

    memory_stats()
        :m_bytes_hash(0), m_bytes_mem(0), m_bytes_reserved(0)
    {};
};

// class storing all required object tables
template<class Tag, int Code>
class object_tables : public object_tables<Tag, Code - 1>
{
    private:
        using Node_type = typename dag_code_to_node<Tag, Code>::type;
        using table     = object_table<dag_ptr<Node_type>, symbolic_allocator<Tag>>;

    public:
        table           m_table;

    public:
        void            close();
        void            print_reuse_stats(std::ostream& os);
        void            print_memory_stats(std::ostream& os, memory_stats& stats);
        void            print_collisions(std::ostream& os);
};

// empty object table
template<class Tag>
class object_tables<Tag, -1>
{
    public:
        void            close(){};
        void            print_reuse_stats(std::ostream&) {};
        void            print_memory_stats(std::ostream&, memory_stats&) {};
        void            print_collisions(std::ostream&) {};
};

// calculate required number of object tables
template<class Tag>
struct get_number_tables
{
    static const size_t num_codes   = dag_tag_traits<Tag>::number_codes;
    static const int value          = (num_codes > 0) ? num_codes - 1 : -1;
};

template<class Tag>
class weak_node
{
    private:
        using base_type = const dag_item_base<Tag>*;
        using dag_ptr   = refptr<const dag_item_base<Tag>, details::dag_item_traits<Tag>>;

    public:
        // tag used to create weak node from dag node
        struct create_tag{};

        // tag used to make weak node expired
        struct destroy_tag{};

    private:        
        mutable size_t  m_refcount;
        base_type       m_base;
        mutable bool    m_destroyed;        

        friend dag_context<Tag>;

    public:
        weak_node(base_type h, create_tag);

        static size_t   eval_hash(base_type h, create_tag);
        static size_t   eval_hash(base_type h, destroy_tag);

        size_t          hash_value() const;
        bool            equal(base_type h, create_tag) const;
        bool            equal(base_type h, destroy_tag) const;

        // return reference count
        size_t          refcount() const;

        // increase reference counter by one
        void            increase_refcount() const;

        // decrease reference counted by one; return true is the reference
        // count drops to zero and false otherwise
        bool            decrease_refcount() const;

        // checks whether the referenced object was already deleted 
        bool            expired() const;

        // creates a shared_ptr that manages the referenced object 
        dag_ptr         lock() const;

    private:
        void            mark_expired() const;
};

// class storing weak pointers
template<class Tag>
class object_table_weak
{
    public:
        using base_type = weak_node<Tag>;
        using ptr_type  = refptr<const base_type, details::weak_node_traits<Tag>>;
        using table     = hashed_object_table<ptr_type, symbolic_allocator<Tag>>;

    public:
        table           m_table;

    public:
        void            close();
        void            print_reuse_stats(std::ostream& os);
        void            print_memory_stats(std::ostream& os, memory_stats& stats);
        void            print_collisions(std::ostream& os);
};

// base class of memory managers
class dag_context_base
{
    public:
        // release memory; destructors are not called
        virtual void            close() = 0;

        // destroy context data
        virtual void            close_context_data() = 0;

        // print different statistics
        virtual void            print_reuse_stats(std::ostream& os) = 0;
        virtual void            print_memory_stats(std::ostream& os,
                                    memory_stats& stats) = 0;
        virtual void            print_collisions(std::ostream& os) = 0;
};

};};
