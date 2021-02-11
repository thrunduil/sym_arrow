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

#pragma once

#include "sym_arrow/sym_arrow.h"

namespace sym_arrow { namespace testing
{

class error_value
{
    private:
        value           m_value;
        double          m_error;

    public:
        explicit error_value(const value& v);
        error_value(const value& v, const double& err);

        bool            is_precise(double ulp_limit) const;
        bool            is_equal(const value& val, double ulp_limit,
                            double& ulp_error) const;
        bool            is_finite() const;
        bool            is_value_finite() const;

        const value&    get_value() const;
        double          get_error() const;
        value           get_abs_error() const;
};

void        disp(std::ostream& os, const error_value& v, bool add_nl = true);
void        disp(const error_value& v, bool add_nl = true);
error_value operator-(const error_value& v1);
error_value exp(const error_value& v1);
error_value log(const error_value& v1);
error_value abs(const error_value& v1);

error_value operator+(const error_value& v1, const error_value& v2);
error_value operator-(const error_value& v1, const error_value& v2);
error_value operator-(const error_value& v1, const value& v2);
error_value operator-(const value& v1, const error_value& v2);
error_value operator*(const error_value& v1, const error_value& v2);
error_value operator*(const value& v1, const error_value& v2);
error_value operator*(const error_value& v1, const value& v2);
error_value operator/(const error_value& v1, const error_value& v2);
error_value power_int(const error_value& v1, int v2);
error_value power_real(const error_value& v1, const error_value& v2);

};};
