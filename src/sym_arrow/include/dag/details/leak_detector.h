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

#include "dag/config.h"
#include <functional>
#include <vector>
#include <memory>

namespace sym_dag { namespace details
{

class SYM_DAG_EXPORT leak_detector
{
    public:
        static void report_malloc(void* ptr);
        static void report_free(void* ptr);

        static void break_at_codes(const std::vector<size_t>& codes,
                        const std::function<void ()>& handler);

        static void report_leaks(std::ostream& os);
};

};};