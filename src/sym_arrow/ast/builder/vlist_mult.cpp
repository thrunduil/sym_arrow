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

#include "sym_arrow/ast/builder/vlist_mult.inl"
#include "sym_arrow/ast/ast.h"

namespace sym_arrow { namespace ast
{

//----------------------------------------------------------------------
//                        ilist_mult
//----------------------------------------------------------------------
void ilist_mult::make_pow(const value& p, rlist_mult& rl) const
{
    int tot_pow             = 1;
    const ilist_mult* list  = this;

    while (list != nullptr)
    {
        size_t cur_size     = list->current_size();

        for (size_t i = 0; i < cur_size; ++i)
        {
            const item_type& e  = list->elem(i);
            int pow             = e.get_value() * tot_pow;

            rl.push_back(p * value::make_value(pow), e.get_expr_handle());
        };

        tot_pow                 = tot_pow * list->get_pow();
        list                    = list->has_previous()? list->get_previous() : nullptr;
    };
};

};};
