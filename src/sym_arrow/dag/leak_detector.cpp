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

#include "dag/details/leak_detector.h"
#include "dag/details/global_objects.h"

#include <map>
#include <set>
#include <ostream>

namespace sym_dag { namespace details
{

struct leak_detector_impl
{
    private:
        using ptr_map   = std::map<void*, size_t>;
        using code_set  = std::set<size_t>;
        using hander    = std::function<void ()>;

    private:
        ptr_map     m_ptr_map;
        size_t      m_code;
        code_set    m_stop_codes;
        hander      m_handler;

    public:
        leak_detector_impl();

        void        report_malloc(void* ptr);
        void        report_free(void* ptr);

        void        report_leaks(std::ostream& os);
        void        break_at_codes(const std::vector<size_t>& codes,
                        const std::function<void ()>& handler);

};

leak_detector_impl::leak_detector_impl()
    :m_code(0)
{};

void leak_detector_impl::report_malloc(void* ptr)
{
    if (m_stop_codes.find(m_code) != m_stop_codes.end())
        m_handler();

    m_ptr_map[ptr]  = m_code;
    ++m_code;
};

void leak_detector_impl::report_free(void* ptr)
{
    auto pos = m_ptr_map.find(ptr);

    if (pos == m_ptr_map.end())
        throw std::runtime_error("invalid free");

    m_ptr_map.erase(pos);
};

void leak_detector_impl::break_at_codes(const std::vector<size_t>& codes,
                const std::function<void ()>& handler)
{
    m_handler = handler;
    m_stop_codes.insert(codes.begin(), codes.end());
}

void leak_detector_impl::report_leaks(std::ostream& os)
{
    if (m_ptr_map.size() == 0)
    {
        os << "NO LEAKS DETECTED" << "\n";
        return;
    }

    os << "LEAKS DETECTED !!" << "; number leaks: " << m_ptr_map.size() << "\n";

    os << "    ";

    size_t count = 0;
    for (const auto& elem : m_ptr_map)
    {
        os << elem.second << ", ";
        ++count;

        if (count > 20)
            break;
    }

    os << "\n";
};

leak_detector_impl* g_leak_detector
    = global_objects::make_last<leak_detector_impl>();

void leak_detector::report_malloc(void* ptr)
{
    g_leak_detector->report_malloc(ptr);
};

void leak_detector::report_free(void* ptr)
{
    g_leak_detector->report_free(ptr);
};

void leak_detector::report_leaks(std::ostream& os)
{
    g_leak_detector->report_leaks(os);
};

void leak_detector::break_at_codes(const std::vector<size_t>& codes,
                const std::function<void ()>& handler)
{
    g_leak_detector->break_at_codes(codes, handler);
};

};};