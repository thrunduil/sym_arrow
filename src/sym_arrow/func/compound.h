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
#include "sym_arrow/nodes/expr.h"

namespace sym_arrow { namespace details
{

class compound
{
    public:
                            //a + b*s
        static void         plus_build(expr& ret, const value& a,  const value& b,  
                                ast::expr_handle s);  

                            //a + b1*s1 + b2*s2
        static void         plus_build(expr& ret, const value& a,  const value& b1, 
                                ast::expr_handle s1 , const value& b2, ast::expr_handle s2);

        static void         exp_build(expr& ret, ast::expr_handle x1);
        static void         log_build(expr& ret, ast::expr_handle x1);

                            //a * x1^b1
        static void         mult_build(expr& ret, const value& a,  ast::expr_handle x1, int b1);
                            //x1^b1 * x2^b2
        static void         mult_build(expr& ret, ast::expr_handle x1, int b1, ast::expr_handle x2, int b2);
                            //x^b
        static void         mult_build(expr& ret, ast::expr_handle x, const expr& b);
        static void         mult_build(expr& ret, const value& x, ast::expr_handle b);
        static void         mult_build(expr& ret, ast::expr_handle x, const value& b);

    private:
                            // x ^ b
        static void         mult_build_pow(expr& ret, const value& x, ast::expr_handle b);

        static ast::add_build*  get_add_build_temp(ast::expr_handle ex);
        static ast::mult_build* get_mult_build_temp(ast::expr_handle ex);
};

};};
