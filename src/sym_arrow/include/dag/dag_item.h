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

#include "dag/dag_ptr.h"
#include "dag/details/dag_header.h"
#include "dag/details/release_stack.h"

#include <vector>
#include <functional>

namespace sym_dag
{

// forward declaration
template<class Tag>
class dag_context;

// base type of additional data stored in dag_context
class context_data_base;

// default type of additional user data stored in dag_item
class dag_data_base
{};

// assign unique codes from 0 to last_code to types Node_type
// which derive from dag_item; the user must provide specializations
// of this template for each node type; 
// last_code < dag_tag_traits<Tag>::num_codes
template<class Tag, class Node_type>
struct dag_node_to_code  
{
    // must implement:
    // static const size_t code    = [some unique value];
};

// convert code to type; this is the inverse mapping to dag_node_to_code, i.e.
// dag_code_to_node<Tag, dag_node_to_code<Tag, Node>::code>::type = Node
// for all node types Node, that belongs to dag type with tag Tag
// the user must provide specializations of this template for each code
// for 0 to last_code 
template<class Tag, int Code>
struct dag_code_to_node
{
    // must implement:
    // using type  = [some type]
};

// configure dag type with given tag Tag; this template must be specialized
// by the user;
// user_flag_bits - number of bits if the user flag in dag_item; this number
//     should be small, at most three
// number_codes - number of possible codes associated to nodes of dag type with
//     given Tag; number_codes = last_code + 1
template<class Tag>
struct dag_tag_traits
{
    // must implement:
    // static const size_t number_codes    = [some value]
    // static const size_t user_flag_bits  = [some value]
};

// configure additional data stored in dag_context for given tag Tag; 
// this template can be specialized by the user;
// context_data_type - type of data stored in given dag_context
//     this type must be derived from context_data_base
template<class Tag>
struct dag_context_data
{
    // additional data stored in the dag_context
    using context_data_type = context_data_base;
};

// configure additional data stored in dag_item for given tag Tag; 
// this template can be specialized by the user;
// type - type of data stored in given dag_item
template<class Tag>
struct dag_node_data
{
    // additional data stored in the dag_item
    using type = dag_data_base;
};

// base class of all nodes, that can be used in a DAG representation
// of a symbolic expression; this class is responsible for memory
// management and function dispatching based on on a code associated
// with a real node (i.e. a derived class);
// Tag argument is used to differentiate different dag types
// this class is not thread safe
template<class Tag>
class dag_item_base
{
    public:
        // vector of nodes to be destroyed
        using stack_type    = details::release_stack<dag_item_base<Tag>>;

        // type of additional user data
        using user_data     = typename dag_node_data<Tag>::type;

        // tag argument supplied to this type
        using tag_type      = Tag;

        // type of dag_context that is responsible for memory
        // management of this type
        using context_type  = dag_context<Tag>;

        // type of tracking functions
        using track_function = std::function<void (const dag_item_base*, stack_type& st)>;

    private:
        using header_type   = details::dag_item_header<Tag, user_data>;

    private:
        mutable header_type m_data;

        friend details::dag_item_traits<Tag>;
        friend dag_context<Tag>;

    public:
        // initialization based on a type of derived class; t argument
        // is not used; equivalent to dag_item_base(code), where code 
        // is given by: code = dag_node_to_code<Node_type>::code
        template<class Node_type>
        dag_item_base(const Node_type* t);

        // virtual destructor added when requested for debugging purpose only
        #if SYM_DAG_POLYMORPHIC
            virtual ~dag_item_base(){};
        #endif

        // initialization based on a code associated with a derived class
        explicit dag_item_base(size_t code);

        // return reference count of this item
        size_t              refcount() const;

        // increase reference counter by one
        void                increase_refcount() const;

        // decrease reference counted by one; return true is the reference
        // count drops to zero and false otherwise
        bool                decrease_refcount() const;

        // return code associated with this node
        size_t              get_code() const;

        // mark this node as a temporary object if t = true and as nontemporary
        // object if t = false; a temporary and unique (i.e. refcount() == 1)
        // object can be modified inplace; if in given function this node is
        // marked as temporary, then must be marked as nontemporary at the 
        // function exit, otherwise this node can be accidentally destroyed;
        // only rvalue references should be marked as a temporary objects
        void                make_temporary(bool t) const;

        // return true is this object is marked as temporary and reference
        // count is 1; such object can be modified inplace
        bool                is_temporary() const;

        // return true if code of this item is the same as code assotiated
        // with the type Term, i.e. get_code() ==  dag_node_to_code<Term>::code
        template<class Term>
        bool                isa() const;

        // cast this node to const Term* type; isa<Term>() must return true
        template<class Term>
        const Term*         static_cast_to() const;

        // set of the user flags at position Bit to 1 (if val = true) or 0, 
        // where 0 <= Bit < MAX, and MAX = SYMBOLIC_FLAG_BITS_IN_DAG_ITEM 
        // (macro defined in the config file); 
        // user flag can be modified even for constant nodes; two nodes with
        // different user flags but otherwise equal should be considered as
        // eval
        template<size_t Bit>
        void                set_user_flag(bool val) const;

        // get value of the the use flags at position BIT, return true if 
        // this flag is set; see also set_user_flag
        template<size_t Bit>
        bool                get_user_flag() const;

        // get additional user data; type of this data is configured by
        // dag_node_data<Tag> class
        user_data&          get_user_data() const;

        //-------------------------------------------------------------------
        //                      object tracking
        //-------------------------------------------------------------------
        // when object is tracked; then a tracking function is called, when
        // this object is about to be destroyed, there may exist many tracking
        // functions

        // return true if this object is tracked
        bool                is_tracked() const;

        // start or stop tracking this object
        void                set_tracked(bool val) const;

        // register new tracking function and associate a tag to this function
        static void         add_tracking_function(size_t tag, const track_function& f);

        // unregister a tracking function with given tag code
        static void         remove_tracking_function(size_t tag);

    private:
        void                set_has_weak_ptr() const;
        bool                has_assigned_data() const;
        bool                has_weak_ptr() const;
};

// all dag nodes must be derived from this type
// Derived - type of derived class
// Tag - unique identifier used to differentiate dag types
// hash_node - if true, then objects of type Derived should be
//     hashed, i.e. equal objects should have the same address;
//
// allocations and deallocations of objects of type Derived cannot
// be done explicitly, but must be performed by dag_context
// class. New objects should be created by the make(...) function;
// deallocations are performed implicitely by destructor of dag_ptr
// class.
//
// if hash_node = true; then Derived class must implement functions:
//     // some constructor
//     Derived(args ... );
//
//     // function returning a hash value ; args is the same list
//     // of arguments as in one of constructors
//     static size_t   eval_hash(args ...);
//
//     // return true if this object is equal to Derived(args2...)
//     // in this case eval_hash(args ...) == eval_hash(args2 ...)
//     // user flags and user data should not be used in equality tests
//     // since they can change in any time
//     bool            equal(args2 ...) const;
//
//     // return hash value computed by eval_hash(args ...)
//     std::size_t     hash_value() const;
//
//     // if other dag nodes are referred by this object; then all directly
//     // referred dag nodes should be released (by dag_ptr::release() function)
//     // and pushed to stack; this is not stricly required, but in this way
//     // one avoids recursive calls to destructors, which can easily cause
//     // the stack overflow; after this function call, object destructor will
//     // be called
//     void            release(std::vector<dag_item_base<Tag>*>& stack);
//
// if hash_node = true; then Derived class must implement functions:
//
//     void            release(std::vector<dag_item_base<Tag>*>& stack);
//
template<class Derived, class Tag, bool hash_node>
class dag_item : public dag_item_base<Tag>
{
    public:
        // type of reference counted smart pointer storing objects
        // of type Derived 
        using ptr_type      = dag_ptr<Derived, Tag>;

    public:
        // value of hash_node argument supplied to this type
        static const bool do_hashing    = hash_node;

    public:
        // initialization based on a type of Derived class; t argument
        // is not used; equivalent to dag_item(code), where code is given by
        // code = dag_node_to_code<Node_type>::code
        dag_item(const Derived* t);

        // initialization based on a code associated with the Derived class
        explicit dag_item(size_t code);

        // create refcounted smart pointer; logically equivalent to 
        // ptr_type(new Derived(args...));
        // note that this is the only way to create an object of type Derived
        template<class ... Args>
        static ptr_type     make(const Args& ... args);
};

// weak pointer associated with dag_item
template<class Tag>
class weak_dag_item
{
    private:
        using base_type     = details::weak_node<Tag>;
        using impl_type     = refptr<const base_type, details::weak_node_traits<Tag>>;
        using dag_ptr       = refptr<const dag_item_base<Tag>, details::dag_item_traits<Tag>>;

    private:
        impl_type           m_impl;
    
    public:
        // uninitialized weak pointer
        weak_dag_item();

        // copy and move constructor
        weak_dag_item(weak_dag_item const& r);
        weak_dag_item(weak_dag_item&& r);

        // make weak pointer associated with exisitng shared pointed
        explicit weak_dag_item(dag_ptr const& r);

        // destructor
        ~weak_dag_item();

        // assignment and move assignment
        weak_dag_item&      operator=(weak_dag_item const & r);
        weak_dag_item&      operator=(weak_dag_item&& r);

        // checks whether the referenced object was already deleted 
        bool                expired() const;

        // creates a shared_ptr that manages the referenced object 
        dag_ptr             lock() const;
};

};
