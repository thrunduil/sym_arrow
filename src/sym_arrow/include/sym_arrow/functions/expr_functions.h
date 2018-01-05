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

#pragma once

#include "sym_arrow/nodes/expr.h"
#include "sym_arrow/functions/contexts.h"

namespace sym_arrow
{

// display expression on a stream; add new line at the end if 
// add_newline = true; expression is cannonized
void SYM_ARROW_EXPORT   disp(std::ostream& os, const expr& ex, 
                            bool add_newline = true);

// display expression on cout stream; add new line at the end if 
// add_newline = true; expression is cannonized
void SYM_ARROW_EXPORT   disp(const expr& ex, bool add_newline = true);

// display expression on a stream; add new line at the end if 
// add_newline = true; expression is not cannonized
void SYM_ARROW_EXPORT   disp_nocannonize(std::ostream& os, const expr& ex, 
                            bool add_newline = true);

// set/get precision used to display values; when precision is negative, then
// values will be displayed with precision allowing to read it back exactly
// (assuming that the input and output variables are represented with the same
// precision); when precision is zero, then values will be displayed according
// to the format of the output steam; when precision is positive, then values 
// will be displayed with specified number of significant digits
int SYM_ARROW_EXPORT    get_disp_precision();
void SYM_ARROW_EXPORT   set_disp_precision(int prec);

// set/get default precision (in bits) used to represent floating point numbers, 
// for example precision used to represent double precision values is 53; these 
// functions have effect only when SYM_ARROW_VALUE_TYPE == SYM_ARROW_VALUE_MP
int SYM_ARROW_EXPORT    get_default_precision();
void SYM_ARROW_EXPORT   set_default_precision(int prec);

// convert expression to a string; expression is cannonized
std::string SYM_ARROW_EXPORT 
                        to_string(const expr& ex);

// create expression for a string representation
expr SYM_ARROW_EXPORT   parse(const std::string& expression_string);

// check internal representation; for debug purpose only
bool SYM_ARROW_EXPORT   check_expression(const expr& ex);

// calculate hash value; expression ex is not cannonized
size_t SYM_ARROW_EXPORT hash_value(const expr& ex);

// return true in an expression contains a symbol sym;
// expression ex is cannonized first
bool SYM_ARROW_EXPORT   contain_symbol(const expr& ex, const symbol& sym);

// return true in an expression contains any of symbol syms;
// expression ex is cannonized first
bool SYM_ARROW_EXPORT   contain_any(const expr& ex, const std::vector<symbol>& syms);

// return true in an expression contains all of symbol syms;
// expression ex is cannonized first
bool SYM_ARROW_EXPORT   contain_all(const expr& ex, const std::vector<symbol>& syms);        

// addition
expr SYM_ARROW_EXPORT   operator+(const expr& a, const expr& b);
expr SYM_ARROW_EXPORT   operator+(expr&& a, const expr& b);
expr SYM_ARROW_EXPORT   operator+(const expr& a, expr&& b);
expr SYM_ARROW_EXPORT   operator+(expr&& a, expr&& b);
expr SYM_ARROW_EXPORT   operator+(const value& a, const expr& b);
expr SYM_ARROW_EXPORT   operator+(const value& a, expr&& b);
expr SYM_ARROW_EXPORT   operator+(const expr& a, const value& b);
expr SYM_ARROW_EXPORT   operator+(expr&& a, const value& b);

// subtraction
expr SYM_ARROW_EXPORT   operator-(const expr& a, const expr& b);
expr SYM_ARROW_EXPORT   operator-(expr&& a, const expr& b);
expr SYM_ARROW_EXPORT   operator-(const expr& a, expr&& b);
expr SYM_ARROW_EXPORT   operator-(expr&& a, expr&& b);
expr SYM_ARROW_EXPORT   operator-(const value& a, const expr& b);
expr SYM_ARROW_EXPORT   operator-(const value& a, expr&& b);
expr SYM_ARROW_EXPORT   operator-(const expr& a, const value& b);
expr SYM_ARROW_EXPORT   operator-(expr&& a, const value& b);

// unary minus
expr SYM_ARROW_EXPORT   operator-(const expr& a);
expr SYM_ARROW_EXPORT   operator-(expr&& a);

// multiplication
expr SYM_ARROW_EXPORT   operator*(const expr& a, const expr& b);
expr SYM_ARROW_EXPORT   operator*(expr&& a, const expr& b);
expr SYM_ARROW_EXPORT   operator*(const expr& a, expr&& b);
expr SYM_ARROW_EXPORT   operator*(expr&& a, expr&& b);
expr SYM_ARROW_EXPORT   operator*(const value& a, const expr& b);
expr SYM_ARROW_EXPORT   operator*(const value& a, expr&& b);
expr SYM_ARROW_EXPORT   operator*(const expr& a, const value& b);
expr SYM_ARROW_EXPORT   operator*(expr&& a, const value& b);

// division
expr SYM_ARROW_EXPORT   operator/(const expr& a, const expr& b);
expr SYM_ARROW_EXPORT   operator/(expr&& a, const expr& b);
expr SYM_ARROW_EXPORT   operator/(const expr& a, expr&& b);
expr SYM_ARROW_EXPORT   operator/(expr&& a, expr&& b);
expr SYM_ARROW_EXPORT   operator/(const value& a, const expr& b);
expr SYM_ARROW_EXPORT   operator/(const value& a, expr&& b);
expr SYM_ARROW_EXPORT   operator/(const expr& a, const value& b);
expr SYM_ARROW_EXPORT   operator/(expr&& a, const value& b);

// integer power
expr SYM_ARROW_EXPORT   power_int(const expr& a, int p);
expr SYM_ARROW_EXPORT   power_int(expr&& a, int p);

// real power
expr SYM_ARROW_EXPORT   power_real(const expr& a, const expr& b);
expr SYM_ARROW_EXPORT   power_real(expr&& a, const expr& b);
expr SYM_ARROW_EXPORT   power_real(const expr& a, expr&& b);
expr SYM_ARROW_EXPORT   power_real(expr&& a, expr&& b);
expr SYM_ARROW_EXPORT   power_real(const value& a, const expr& b);
expr SYM_ARROW_EXPORT   power_real(const value& a, expr&& b);
expr SYM_ARROW_EXPORT   power_real(const expr& a, const value& b);
expr SYM_ARROW_EXPORT   power_real(expr&& a, const value& b);

// exponent function
expr SYM_ARROW_EXPORT   exp(const expr& a);
expr SYM_ARROW_EXPORT   exp(expr&& a);

// logarithm function   
expr SYM_ARROW_EXPORT   log(const expr& a);
expr SYM_ARROW_EXPORT   log(expr&& a);

// absolute function
expr SYM_ARROW_EXPORT   abs(const expr& a);
expr SYM_ARROW_EXPORT   abs(expr&& a);

// make function 
expr SYM_ARROW_EXPORT   function(const symbol& sym);
expr SYM_ARROW_EXPORT   function(const symbol& sym, const expr& arg1);
expr SYM_ARROW_EXPORT   function(const symbol& sym, const expr& arg1, const expr& arg2);
expr SYM_ARROW_EXPORT   function(const symbol& sym, const std::vector<expr>& arg);
expr SYM_ARROW_EXPORT   function(const symbol& sym, const expr* arg, size_t n);

// comparison function based on addresses of pointers
bool SYM_ARROW_EXPORT   operator<(const expr& s1, const expr& s2);
bool SYM_ARROW_EXPORT   operator>(const expr& s1, const expr& s2);
bool SYM_ARROW_EXPORT   operator<=(const expr& s1, const expr& s2);
bool SYM_ARROW_EXPORT   operator>=(const expr& s1, const expr& s2);
bool SYM_ARROW_EXPORT   operator==(const expr& s1, const expr& s2);
bool SYM_ARROW_EXPORT   operator!=(const expr& s1, const expr& s2);

// return expression type code
SYM_ARROW_EXPORT ast::term_types     get_expression_type(const expr& ex);

// cast functions; expression must has valid type
SYM_ARROW_EXPORT const scalar&       cast_scalar(const expr& ex);
SYM_ARROW_EXPORT const symbol&       cast_symbol(const expr& ex);
SYM_ARROW_EXPORT const add_expr&     cast_add(const expr& ex);
SYM_ARROW_EXPORT const mult_expr&    cast_mult(const expr& ex);
SYM_ARROW_EXPORT const function_expr& cast_function(const expr& ex);

};

