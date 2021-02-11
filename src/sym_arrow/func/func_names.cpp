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

#include "sym_arrow/func/func_names.h"

namespace sym_arrow { namespace details
{

identifier func_name::bool_eq()
{
    return identifier("bool_eq");
};

identifier func_name::bool_neq()
{
    return identifier("bool_neq");
};

identifier func_name::bool_lt()
{
    return identifier("bool_lt");
};

identifier func_name::bool_gt()
{
    return identifier("bool_gt");
};

identifier func_name::bool_leq()
{
    return identifier("bool_leq");
};

identifier func_name::bool_geq()
{
    return identifier("bool_geq");
};

identifier func_name::bool_or()
{
    return identifier("bool_or");
};

identifier func_name::bool_and()
{
    return identifier("bool_and");
};

identifier func_name::bool_xor()
{
    return identifier("bool_xor");
};

identifier func_name::bool_andnot()
{
    return identifier("bool_andnot");
};

identifier func_name::bool_not()
{
    return identifier("bool_not");
};

identifier func_name::if_then()
{
    return identifier("if_then");
}

identifier func_name::if_then_else()
{
    return identifier("if_then_else");
}

identifier func_name::delta()
{
    return identifier("delta");
}

}}