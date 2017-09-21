# codeeval
My solutions to CodeEval.com challenges

These challenges offer a nice opportunity to solve problems not of the sort I usually experience at work. I try look for challenges where I recognise the need to use std library functions or techniques that I'm not that familiar with as a learning opportunity. 


## Listing of noteworthy features and the code I use that I may want to refer back to.

### Standard library calls

| Function                | Code
|-------------------------|------
| std::next_permutation() | [14](14-permutations/main.cpp), [48](48-discounts/main.cpp), [86](86-poker/main.cpp)
| std::set_intersection() | [48](48-discounts/main.cpp)
| std::iota() | [48](48-discounts/main.cpp)
| std::isdigit() | [108](108-terminal/main.cpp), [51](51-closest-pair/main.cpp)
| std::isblank() | [51](51-closest-pair/main.cpp)
| std::copy_n() | [108](108-terminal/main.cpp)
| std::minmax_element() | [213](213-lakes/main.cpp)
| std::numeric_limits() | [51](51-closest-pair/main.cpp)
| std::accumulate() | [69](69-distinct-subsequences/main.cpp)

### Code snippets

| Snippet                               | My code
|---------------------------------------|--------
| for-each-line-in-file operation  | [14](14-permutations/main.cpp)
| pointers-to-struct-members  | [48](48-discounts/main.cpp)
| raw string literals  | [108](108-terminal/main.cpp)
| using lambdas as algorithm predicates  | [48](48-discounts/main.cpp)
| Custom comparison operator  | [86](86-poker/main.cpp)
| Custom istream and ostream operators  | [86](86-poker/main.cpp), [176](176-ray-of-light/main.cpp)
| generic comparison of two ranges  | [86](86-poker/main.cpp)
| good separation of concerns  | [108](108-terminal/main.cpp)
| erase-remove idiom  | [213](213-lakes/main.cpp)
| is string numeric  | [51](51-closest-pair/main.cpp)
| test file comparison  | [176](176-ray-of-light/main.cpp)
| substr of massive string with wildcards | [28](28-string-searching/main.cpp)
