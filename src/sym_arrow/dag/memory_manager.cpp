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

#include "dag/details/memory_manager.h"
#include "dag/details/global_objects.h"
#include "dag/dag_context.h"
#include "dag/details/dag_context_details.h"

#include <iostream>
#include <map>

namespace sym_dag { namespace details
{

void details::report_bad_alloc()
{
    static bool cache_clearing = false;

    // we need some additional memory for stack unwinding
    if (cache_clearing == false)
    {
        cache_clearing = true;
        registered_dag_context::get().clear_cache();
        cache_clearing = false;

        throw std::bad_alloc();
    }    
    else
    {
        throw std::bad_alloc();
    }
};

//--------------------------------------------------------------------
//                  memory_debugger
//--------------------------------------------------------------------
class mem_use_info
{
    private:
        double      m_mallocs;
        double      m_max_usage;
        size_t      m_current_usage;
        size_t      n_elems;

    public:
        mem_use_info();

        void        report_malloc(size_t bytes);
        void        report_free(size_t bytes);
        void        disp_memory_stats(std::ostream& os, const char* type_name,
                        memory_stats& stats);
};

mem_use_info::mem_use_info()
    :m_mallocs(0.), m_max_usage(0.), m_current_usage(0), n_elems(0)
{};

void mem_use_info::report_malloc(size_t bytes)
{
    double m        = static_cast<double>(bytes);
    m_mallocs       += m;
    m_current_usage += bytes;
    m_max_usage     = std::max(m_max_usage, (double)m_current_usage);
    ++n_elems;
};

void mem_use_info::report_free(size_t bytes)
{
    m_current_usage -= bytes;
    --n_elems;
};

void mem_use_info::disp_memory_stats(std::ostream& os, const char* type_name,
                                     memory_stats& stats)
{
    os  << "tag: " << type_name << "\n"
        << "    " << "mallocs: "    << m_mallocs/1e6 << " MB" << "\n"
        << "    " << "max: "        << m_max_usage/1e6 << " MB" << "\n"
        << "    " << "current: "    << m_current_usage/1e6 << " MB" << "\n"
        << "    " << "elements: "   << n_elems << "\n"
        ;

    stats.m_bytes_reserved += m_current_usage;
};

using stat_type = std::map<const char*, mem_use_info>;

static stat_type* global_stats = global_objects::make_last<stat_type>();

static stat_type& get_stats()
{
    return *global_stats;
};

void memory_debugger::report_malloc(const char* type_name, size_t size)
{
    get_stats()[type_name].report_malloc(size);
};

void memory_debugger::report_free(const char* type_name, size_t size)
{
    get_stats()[type_name].report_free(size);
};

void memory_debugger::disp_memory_stats(std::ostream& os, details::memory_stats& stats)
{
    for (auto pos = get_stats().begin(); pos != get_stats().end(); ++pos)
        pos->second.disp_memory_stats(os, pos->first, stats);
};

//--------------------------------------------------------------------
//                  mem_manager
//--------------------------------------------------------------------
mem_manager::mem_manager()
{
    m_pools = (pool*)allocator_type::malloc( (DAG_MALLOC_MAX_SIZE + 1) * sizeof(pool));

    for(size_t i = 1; i <= DAG_MALLOC_MAX_SIZE; ++i)
        new(m_pools+i) pool(i * sizeof(size_t));
};

mem_manager::~mem_manager()
{
    if (m_pools)
    {        
        for(size_t i = 1; i <= DAG_MALLOC_MAX_SIZE; ++i)
            m_pools[i].~pool();

        allocator_type::free(m_pools);
        m_pools = nullptr;
    };
};

void mem_manager::purge_memory()
{
    if (m_pools)
    {        
        for(size_t i = 1; i <= DAG_MALLOC_MAX_SIZE; ++i)
            m_pools[i].purge_memory();
    };
};

};};