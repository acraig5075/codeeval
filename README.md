# codeeval
My solutions to CodeEval.com challenges

These challenges offer a nice opportunity to solve problems not of the sort I usually experience at work. I try look for challenges where I recognise the need to use std library functions or techniques that I'm not that familiar with as a learning opportunity. 


## [Challenge #14: String permutations](https://www.codeeval.com/open_challenges/14/) - [code](14-permutations/main.cpp)
1. Generic way of for-each-line-in-file perform operation
2. std::next_permutation()

## [Challenge #48: Discount offers (incomplete)](https://www.codeeval.com/open_challenges/48/) - [code](48-discounts/main.cpp),
1. std::next_permutation()
2. std::set_intersection()
3. std::iota()
4. pointers-to-struct-members
5. using lambdas with std::sort and std::count_if

## Challenge [#86](https://www.codeeval.com/open_challenges/86/): Poker hands - [code](86-poker/main.cpp), 
1. Overridden comparison operator
2. Custom istream and ostream operators
3. std::iota() and std::next_permutation()
4. generic comparison of two ranges, with a custom binary predicate function.

## Challenge [#108](https://www.codeeval.com/open_challenges/108/): Terminal - [code](108-terminal/main.cpp), 
1. good structure and separation of concerns
2. raw string literals
3. std::is_digit()
4. std::copy_n()



## Listing of noteworthy features and the code I use that I may want to refer back to.

### Standard library calls
| Function                | Code
|-------------------------|------
| std::next_permutation() | [14](14-permutations/main.cpp), [code](48-discounts/main.cpp), [code](86-poker/main.cpp), 
| std::set_intersection() | [code](48-discounts/main.cpp),
| std::iota() | [code](48-discounts/main.cpp),
| std::is_digit() | [code](108-terminal/main.cpp), 
| std::copy_n() | [code](108-terminal/main.cpp), 

### Code snippets

for-each-line-in-file operation [14](14-permutations/main.cpp), 
pointers-to-struct-members [code](48-discounts/main.cpp),
raw string literals [code](108-terminal/main.cpp), 
using lambdas as algorithm predicates [code](48-discounts/main.cpp),
Custom comparison operator [code](86-poker/main.cpp), 
Custom istream and ostream operators [code](86-poker/main.cpp), 
generic comparison of two ranges [code](86-poker/main.cpp), 
good separation of concerns [code](108-terminal/main.cpp), 
