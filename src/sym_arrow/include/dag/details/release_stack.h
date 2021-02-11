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

namespace sym_dag { namespace details
{

template<class Item>
class release_stack;

template<class Item>
class release_stack_handle;

// swap contents of two vectors
template<class Item>
void    swap(release_stack<Item>& h1, release_stack<Item>& h2);

template<class Item>
class release_stack
{
    public:
        using handle_type   = release_stack_handle<Item>;

    private:
        size_t              m_capacity;
        size_t              m_pos;
        Item**              m_array;
        release_stack*      m_previous;

    private:
        release_stack(size_t capacity, Item** array_ptr);
        release_stack(const release_stack& other);

        ~release_stack();
        
        release_stack& operator=(const release_stack&) = delete;        

    public:
        // move constructor
        release_stack(release_stack&& other);        

        // move assign
        release_stack&      operator=(release_stack&& other);

        // swap contents of two vectors
        template<class Item>
        friend void         swap(release_stack<Item>& h1, release_stack<Item>& h2);

        // return true, if this vector does not contain any item
        bool                empty() const;

        // return array of items at current level
        Item**              this_level_data();

        // return number of items at current level
        size_t              this_level_size() const;

        // return previous level
        release_stack*      get_previous() const;

        // add new item
        void                push_back(const Item* it);

    private:
        void                expand(const Item* it);
        void                pop_level();
        void                push_level(size_t size, Item** arr);        

        friend handle_type;
};

template<class Item>
class release_stack_handle
{
    public:
        using stack_type    = release_stack<Item>;

    private:
        stack_type          m_stack;

        release_stack_handle(stack_type&& st);

    public:
        // move constructor
        release_stack_handle(release_stack_handle&& other);

        // move assign
        release_stack_handle& operator=(release_stack_handle&& other);

        // perform delayed destructions and clear memory
        ~release_stack_handle();

        // get stack vector
        stack_type&         get();

        // create new stack vector
        static release_stack_handle
                            get_handle();

        // create empty stack vector; when new item will be pushed to
        // this vector, then destructor will be called immediately
        static release_stack_handle
                            get_empty_handle();

        // call delayed destructors
        void                release();

        // swap contents of two handles
        template<class Item>
        friend void         swap(release_stack_handle<Item>& h1, 
                                release_stack_handle<Item>& h2);

    private:
        release_stack_handle(const release_stack_handle&) = delete;
        release_stack_handle& operator=(const release_stack_handle&) = delete;        

    private:
        static void         expand(release_stack<Item>& st);
        static bool         pop_level(release_stack<Item>& stack);
        static void         release_array(release_stack<Item>& stack);

        static Item**       get_small_array(size_t& size);
        static Item**       get_large_array(size_t& size);
        static stack_type*  malloc_ptr(const stack_type& st);
        static void         free_ptr(stack_type* ptr);

        friend release_stack<Item>;
};

};};

#include "dag/details/release_stack.inl"