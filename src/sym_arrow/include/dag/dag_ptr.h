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
#include "dag/details/release_stack.h"

namespace sym_dag
{
    // base class of dag nodes
    template<class Tag>
    class dag_item_base;
};

namespace sym_dag { namespace details
{

// implementation of the weak pointer
template<class Tag>
class weak_node;

// implementation of reference counting
template<class Tag>
class dag_item_traits
{
    private:
        using value_type    = dag_item_base<Tag>;

    public:
        using stack_type    = sym_dag::details::release_stack<value_type>;

        static void     check_free(const value_type* p);
        static void     copy(const value_type* p);
        static void     assign(const value_type* to, const value_type* from);

        static void     check_destroy(const value_type* p, stack_type& stack);        

    private:
        static void     call_free(const value_type* ptr);            
        static void     call_destroy(const value_type* ptr, stack_type& stack);
};

// implementation of reference counting for weak nodes
template<class Tag>
class weak_node_traits
{
    private:
        using value_type    = details::weak_node<Tag>;

    public:
        static void     check_free(const value_type* p);
        static void     copy(const value_type* p);
        static void     assign(const value_type* to, const value_type* from);

    private:
        static void     call_free(const value_type* ptr);
};

// reference counting not performed
template<class T>
struct not_tracked_traits
{
    public:
        static void check_free(const T*)        {};
        static void copy(const T*)              {};
        static void assign(const T*, const T*)  {};
};

}};

namespace sym_dag
{

// reference counted smart pointer to dag node types
// type T must be derived from dag_item<T, Tag2, hash_node>
// Tag must be the same as Tag2; Tag argument need not be supplied
// if T is a complete type
template<class T, class Tag = typename T::tag_type>
using dag_ptr   = refptr<const T, details::dag_item_traits<Tag>>;

};
