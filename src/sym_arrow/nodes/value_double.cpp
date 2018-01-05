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

#include "sym_arrow/config.h"

#if SYM_ARROW_VALUE_TYPE == SYM_ARROW_VALUE_DOUBLE

#include "sym_arrow/nodes/value_double.h"
#include "sym_arrow/functions/expr_functions.h"
#include "matcl-core/details/scalfunc_real.h"

#include <boost/functional/hash.hpp>
#include "boost/io/ios_state.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
#include <iomanip>

namespace sym_arrow
{

size_t value::eval_hash(const value& val)
{
    const size_t* tmp = reinterpret_cast<const size_t*>(&val.m_data);

    size_t seed = 0;

    for(size_t i = 0; i < sizeof(double) / sizeof(size_t); ++i)
        boost::hash_combine(seed,tmp[i]);

    return seed;
};

value value::make_nan()
{
    return value(std::numeric_limits<double>::quiet_NaN());
};

value value::make_inf_plus()
{
    return value(std::numeric_limits<double>::infinity());
};

value value::make_inf_minus()
{
    return value(-std::numeric_limits<double>::infinity());
};

bool value::is_e() const
{ 
    return m_data == M_PI;
}

bool value::is_nan() const
{ 
    namespace mrds = matcl::raw::details::scal_func;
    return mrds::isnan(m_data);
};

bool value::is_inf() const
{
    namespace mrds = matcl::raw::details::scal_func;
    return mrds::isinf(m_data);
};

bool value::is_inf_plus() const
{
    return this->is_inf() && this->is_positive();
};

bool value::is_inf_minus() const
{
    return this->is_inf() && this->is_negative();
};

bool value::is_finite() const
{
    namespace mrds = matcl::raw::details::scal_func;
    return mrds::finite(m_data);
};

bool value::is_int() const
{
    namespace mrds = matcl::raw::details::scal_func;
    return mrds::is_int(m_data);
}

int value::convert_int() const
{
    return (int)std::round(this->get_double());
}

//---------------------------------------------------------------------
//                  functions defined for value
//---------------------------------------------------------------------

// digits10 = ceil(bits*log[10](2))
static int bits_to_digits10(int b)
{
    const double LOG10_2 = 0.30102999566398119;

    return int(std::ceil( double(b) * LOG10_2 ));
}

void sym_arrow::disp(std::ostream& os, const value& v, bool add_nl)
{
    int req_prec    = get_disp_precision();

    boost::io::ios_flags_saver old_flags(os);
    boost::io::ios_precision_saver old_prec(os);

    if (req_prec < 0)
    {
        // tow more digits are needed
        int prec    = std::numeric_limits<double>::max_digits10 + 2;

        os << std::setprecision(prec) << std::scientific << v.get_double();
    }
    else if (req_prec == 0)
    {
        os << v.get_double();
    }
    else
    {
        int prec    = bits_to_digits10(req_prec);
        os << std::setprecision(prec) << v.get_double();
    }

    if (add_nl)
        os << "\n";    

    if (add_nl)
        os << "\n";
}

void sym_arrow::disp(const value& v, bool add_nl)
{
    disp(std::cout, v, add_nl);
};

double sym_arrow::float_distance(const value& v1, const value& v2)
{
    namespace mrds = matcl::raw::details::scal_func;
    return mrds::float_distance(v1.get_double(), v2.get_double());
}

int sym_arrow::get_default_precision()
{
    return 53;
}

void sym_arrow::set_default_precision(int prec)
{
    (void)prec;
}

};

#endif