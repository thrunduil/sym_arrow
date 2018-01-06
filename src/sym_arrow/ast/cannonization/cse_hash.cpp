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

#include "cse_hash.h"
#include "sym_arrow/exception.h"
#include "sym_arrow/ast/ast.h"

#include <fstream>
#include <sstream>

namespace sym_arrow { namespace ast
{

#define SYM_ARROW_LOG_BRANCH_PRED 0

#if SYM_ARROW_LOG_BRANCH_PRED
    std::vector<std::ofstream*> log_pred_logs;
    std::ofstream* log_pred_stats = new std::ofstream("log_pred_stats.txt.");

    static std::ofstream& get_log(size_t level)
    {
        for (size_t i = log_pred_logs.size(); i <= level; ++i)
        {
            std::ostringstream name;
            name << "log_pred_" << i << ".txt";

            log_pred_logs.push_back(new std::ofstream(name.str()));
        }

        return *log_pred_logs[level];
    };
#endif

cse_hash::cse_hash()
    :m_nest_level(0)
{
    for (int i = 0; i < num_predictors; ++i)
        m_predictors[i].set_tag(i);

    using track_func = expr_base::track_function;

    size_t code     = (size_t)ast::track_function_code::cse;
    track_func f    = [=](expr_handle h, stack_type& st) 
                        { this->unregister(h, st); };

    expr_base::add_tracking_function(code, f);

    sym_dag::registered_dag_context::get().register_cache(this);
};

cse_hash::~cse_hash()
{
    m_cache.close();
    m_hash_map.close();

    #if SYM_ARROW_LOG_BRANCH_PRED
        for (size_t i = 0; i < log_pred_logs.size(); ++i)
            log_pred_logs[i]->flush();

        for (size_t i = 0; i < num_predictors; ++i)
            m_predictors[i].print_stats(*log_pred_stats);

        log_pred_stats->flush();
    #endif
};

void cse_hash::clear()
{
    m_cache.clear();
    m_hash_map.clear();
};

void cse_hash::unregister(expr_handle h, stack_type& st)
{
    auto pos = m_hash_map.find(h);

    if (pos.empty() == true)
        return;

    m_hash_map.remove(pos, st);
};

cse_hash* g_cse_hash
    = sym_dag::global_objects::make_before<cse_hash>();

cse_hash& cse_hash::get()
{
    return *g_cse_hash;
}

SYM_ARROW_FORCE_INLINE
branch_predictor& cse_hash::get_predictor(int level)
{
    // merge branch predictions for highly nested calls;
    // this will confuse the predictor, prediction will be
    // weak, but deep calls should be very rare
    return m_predictors[std::min(level, num_predictors - 1)];
}

bool cse_hash::check_hash()
{
    bool pred   = get_predictor(m_nest_level).get_prediction();
    ++m_nest_level;

    return pred;
}

void cse_hash::add_observation(bool obs, bool pred)
{
    --m_nest_level;    
    assertion(m_nest_level >= 0, "error in cse_hash");

    #if SYM_ARROW_LOG_BRANCH_PRED
        size_t lev  = std::min(m_nest_level, num_predictors - 1);
        get_log(lev)    << (obs == false ? 1 : 2) << " " 
                        << (pred == false ? 1 : 2) << "\n";
    #endif

    return get_predictor(m_nest_level).add_observation(obs, pred);
}

bool cse_hash::get_hashed_subexpr_elim(const expr& ex, expr& simpl)
{
    expr_handle h   = ex.get_ptr().get();
    bool is_tracked = h->is_tracked();

    if (is_tracked == false)
        return false;

    auto ed = m_hash_map.find(h);

    if (ed.empty() == true)
        return false;

    const cse_hash_data& elim_data = ed->get_value();

    if (elim_data.is_empty() == true)
        return false;

    simpl   = elim_data.get_simplified_expr();

    m_cache.add(ex.get_ptr());
    m_cache.add(simpl.get_ptr());

    return true;
}

void cse_hash::set_hashed_subexpr_elim(const expr& ex, const expr& simpl)
{
    assertion(ex.is_null() == false, "error in set_hashed_subexpr_elim");

    expr_handle h   = ex.get_ptr().get();
    m_hash_map.insert(h, cse_hash_data(simpl));
    
    h->set_tracked(true);

    m_cache.add(ex.get_ptr());
    m_cache.add(simpl.get_ptr());
};

};};
