#!/bin/bash

source ./func.sh

assert 42 'main() {return 42;}'

assert 10 'main() { return 7 + 3;}'
assert 4 'main() {return 7-3;}'
assert 14 'main() {return 13+7-5-1;}'

assert 47 'main() {return 5+6*7;}'
assert 10 'main() {return 5*6/3;}'
assert 15 'main() {return 5 * (9 - 6);}'
assert 4 'main() {return ( 3+5 )/2;}'

assert 10 'main() {return -10+20;}'
assert 10 'main() {return - -10;}'
assert 10 'main() {return - - +10;}'

assert 3 'main() {return a123=3;}'
assert 8 'main() {abc1=3; xyz9=5; return abc1+xyz9;}'
assert 6 'main() {abcd=bcde=3; return abcd+bcde;}'
assert 5 'main() {return _abc=_xyz3=ddd=5;}'

assert 0 'main() {return 0==1;}'
assert 1 'main() {return 42==42;}'
assert 1 'main() {return 0!=1;}'
assert 0 'main() {return 42!=42;}'
assert 0 'main() {return 1==2==2;}'
assert 1 'main() {return 1!=2!=0;}'

assert 1 'main() {return 0<1;}'
assert 0 'main() {return 1<1;}'
assert 0 'main() {return 2<1;}'
assert 1 'main() {return 0<=1;}'
assert 1 'main() {return 1<=1;}'
assert 0 'main() {return 2<=1;}'
assert 1 'main() {return 1>0;}'
assert 0 'main() {return 1>1;}'
assert 0 'main() {return 1>2;}'
assert 1 'main() {return 1>=0;}'
assert 1 'main() {return 1>=1;}'
assert 0 'main() {return 1>=2;}'

assert 1 'main() {return 1; 2; 3;}'
assert 2 'main() {1; return 2; 3;}'
assert 3 'main() {1; 2; return 3;}'

assert 3 'main() {if (0) return 2; return 3;}'
assert 3 'main() {if (1-1) return 2; return 3;}'
assert 2 'main() {if (1) return 2; return 3;}'
assert 2 'main() {if (2-1) return 2; return 3;}'
assert 4 'main() {if (0) return 3; else return 4;}'
assert 3 'main() {if (1 > 0) return 3; else return 4;}'

assert 10 'main() {for(i=0;i<10;) i=i+1; return i;}'
assert 55 'main() {j=0; for (i=0; i<=10; i=i+1) j=i+j; return j;}'
assert 3 'main() {for (;;) return 3; return 5;}'
assert 32 'main() {x = 1; for (i = 0; i < 5; i=i+1) x = x * 2; return x;}'

assert 10 'main() {i=0; while(i<10) i=i+1; return i;}'

assert 5 'main() {;;; return 5;}'
assert 3 'main() {{ {1; {2;} return 3;} }}'

assert 10 'main() {i=0; while(i<10) { i=i+1; } return i;}'

assert 10 'main() { i=0; for(;i<10;) { i=i+1; } return i;}'
assert 10 'main() { i=0; for(;i<10;i=i+1) {} return i;}'
assert 55 'main() { { i=0; j=0; for (i=0; i<=10; i=i+1) j=i+j; return j; }}'
assert 3 'main() { { for (;;) {return 3;} return 5; }}'
assert 3 'main() { for (i = 0;;) { i = i + 1; i = i + 2; return i;}}'

assert 7 'main() { a = 1 + ret1(); return a + ret5();} ret1() { return 1;} ret5() {return 5;}'
assert 91 'main() { return arg6(1,2,3,4,5,6);} arg6(a, b, c, d, e, f) {return 1*a + 2*b + 3*c + 4*d + 5*e + 6*f;}'
assert 147 'main() { a = arg6(1,2,3,4,5,6); return a + arg6(6,5,4,3,2,1);} arg6(a, b, c, d, e, f) {return 1*a + 2*b + 3*c + 4*d + 5*e + 6*f;}'

assert 55 'main(){return fib(10);} fib(n){if (n<=1) return n; return fib(n-1) + fib(n-2);}'
