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

#include "dag/dag_visitor.h"
#include <boost/preprocessor/repetition/repeat.hpp>

namespace sym_dag { namespace details
{

template<class Tag, size_t Code>
struct cast_node
{
    using type  = typename dag_code_to_node<Tag, Code>::type;

    static const type* eval(const sym_dag::dag_item_base<Tag>* ast)
    {
        return static_cast<const type*>(ast);
    };
};

template<class Ret, class Tag, class Derived, size_t Code,
        bool Has_code = (Code < dag_tag_traits<Tag>::number_codes) >
struct case_eval
{
    using ast_type = sym_dag::dag_item_base<Tag>;

    template<class ... Args>
    static Ret make(Derived* d, const ast_type* ast, Args&& ... args)
    {
        return d->eval(details::cast_node<Tag, Code>::eval(ast), 
                       std::forward<Args>(args)...);
    };
};

template<class Ret, class Tag, class Derived, size_t Code>
struct case_eval<Ret, Tag, Derived, Code, false>
{
    using ast_type = sym_dag::dag_item_base<Tag>;

    template<class ... Args>
    static Ret make(Derived*, const ast_type*, Args&& ...)
    {
        throw std::runtime_error("invalid term code");
    };
};

}};

namespace sym_dag
{

#define SYM_DAG_VISITOR_CASE(code)                          \
case code:                                                  \
    return details::case_eval<ret_type, Tag, Derived, code> \
        ::make(static_cast<Derived*>(this), ast,            \
               std::forward<Args>(args)...);                \

#define SYM_DAG_VISITOR_CASE_M(i1, code, i2)                \
SYM_DAG_VISITOR_CASE(code)

template<class Tag, class Derived>
template<class ... Args>
auto dag_visitor<Tag, Derived>::visit(const sym_dag::dag_item_base<Tag>* ast, Args&& ... args)
    -> decltype(std::declval<Derived>().eval(ast, std::forward<Args>(args)...)) 
{
    using ret_type = decltype(std::declval<Derived>().eval(ast, std::forward<Args>(args)...));

    switch (ast->get_code())
    {
        BOOST_PP_REPEAT(SYM_DAG_MAX_NUMBER_CODES, SYM_DAG_VISITOR_CASE_M, 0)
    };

    throw std::runtime_error("invalid term code");
};

};
