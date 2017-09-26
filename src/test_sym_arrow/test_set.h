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

#include "sym_arrow/sym_arrow.h"

namespace sym_arrow { namespace testing
{

struct test_set
{
    public:
        static void     test_visitor();
        static void     test_special_cases();
        static void     test_diff_context();

	    static void     test_random_subs(size_t n_rep, bool dodisp, 
                            std::ostream& log1, std::ostream& log2);
	    static void     test_random_diff(size_t n_rep);
        static void     test_diff();
        static void     test_expression(size_t n_rep);        

    private:
        static void     init_genrand(size_t seed);
        static void     tic();
        static double   toc();
};

}};