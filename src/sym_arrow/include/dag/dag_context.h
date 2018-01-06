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

#include "dag/dag_item.h"
#include "dag/details/allocator.h"
#include "dag/dag_ptr.h"
#include "dag/details/object_table.h"
#include "dag/details/memory_manager.h"
#include "dag/details/dag_context_details.h"
#include "dag/details/vector_provider.h"
#include "dag/details/global_objects.h"

#include <map>

#pragma warning(push)
#pragma warning(disable: 4251)  //needs to have dll-interface to be used by clients

namespace sym_dag
{

// forward declaration
template<class Tag>
class dag_context;

// default implementation of additional data stored in dag_context
class context_data_base
{
    public:
        virtual ~context_data_base(){};

        // called after construction of dag_context when context
        // data is accessed first time
        virtual void    initialize() {};

        // called when dag_context::close() function is called 
        // and when destructor of dag_context is called
        virtual void    close() {};
};

// class responsible for allocations, hashing and deallocations
// for dag with given Tag
template<class Tag>
class dag_context : public details::dag_context_base
{
    private:
        static const size_t n_tables  = details::get_number_tables<Tag>::value;
        using table_type              = details::object_tables<Tag, n_tables>;
        using table_weak              = details::object_table_weak<Tag>;
        using context_data            = typename dag_context_data<Tag>::context_data_type;
        using weak_node_ptr           = typename table_weak::ptr_type;
        using track_function          = typename dag_item_base<Tag>::track_function;
        using track_func_map          = std::map<size_t, track_function>;

    public:
        // vector of nodes to be destroyed
        using stack_type        = sym_dag::details::release_stack<dag_item_base<Tag>>;
        using handle_type       = const dag_item_base<Tag>*;
        using stack_handle      = typename stack_type::handle_type;

    private:
        static dag_context<Tag>*m_global;
        context_data_base*      m_context_data;
        table_type              m_tables; 
        table_weak              m_table_weak;
        track_func_map          m_track_functions;
        details::mem_manager    m_mem_manager;
        size_t                  m_allocated;

    private:
        dag_context();
        ~dag_context();

        dag_context(const dag_context&) = delete;
        dag_context& operator=(const dag_context&) = delete;        

        // release memory; destructors are not called
        virtual void            close() override;

        // destroy context data; all context data must be
        // destroyed before any of dag_contexts
        virtual void            close_context_data() override;

        friend global_objects;

    public:
        // get global memory manager
        static dag_context&     get();

        // get context data associated with given type
        context_data&           get_context_data();
        const context_data&     get_context_data() const;

        // return a stack of objects; this stack should be passed to
        // unregister functions; one can also create this stack directly
        // but this is more costly; release_stack function must be called
        // before destruction of returned stack_handle
        stack_handle            get_stack();

        // destroy previously created object; ptr != nullptr
        template<class Node_type>
        void                    unregister(Node_type* h);

        template<class Node_type>
        void                    unregister_without_stack(Node_type* h);

        // destroy previously created object; ptr != nullptr
        template<class Node_type>
        void                    unregister(Node_type* h, stack_type& stack);

        // create new object; perform hashing if necessary
        template<class Node_type, class ... Args>
        dag_ptr<Node_type>      get(const Args& ... args);

        // allocatate given number of bytes; throw exception if allocation fails
        void*                   malloc(size_t bytes);

        // deallocation of previously allocated array of given size in bytes
        // ptr != nullptr
        void                    free(void* ptr, size_t bytes);

        //-------------------------------------------------------------------
        //                      object tracking
        //-------------------------------------------------------------------
        // register new tracking function and associate a tag to this function
        void                    add_tracking_function(size_t tag, const track_function& f);

        // unregister a tracking function with given tag code
        void                    remove_tracking_function(size_t tag);

        //-------------------------------------------------------------------
        //                      weak pointers management
        //-------------------------------------------------------------------
        // destroy previously created object; ptr != nullptr
        void                    unregister_weak(details::weak_node<Tag>* h);

        // get weak node impl associated with given node
        weak_node_ptr           get_weak_ptr(handle_type h);

        //-------------------------------------------------------------------
        //                      debugging and profiling
        //-------------------------------------------------------------------
        // print different statistics
        virtual void            print_reuse_stats(std::ostream& os)  override;
        virtual void            print_memory_stats(std::ostream& os, 
                                    details::memory_stats& stats) override;
        virtual void            print_collisions(std::ostream& os) override;

    private:
        template<class Node_type>
        details::object_table<dag_ptr<Node_type>, details::symbolic_allocator<Tag>>&      
                                get_object_table();
        void                    remove_assigned_data(handle_type h, stack_type& st);
        void                    remove_assigned_data(handle_type h);
        void                    call_track_functions(handle_type h, stack_type& st);
        void                    call_track_functions(handle_type h);
        void                    remove_weak_ptr(handle_type h);
};

// base type of cache types; global caches must be destroyed
// before dag contexts
class SYM_DAG_EXPORT node_cache
{
    public:
        virtual ~node_cache(){};

        // function should destroy all nodes
        virtual void    clear() = 0;
};

// perform functions on all created dag_context
class SYM_DAG_EXPORT registered_dag_context
{
    private:
        using dag_vect      = std::vector<details::dag_context_base*>;
        using cache_vect    = std::vector<node_cache*>;

    private:
        dag_vect            m_dags;
        cache_vect          m_caches;

    private:
        registered_dag_context();
        ~registered_dag_context();

        registered_dag_context(const registered_dag_context&) = delete;
        registered_dag_context& operator=(const registered_dag_context&) = delete;

        void                register_dag(details::dag_context_base* dag);
        void                destroy();

        template<class Tag>
        friend class dag_context;
        friend global_objects;

    public:
        // get global object
        static registered_dag_context&
                            get();        

        // register cache, that must be cleared, when close() or
        // clear_cache() functions are called
        void                register_cache(node_cache* c);

        // release memory; destructors are not called; after this call
        // destructors of dag items cannot be called; however new dag items
        // can be created
        void                close();

        // clear all registered caches
        void                clear_cache();

        // print different statistics
        void                print_reuse_stats(std::ostream& os);
        void                print_memory_stats(std::ostream& os);
        void                print_collisions(std::ostream& os);

        // print memory leaks; close should be called first
        void                print_memory_leaks(std::ostream& os);
};

};

#pragma warning(pop)