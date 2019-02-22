/*
*  Morfa programming language.
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

#include "sym_arrow/ast/ast.h"

namespace sym_arrow { namespace ast
{

// default traversal for cannonized nodes
template<class Derived>
class traversal_visitor : public sym_dag::dag_visitor<sym_arrow::ast::term_tag, Derived>
{
    public:
        using tag_type  = sym_arrow::ast::term_tag;

    public:
        template<class ... Args>
        void eval(const ast::scalar_rep * h, Args&& ... args);

        template<class ... Args>
        void eval(const ast::symbol_rep* h, Args&& ... args);

        template<class ... Args>
        void eval(const ast::indexed_symbol_rep* h, Args&& ... args);

        template<class ... Args>
        void eval(const ast::add_build* h, Args&& ... args);

        template<class ... Args>
        void eval(const ast::mult_build* h, Args&& ... args);

        template<class ... Args>
        void eval(const ast::add_rep* h, Args&& ... args);

        template<class ... Args>
        void eval(const ast::mult_rep* h, Args&& ... args);

        // function name is not visited
        template<class ... Args>
        void eval(const ast::function_rep* h, Args&& ... args);
};

//-------------------------------------------------------------------
//                 traversal_visitor implementation
//-------------------------------------------------------------------
template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const ast::scalar_rep*, Args&& ...)
{
    return;
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const ast::symbol_rep*, Args&& ...)
{
    return;
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const ast::indexed_symbol_rep*, Args&& ...)
{
    TODO
    return;
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const ast::add_build*, Args&& ...)
{
    assertion(0,"we should not be here");
    throw;
}

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const ast::mult_build*, Args&& ...)
{
    assertion(0,"we should not be here");
    throw;
}

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const ast::add_rep* h, Args&& ... args)
{
    size_t n = h->size();

    for (size_t j = 0; j < n; ++j)
        visit(h->E(j), std::forward<Args>(args)...);

    if (h->has_log() == true)
        visit(h->Log(), std::forward<Args>(args)...);
}

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const ast::mult_rep* h, Args&& ... args)
{
    size_t in   = h->isize();
    size_t rn   = h->rsize();

    for (size_t i = 0; i < in; ++i)
        visit(h->IE(i), std::forward<Args>(args)...);    

    for (size_t i = 0; i < rn; ++i)
        visit(h->RE(i), std::forward<Args>(args)...);

    if (h->has_exp())
        visit(h->Exp(), std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const ast::function_rep* h, Args&& ... args)
{
    size_t n = h->size();

    for (size_t j = 0; j < n; ++j)
        visit(h->arg(j), std::forward<Args>(args)...);
};

};};
