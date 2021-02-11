# sym_arrow

C++ library for symbolic differentiation. 
Sym_arrow provides only basic algebraic capabilities but instead is concentraced on
calculating symbolic derivatives of expressions as fast as possible.

## Code example

A complete C++ program that computes (not normalized) 
Laguerre Polynomials and the associated Laguerre Polynomials.
```cpp
#include "sym_arrow/sym_arrow.h"

using namespace sym_arrow;

// Laguerre Polynomial L_n(x) == exp(x) * (d/dx)^n [x^n * exp(-x)]
expr laguerre_poly(int n, const symbol & x)
{
    expr ker    = exp(-x);
    expr poly   = power_int(x, n) * ker;
    return diff(poly, x, n) / ker;
}

// associated Laguerre Polynomial L_{n,m}(x) == (d/dx)^m [L_n(x)]
expr assoc_laguerre_poly(int n, int m, const symbol & x)
{
    expr poly   = laguerre_poly(n, x);
    poly        = simplify(poly);
    return diff(poly, x, m);
}
void example()
{    
    tic();

    expr poly   = assoc_laguerre_poly(100, 100, symbol("x"));

    tocdisp();
};

Output:
Elapsed time is 0.003435 seconds.
```            

For comparison Matlab needs 19.047018 sec to compute this polynomial, over 5500 times
longer.

## Licence

This library is published under GPL licence.