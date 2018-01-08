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

#include "sym_arrow/func/func_names.h"

namespace sym_arrow { namespace details
{

symbol func_name::bool_eq()
{
    return symbol("bool_eq");
};

symbol func_name::bool_neq()
{
    return symbol("bool_neq");
};

symbol func_name::bool_lt()
{
    return symbol("bool_lt");
};

symbol func_name::bool_gt()
{
    return symbol("bool_gt");
};

symbol func_name::bool_leq()
{
    return symbol("bool_leq");
};

symbol func_name::bool_geq()
{
    return symbol("bool_geq");
};

symbol func_name::bool_or()
{
    return symbol("bool_or");
};

symbol func_name::bool_and()
{
    return symbol("bool_and");
};

symbol func_name::bool_xor()
{
    return symbol("bool_xor");
};

symbol func_name::bool_andnot()
{
    return symbol("bool_andnot");
};

symbol func_name::bool_not()
{
    return symbol("bool_not");
};

symbol func_name::if_then()
{
    return symbol("if_then");
}

symbol func_name::if_then_else()
{
    return symbol("if_then_else");
}

}}