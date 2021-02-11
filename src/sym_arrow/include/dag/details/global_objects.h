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

#include "dag/config.h"
#include <vector>

namespace sym_dag { namespace details
{

class global_object_base
{
    public:
        virtual ~global_object_base() {};
};

template<class Type>
class global_object_type : public global_object_base
{
    private:
        Type*       m_ptr;

    public:
        global_object_type(Type* ptr)
            : m_ptr(ptr)
        {};

        virtual ~global_object_type() override;
};

}};

#pragma warning(push)
#pragma warning(disable : 4251) //needs to have dll-interface

namespace sym_dag
{

// lifetime management of global objects; to release memory
class SYM_DAG_EXPORT global_objects
{
    private:
        using item_type         = details::global_object_base;
        using vector_obj        = std::vector<item_type*>;

    private:
        vector_obj              m_objects_before;
        vector_obj              m_objects_context;
        vector_obj              m_objects_after;
        vector_obj              m_objects_last;

    private:
        global_objects();
        ~global_objects();

        global_objects(const global_objects&) = delete;
        global_objects& operator=(const global_objects&) = delete;

        static global_objects&  get_global();
        void                    close_impl();
        void                    destroy(vector_obj& vec);

        template<class Type>
        static void             delete_item(Type* ptr);

        template<class Type>
        friend class details::global_object_type;

        friend struct dag_initializer;

    public:
        // destroy all registered global data; after this call
        // no function from dag library can be called
        static void             close();

        // make_[type] functions create a pointer to type Type, args 
        // is a variable length size of additional arguments supplied
        // to the constructor f type Type; created object will be 
        // destroyed when global objects are destroyed or explicit call
        // to close function is made

        // create a pointer to type Type; this pointer will be deleted
        // before destruction of dag contexts
        template<class Type, class ... Args>
        static Type*            make_before(Args&&  ... args);

        // create a pointer to dag context
        template<class Type, class ... Args>
        static Type*            make_context(Args&&  ... args);

        // create a pointer to type Type; this pointer will be deleted
        // after destruction of dag contexts
        template<class Type, class ... Args>
        static Type*            make_after(Args&&  ... args);        

        // create a pointer to type Type; this pointer will be deleted
        // at the end
        template<class Type, class ... Args>
        static Type*            make_last(Args&&  ... args);
};

// initializer of dag library
struct SYM_DAG_EXPORT dag_initializer 
{
    dag_initializer();
    ~dag_initializer();
};

// static initializer for every translation unit
static dag_initializer dag_init;

};

#pragma warning(pop)

#include "dag/details/global_objects.inl"