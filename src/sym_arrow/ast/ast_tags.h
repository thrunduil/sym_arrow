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

#pragma once

#include "sym_arrow/config.h"
#include "sym_arrow/fwd_decls.h"
#include "dag/dag.h"
#include "sym_arrow/details/dag_traits.h"
#include "sym_arrow/ast/expr_symbols.h"

namespace sym_arrow { namespace ast
{

struct ast_flags
{
    // flag used to mark presence of log/exp terms
    static const size_t special     = 0;

    // flag used by add_rep to mark normalization
    static const size_t normalized  = 1;

    // temporary flag
    static const size_t work        = 2;
};

}};

namespace sym_dag
{

// this file contains configuration of dag nodes required
// by sym_dag library; see also sym_arrow/details/dag_traits.h

//-------------------------------------------------------------
//              term_tag
//-------------------------------------------------------------
// type of additional data stored in the dag_context for term_tag
template<>
struct dag_context_data<sym_arrow::ast::term_tag>
{    
    using context_data_type = sym_arrow::ast::details::term_context_data;
};

// define mapping from code to type for term_tag
template<>
struct dag_code_to_node<sym_arrow::ast::term_tag, (size_t)sym_arrow::ast::term_types::scalar>
{
    using type = sym_arrow::ast::scalar_rep;
};
template<>
struct dag_code_to_node<sym_arrow::ast::term_tag, (size_t)sym_arrow::ast::term_types::symbol>
{
    using type = sym_arrow::ast::symbol_rep;
};
template<>
struct dag_code_to_node<sym_arrow::ast::term_tag, (size_t)sym_arrow::ast::term_types::add_build>
{
    using type = sym_arrow::ast::add_build;
};
template<>
struct dag_code_to_node<sym_arrow::ast::term_tag, (size_t)sym_arrow::ast::term_types::mult_build>
{
    using type = sym_arrow::ast::mult_build;
};
template<>
struct dag_code_to_node<sym_arrow::ast::term_tag, (size_t)sym_arrow::ast::term_types::add_rep>
{
    using type = sym_arrow::ast::add_rep;
};
template<>
struct dag_code_to_node<sym_arrow::ast::term_tag, (size_t)sym_arrow::ast::term_types::mult_rep>
{
    using type = sym_arrow::ast::mult_rep;
};
template<>
struct dag_code_to_node<sym_arrow::ast::term_tag, (size_t)sym_arrow::ast::term_types::function_rep>
{
    using type = sym_arrow::ast::function_rep;
};

// define mapping from type to code for term_tag
template<>
struct dag_node_to_code<sym_arrow::ast::term_tag, sym_arrow::ast::scalar_rep>
{
    static const size_t code    = (size_t)sym_arrow::ast::term_types::scalar;
};

template<>
struct dag_node_to_code<sym_arrow::ast::term_tag, sym_arrow::ast::symbol_rep>
{
    static const size_t code    = (size_t)sym_arrow::ast::term_types::symbol;
};

template<>
struct dag_node_to_code<sym_arrow::ast::term_tag, sym_arrow::ast::add_build>
{
    static const size_t code    = (size_t)sym_arrow::ast::term_types::add_build;
};
template<>
struct dag_node_to_code<sym_arrow::ast::term_tag, sym_arrow::ast::mult_build>
{
    static const size_t code    = (size_t)sym_arrow::ast::term_types::mult_build;
};
template<>
struct dag_node_to_code<sym_arrow::ast::term_tag, sym_arrow::ast::add_rep>
{
    static const size_t code    = (size_t)sym_arrow::ast::term_types::add_rep;
};
template<>
struct dag_node_to_code<sym_arrow::ast::term_tag, sym_arrow::ast::mult_rep>
{
    static const size_t code    = (size_t)sym_arrow::ast::term_types::mult_rep;
};
template<>
struct dag_node_to_code<sym_arrow::ast::term_tag, sym_arrow::ast::function_rep>
{
    static const size_t code    = (size_t)sym_arrow::ast::term_types::function_rep;
};

//-------------------------------------------------------------
//              value_tag
//-------------------------------------------------------------
// type of additional data stored in the dag_context for value_tag
template<>
struct dag_context_data<sym_arrow::ast::value_tag>
{    
    using context_data_type = sym_arrow::ast::details::value_context_data;
};

// define mapping from code to type for value_tag
template<>
struct dag_code_to_node<sym_arrow::ast::value_tag, (size_t)sym_arrow::ast::value_types::mp_float>
{
    using type = sym_arrow::details::value_mp;
};

// define mapping from type to code for value_tag
template<>
struct dag_node_to_code<sym_arrow::ast::value_tag, sym_arrow::details::value_mp>
{
    static const size_t code    = (size_t)sym_arrow::ast::value_types::mp_float;
};

//-------------------------------------------------------------
//              unique_nodes_tag
//-------------------------------------------------------------
// type of additional data stored in the dag_context for unique_nodes_tag
template<>
struct dag_context_data<sym_arrow::ast::unique_nodes_tag>
{    
    using context_data_type = sym_arrow::ast::details::symbol_context_data;
};

// define mapping from code to type for unique_nodes_tag
template<>
struct dag_code_to_node<sym_arrow::ast::unique_nodes_tag, 
                        (size_t)sym_arrow::ast::unique_nodes_types::base_symbol>
{
    using type = sym_arrow::ast::identifier_rep;
};

template<>
struct dag_node_to_code<sym_arrow::ast::unique_nodes_tag, 
                        sym_arrow::ast::identifier_rep>
{
    static const size_t code    = (size_t)sym_arrow::ast::unique_nodes_types::base_symbol;
};

template<>
struct dag_code_to_node<sym_arrow::ast::unique_nodes_tag, 
                            (size_t)sym_arrow::ast::unique_nodes_types::set>
{
    using type = sym_arrow::ast::set_rep;
};

template<>
struct dag_node_to_code<sym_arrow::ast::unique_nodes_tag, 
                            sym_arrow::ast::set_rep>
{
    static const size_t code    = (size_t)sym_arrow::ast::unique_nodes_types::set;
};

};
