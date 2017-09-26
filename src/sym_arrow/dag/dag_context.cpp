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

#include "dag/details/dag_context.inl"
#include "dag/details/leak_detector.h"

namespace sym_dag 
{

registered_dag_context* g_registered_dags 
    = global_objects::make_context<registered_dag_context>();

registered_dag_context::registered_dag_context()
{};

registered_dag_context::~registered_dag_context()
{};

registered_dag_context& registered_dag_context::get()
{
    return *g_registered_dags;
};

void registered_dag_context::register_dag(details::dag_context_base* dag)
{
    m_dags.push_back(dag);
};

void registered_dag_context::register_cache(node_cache* c)
{
    m_caches.push_back(c);
};

void registered_dag_context::clear_cache()
{
    for (node_cache* c : m_caches)
        c->clear();
};

void registered_dag_context::destroy()
{
    m_caches.clear();
    close();
};

void registered_dag_context::close()
{
    clear_cache();

    for (details::dag_context_base* dag : m_dags)
        dag->close_context_data();

    for (details::dag_context_base* dag : m_dags)
        dag->close();
};

void registered_dag_context::print_reuse_stats(std::ostream& os)
{
    os << "REUSE STATS:" << "\n";

    for (details::dag_context_base* dag : m_dags)
        dag->print_reuse_stats(os);
};

void registered_dag_context::print_memory_stats(std::ostream& os)
{
    os << "MEMORY STATS:" << "\n";

    details::memory_stats stats;

    for (details::dag_context_base* dag : m_dags)
        dag->print_memory_stats(os, stats);

    details::memory_debugger::disp_memory_stats(os, stats);

    os << "\n";
    os << std::string(4, ' ') << std::string(40, '-')  << "\n";

    os  << std::string(4, ' ') << "total allocated by hash tables: " 
            << (double)stats.m_bytes_hash / 1.0e6 << "MB" << "\n";
    os  << std::string(4, ' ') << "total allocated by mem managers: "
        << (double)stats.m_bytes_mem / 1.0e6 << "MB" << "\n";
    os  << std::string(4, ' ') << "total in use: "
        << (double)stats.m_bytes_reserved / 1.0e6 << "MB" << "\n";
};

void registered_dag_context::print_memory_leaks(std::ostream& os)
{
    #if SYM_DAG_DEBUG_MEMORY
        details::leak_detector::report_leaks(os);
    #else
        os << "enable SYM_DAG_DEBUG_MEMORY first" << "\n";
    #endif
};

void registered_dag_context::print_collisions(std::ostream& os)
{
    os << "COLLISION STATS:" << "\n";

    for (details::dag_context_base* dag : m_dags)
        dag->print_collisions(os);
};

};
