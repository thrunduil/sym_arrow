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

#include "test_set.h"

#include "sym_arrow/sym_arrow.h"

using namespace sym_arrow;

// Laguerre Polynomial L_n(x) == exp(x) * (d/dx)^n [x^n * exp(-x)]
expr laguerre_poly(int n, const symbol & x)
{
    expr ker    = exp(-x);
    expr poly   = power_int(x, n) * ker;
    return diff(poly, x, n) / ker;
}

// associated Laguerre Polynomial L_{n,m}(x) == (d/dx)^m [L_n(x)]
expr assoc_laguerre_poly(int n, int m, const symbol & x)
{
    expr poly   = laguerre_poly(n, x);
    poly        = simplify(poly);
    return diff(poly, x, m);
}

void example()
{    
    tic();

    expr poly   = assoc_laguerre_poly(100, 100, symbol("x"));

    tocdisp();
};

namespace sym_arrow { namespace testing
{

void test_set::example()
{
    ::example();    
}

}};
