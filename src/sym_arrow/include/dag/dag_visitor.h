/* 
 *  This file is a part of Arrow library.
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

#include "dag/dag_item.h"

namespace sym_dag
{

template<class Tag, class Derived>
class dag_visitor
{
    public:
        template<class ... Args>
        auto visit(const sym_dag::dag_item_base<Tag>* ast, Args&& ... args)
            -> decltype(std::declval<Derived>().eval(ast, std::forward<Args>(args)...)); 
};

template<class Tag, class Derived, class Ret>
class dag_visitor2_impl : public dag_visitor<Tag, dag_visitor2_impl<Tag, Derived, Ret>>
{
    private:
        Derived*    derived;

    public:
        dag_visitor2_impl(Derived* d)
            : derived(d)
        {};

        template<class Node_1, class Node_2, class ... Args>
        Ret eval(const Node_1* arg1, const Node_2* arg2, Args&& ... args)
        {
            return derived->eval(arg1, arg2, std::forward<Args>(args)...);
        };

        template<class Node_2, class ... Args>
        Ret eval(const Node_2* arg2, const sym_dag::dag_item_base<Tag>* arg1, Args&& ... args)
        {
            //extract first type
            return visit(arg1, arg2, std::forward<Args>(args)...);
        };

    public:
        template<class ... Args>
        Ret make(const sym_dag::dag_item_base<Tag>* arg1, 
                   const sym_dag::dag_item_base<Tag>* arg2, Args&& ... args)
        {
            //extract second type
            return visit(arg2, arg1, std::forward<Args>(args)...);
        };
};

template<class Tag, class Derived>
class dag_visitor2
{
    public:
        template<class ... Args>
        auto visit(const sym_dag::dag_item_base<Tag>* arg1, 
                   const sym_dag::dag_item_base<Tag>* arg2, Args&& ... args)
            -> decltype(std::declval<Derived>().eval(arg1, arg2, std::forward<Args>(args)...))
        {
            using ret_type  = decltype(std::declval<Derived>().eval(arg1, arg2, 
                                std::forward<Args>(args)...));
            using impl_type = dag_visitor2_impl<Tag, Derived, ret_type>;

            Derived* d      = static_cast<Derived*>(this);
            return impl_type(d).make(arg1, arg2, std::forward<Args>(args)...);
        };
};

}
