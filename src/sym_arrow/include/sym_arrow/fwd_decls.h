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
#include "dag/dag.h"

namespace sym_arrow
{

class value;
class scalar;
class symbol;
class identifier;
class expr;
class add_expr;
class mult_expr;
class function_expr;

class set;

class data_provider;
class subs_context;
class diff_context;

};

namespace sym_arrow { namespace details
{

class subs_context_impl;
class diff_context_impl;
class function_evaler_impl;
class sym_table_impl;

class value_mp;

}};

namespace sym_arrow { namespace ast 
{

// dag item tag for terms
struct term_tag{};

// dag item tag for values
struct value_tag{};

// dag item tag for unique nodes
struct unique_nodes_tag{};

// other ast nodes
class set_rep;
class identifier_rep;

// terms representations
template<class Derived>
class expr_symbols;

class add_build;
class mult_build;
class add_rep;
class mult_rep;
class function_rep;
class scalar_rep;
class symbol_rep;

class item_collector_add;
class item_collector_mult;
class item_collector_size;

template<class Value_t, class Derived>
class vlist_mult;

class vlist_stack;
class vlist_add;
class ilist_mult;
class rlist_mult;
class cannonize;

template<class Value_type>
class build_item;

using expr_base             = sym_dag::dag_item_base<term_tag>;

using expr_handle           = const expr_base*;
using scalar_handle         = const scalar_rep*;
using symbol_handle         = const symbol_rep*;
using identifier_handle     = const identifier_rep*;

using expr_ptr              = sym_dag::dag_ptr<expr_base, term_tag>;
using scalar_ptr            = sym_dag::dag_ptr<scalar_rep, term_tag>;
using symbol_ptr            = sym_dag::dag_ptr<symbol_rep, term_tag>;

using identifier_ptr        = sym_dag::dag_ptr<identifier_rep, unique_nodes_tag>;
using set_ptr               = sym_dag::dag_ptr<set_rep, unique_nodes_tag>;

using weak_expr_ptr         = sym_dag::weak_dag_item<term_tag>;

}}

namespace sym_arrow { namespace ast { namespace details
{

class term_context_data;
class value_context_data;
class symbol_context_data;

}}};