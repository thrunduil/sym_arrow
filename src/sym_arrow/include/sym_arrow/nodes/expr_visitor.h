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

namespace sym_arrow
{

// expression visitor base class;
// in order to implement some expression visitor one must define a class
// that derive from this class (Derived argument is this implementation
// type); in derived class one must define functions:
//
//     [some_return_type] eval(const expr_type&, [additional_arguments])
//
// where some_return_type is any type, and additional_arguments is zero
// or many additional arguments; these functions must be defined for every
// expression type (i.e. scalar, symbol, add_expr, etc.); 
// eval function must also be declared for expr argument (but need not be defined)
// eval functions can be template functions
template<class Derived>
struct expr_visitor
{
    // find runtime type of an expression and call function eval 
    // defined in derived class for given runtime type; return type
    // of this function is the same as return type of eval function
    // defined in derived class that accepts expression of given type
    template<class ... Args>
    auto visit(const expr& ex, Args&& ... args)
        -> decltype(std::declval<Derived>().eval(ex, std::forward<Args>(args)...)); 

    // visit any type
    template<class Type, class ... Args>
    auto visit(const Type& ex, Args&& ... args)
        -> decltype(std::declval<Derived>().eval(ex, std::forward<Args>(args)...)); 
};

// traversal visitor implementing default depth-first traversal;
// derived class may reimplement some of functions
template<class Derived>
struct expr_traversal_visitor : public expr_visitor<Derived>
{
    // declaration only; determine return type of visitor (which is void)
    template<class ... Args>
    void eval(const expr& ex, Args&& ... args);

    // visit value
    template<class ... Args>
    void eval(const value& v, Args&& ... args);

    // visit int value
    template<class ... Args>
    void eval(int v, Args&& ... args);

    // visit scalar
    template<class ... Args>
    void eval(const sym_arrow::scalar& ex, Args&& ... args);

    // visit symbol
    template<class ... Args>
    void eval(const sym_arrow::symbol& ex, Args&& ... args);

    // visit add expression
    template<class ... Args>
    void eval(const sym_arrow::add_expr& ex, Args&& ... args);

    // visit mult expression
    template<class ... Args>
    void eval(const sym_arrow::mult_expr& ex, Args&& ... args);

    // visit function; function name is not visited
    template<class ... Args>
    void eval(const sym_arrow::function_expr& ex, Args&& ... args);
};

// example of traversal visitor, that counts total number of symbols
// in expression
// struct num_syms : expr_traversal_visitor<num_syms>
// {
//     using expr_traversal_visitor<num_syms>::eval;
// 
//     void eval(const symbol&, size_t& n_syms)
//     { 
//         ++n_syms;
//     };
// };
//
// use:
//     expr ex = parse("x + y^x");
//     size_t nsym = 0;
//     num_syms().visit(ex, nsym);
//     std::cout << nsym << "\n";
//
// output:
//     3
}

#include "sym_arrow/details/expr_visitor.inl"
