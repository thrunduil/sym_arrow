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

#include "sym_arrow/config.h"
#include "sym_arrow/ast/helpers/utils.h"
#include <ostream>

namespace sym_arrow { namespace ast
{

// predict realization of boolean variable
class branch_predictor
{
    private:
        static constexpr double variance_scal   = 0.995;

    private:
        static const int n_lags     = 5;
        static const int data_size  = 0;
        
        bool            m_lags[n_lags];

        // prior table; 2 prior parameters
        double          m_prior_table[(2 + data_size) * details::pow2(n_lags)];

        double          m_pred_error;
        double          m_predictions;
        int             m_tag;

    public:
        branch_predictor();

        branch_predictor(int tag);

        // get prediction of the binary variable; before calling this
        // function again add_observation must be called
        bool            get_prediction();

        // report new observation; get_prediction must be called first
        void            add_observation(bool val, bool pred);

        // assign a tag; mostly for debugging purposes
        void            set_tag(int tag);

        // print statistics
        void            print_stats(std::ostream& os);

    private:
        void            initialize_priors();
        void            initialize_state();

        static void     update_lags(bool* lags, bool obs);

        static size_t   get_prior_offset(const bool* lags);
        static void     update_dirichler_prior(bool obs, const bool* lags, 
                            double* prior_table);
        static double   get_prior_mean(bool obs, const bool* lags, 
                            const double* prior_table);
};

};};
