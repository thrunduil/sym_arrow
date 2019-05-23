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
#include "registered_symbols.h"
#include "dag/details/object_table.inl"
#include "sym_arrow/ast/ast.h"

namespace sym_arrow { namespace ast
{

symbol_codes::symbol_codes()
    : m_current_symbol_code(0)
{};

symbol_codes::~symbol_codes()
{
    close();
};

void symbol_codes::close()
{
    m_free_codes = dbs_lib::dbs();
};

void symbol_codes::release_code(size_t code)
{
    m_free_codes    = std::move(m_free_codes).set(code);
};

size_t symbol_codes::get_fresh_code()
{
    if (m_free_codes.any() == true)
    {
        size_t code     = m_free_codes.first();
        m_free_codes    = std::move(m_free_codes).reset(code);

        return code;
    };

    return m_current_symbol_code++;
};

registered_symbols::reg_sym_ptr registered_symbols::m_handle = nullptr;

registered_symbols::registered_symbols()
    :m_pool(sizeof(code_ident)), m_refcount(0)
{};

registered_symbols::~registered_symbols()
{
    release_memory();
};

void registered_symbols::release_memory()
{
    m_code_ident_map.close();
    m_pool.purge_memory();
};

void registered_symbols::close()
{
    --m_refcount;

    if (m_refcount == 0)
    {
        m_handle = nullptr;
        delete this;
    };
};

void registered_symbols::register_ident(const identifier_rep* h)
{
    size_t code     = h->get_identifier_code();
    code_ident* ptr = (code_ident*)m_pool.malloc(); 

    new(ptr) code_ident(code,h);

    auto pos        = m_code_ident_map.get(code);
    pos.assign(ptr);
};

void registered_symbols::unregister_ident(const identifier_rep* h)
{
    size_t code     = h->get_identifier_code();

    auto pos        = m_code_ident_map.get(code);
    code_ident* ptr = *pos;
    pos.assign<code_ident>(nullptr);
    m_pool.free(ptr);

    m_free_codes.release_code(code);
};

const identifier_rep* registered_symbols::get_ident_from_code(size_t code) const
{
    auto pos = m_code_ident_map.find(code);

    if (pos == nullptr)
        return nullptr;

    const identifier_rep* h = pos->second;
    return h;
};

size_t registered_symbols::get_fresh_identifier_code()
{
    return m_free_codes.get_fresh_code();
};

registered_symbols::reg_sym_ptr registered_symbols::get()
{
    if (m_handle == nullptr)
        m_handle = new registered_symbols();

    ++m_handle->m_refcount;
    return m_handle;
}

};};
