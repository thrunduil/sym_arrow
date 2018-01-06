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

#include "dag/refptr.h"
#include "dag/dag_ptr.h"
#include "dag/dag_item.h"
#include "dag/dag_visitor.h"

namespace sym_dag { namespace details
{

//--------------------------------------------------------------------
//                  dag_item_traits
//--------------------------------------------------------------------
template<class Tag>
SYM_DAG_FORCE_INLINE
void dag_item_traits<Tag>::check_free(const value_type* p)
{
    if (p && p->decrease_refcount())
        call_free(p);
};

template<class Tag>
SYM_DAG_FORCE_INLINE
void dag_item_traits<Tag>::check_destroy(const value_type* p, 
                                    stack_type& stack)
{
    if (p && p->decrease_refcount())
        call_destroy(p, stack);
};

template<class Tag>
SYM_DAG_FORCE_INLINE void dag_item_traits<Tag>::copy(const value_type* p)
{
    if (p) 
        p->increase_refcount();
};

template<class Tag>
SYM_DAG_FORCE_INLINE void 
dag_item_traits<Tag>::assign(const value_type* to, 
                             const value_type* from)
{
    copy(from);
    check_free(to);
};

template<class Tag>
class do_unregister_item_vis : public dag_visitor<Tag, do_unregister_item_vis<Tag>>
{
    private:
        using stack_type = sym_dag::details::release_stack<dag_item_base<Tag>>;

    public:
        template<class Node>
        SYM_DAG_FORCE_INLINE
        void eval(const Node* h, stack_type& stack)
        {
            dag_context<Tag>& c = dag_context<Tag>::get();
            c.unregister<Node>(const_cast<Node*>(h), stack);
        };

        template<class Node>
        SYM_DAG_FORCE_INLINE
        void eval(const Node* h)
        {
            dag_context<Tag>& c = dag_context<Tag>::get();
            c.unregister_without_stack<Node>(const_cast<Node*>(h));
        };
};

template<class Tag, bool need_stack>
struct call_free_impl
{
    using dag_traits    = dag_item_traits<Tag>;
    using stack_type    = typename dag_traits::stack_type;
    using value_type    = dag_item_base<Tag>;

    using context_type  = dag_context<Tag>;
    using stack_handle  = typename context_type::stack_handle;

    SYM_DAG_FORCE_INLINE
    static void eval(const value_type* ptr)
    {
        context_type& c     = context_type::get();
        stack_handle sh     = c.get_stack();
        stack_type& st      = sh.get();

        do_unregister_item_vis<Tag>().visit(ptr, st);
    };
};

template<class Tag>
struct call_free_impl<Tag, false>
{
    using value_type    = dag_item_base<Tag>;

    SYM_DAG_FORCE_INLINE
    static void eval(const value_type* ptr)
    {
        do_unregister_item_vis<Tag>().visit(ptr);
    };
};

template<class Tag>
SYM_DAG_FORCE_INLINE
void dag_item_traits<Tag>::call_free(const value_type* ptr)
{
    static const bool need_stack    = dag_tag_traits<Tag>::need_release_stack;
    return call_free_impl<Tag, need_stack>::eval(ptr);
};

template<class Tag>
SYM_DAG_FORCE_INLINE
void dag_item_traits<Tag>::call_destroy(const value_type* ptr, 
                                        stack_type& stack)
{
    do_unregister_item_vis<Tag>().visit(ptr, stack);
};

//--------------------------------------------------------------------
//                  weak_node_traits
//--------------------------------------------------------------------
template<class Tag>
SYM_DAG_FORCE_INLINE
void weak_node_traits<Tag>::check_free(const value_type* p)
{
    if (p && p->decrease_refcount())
        call_free(p);
}

template<class Tag>
SYM_DAG_FORCE_INLINE void weak_node_traits<Tag>::copy(const value_type* p)
{
    if (p) 
        p->increase_refcount();
}

template<class Tag>
SYM_DAG_FORCE_INLINE
void weak_node_traits<Tag>::assign(const value_type* to, const value_type* from)
{
    copy(from);
    check_free(to);
}

template<class Tag>
SYM_DAG_FORCE_INLINE
void weak_node_traits<Tag>::call_free(const value_type* ptr)
{
    using context_type  = dag_context<Tag>;
    using stack_handle  = typename context_type::stack_handle;

    context_type::get().unregister_weak(const_cast<value_type*>(ptr));
};

};};
