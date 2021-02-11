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

#include "branch_predictor.h"
#include <math.h>

#include <ostream>

namespace sym_arrow { namespace ast
{

branch_predictor::branch_predictor()
    :branch_predictor(0)
{};

branch_predictor::branch_predictor(int tag)
    :m_tag(tag)
{
    initialize_priors();
    initialize_state();
}

void branch_predictor::set_tag(int tag)
{
    m_tag = tag;
}

void branch_predictor::initialize_priors()
{
    static const size_t size    = (2 + data_size) * details::pow2(n_lags);

    for (size_t i = 0; i < size; i += 1)
        m_prior_table[i]    = 0.5;
};

void branch_predictor::initialize_state()
{
    for (int i = 0; i < n_lags; ++i)
        m_lags[i]       = false;

    m_pred_error        = 0.0;
    m_predictions       = 0.0;
};

void branch_predictor::print_stats(std::ostream& os)
{
    os << "predictor " << m_tag << ": "
        << "error: " << m_pred_error / m_predictions
        << ", predictions: " << m_predictions
        << "\n";
}

SYM_ARROW_FORCE_INLINE
void branch_predictor::update_lags(bool* lags, bool obs)
{
    for (int i = n_lags - 1; i > 0; --i)
        lags[i]         = lags[i - 1];

    lags[0]             = obs;
};

SYM_ARROW_FORCE_INLINE
size_t branch_predictor::get_prior_offset(const bool* lags)
{
    size_t pos          = 0;

    for (size_t i = 0; i < n_lags; ++i)
        pos             += size_t(lags[i]) * details::pow2(i);

    return pos * (2 + data_size);
};

SYM_ARROW_FORCE_INLINE
double branch_predictor::get_prior_mean(bool obs, const bool* lags, 
                                       const double* prior_table_root)
{
    const double* prior_table;

    size_t off          = get_prior_offset(lags);    
    prior_table         = prior_table_root + off;

    double sum          = prior_table[0] + prior_table[1];

    if (obs == false)
        return prior_table[0] / sum;
    else
        return prior_table[1] / sum;
}

SYM_ARROW_FORCE_INLINE
void branch_predictor::update_dirichler_prior(bool obs, const bool* lags, 
                double* prior_table_root)
{
    double* table;

    size_t off          = get_prior_offset(lags);    
    table               = prior_table_root + off;
    
    int off_obs         = obs == false ? 0 : 1;

    // increase prior variance
    table[0]            = table[0] * variance_scal;
    table[1]            = table[1] * variance_scal;

    // update prior
    table[off_obs]      += 1.0;
}

bool branch_predictor::get_prediction()
{
    // get observation probabilities
    double prob_0       = get_prior_mean(false, m_lags, m_prior_table);

    // update state
    bool pred           = prob_0 >= 0.5 ? false : true;
    return pred;
}

void branch_predictor::add_observation(bool obs, bool pred)
{    
    // update prediction error
    if (obs != pred)
        m_pred_error    += 1.0;

    m_predictions       += 1.0;

    // update prior
    update_dirichler_prior(obs, m_lags, m_prior_table);
          
    // update state
    update_lags(m_lags, obs);
};

};};
