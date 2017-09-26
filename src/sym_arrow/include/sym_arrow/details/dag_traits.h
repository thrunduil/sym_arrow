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

#include "sym_arrow/config.h"
#include "sym_arrow/fwd_decls.h"
#include "dag/dag.h"

namespace sym_arrow { namespace ast
{

//---------------------------------------------------------------------
//      this file contains configuration of dag nodes required
//      by sym_dag library
//---------------------------------------------------------------------

enum class term_types : size_t
{
    scalar  = 0,
    symbol,
    add_build,
    mult_build,
    add_rep,
    mult_rep,
    function_rep,
    number_codes
};

}};

namespace sym_dag
{

//-------------------------------------------------------------
//              term_tag
//-------------------------------------------------------------
// configure dag_item for term_tag
template<>
struct dag_tag_traits<sym_arrow::ast::term_tag>
{
    static const size_t number_codes    = (size_t)sym_arrow::ast::term_types::number_codes;
    static const size_t user_flag_bits  = 3;
};

};
