#!/bin/bash

source ./func.sh

assert 42 'return 42;'

assert 10 'return 7 + 3;';
assert 4 'return 7-3;';
assert 14 'return 13+7-5-1;';

assert 47 'return 5+6*7;'
assert 10 'return 5*6/3;'
assert 15 'return 5 * (9 - 6);'
assert 4 'return ( 3+5 )/2;'

assert 10 'return -10+20;'
assert 10 'return - -10;'
assert 10 'return - - +10;'

assert 3 'return a123=3;'
assert 8 'abc1=3; xyz9=5; return abc1+xyz9;'
assert 6 'abcd=bcde=3; return abcd+bcde;'
assert 5 'return _abc=_xyz3=ddd=5;'

assert 0 'return 0==1;'
assert 1 'return 42==42;'
assert 1 'return 0!=1;'
assert 0 'return 42!=42;'
assert 0 'return 1==2==2;'
assert 1 'return 1!=2!=0;'

assert 1 'return 0<1;'
assert 0 'return 1<1;'
assert 0 'return 2<1;'
assert 1 'return 0<=1;'
assert 1 'return 1<=1;'
assert 0 'return 2<=1;'
assert 1 'return 1>0;'
assert 0 'return 1>1;'
assert 0 'return 1>2;'
assert 1 'return 1>=0;'
assert 1 'return 1>=1;'
assert 0 'return 1>=2;'

assert 1 'return 1; 2; 3;'
assert 2 '1; return 2; 3;'
assert 3 '1; 2; return 3;'

assert 3 'if (0) return 2; return 3;'
assert 3 'if (1-1) return 2; return 3;'
assert 2 'if (1) return 2; return 3;'
assert 2 'if (2-1) return 2; return 3;'
assert 4 'if (0) return 3; else return 4;'
assert 3 'if (1 > 0) return 3; else return 4;'

assert 10 'for(i=0;i<10;) i=i+1; return i;'
assert 55 'j=0; for (i=0; i<=10; i=i+1) j=i+j; return j;'
assert 3 'for (;;) return 3; return 5;'
assert 32 'x = 1; for (i = 0; i < 5; i=i+1) x = x * 2; return x;'

assert 10 'i=0; while(i<10) i=i+1; return i;'

assert 5 ';;; return 5;'
assert 3 '{ {1; {2;} return 3;} }'

assert 10 'i=0; while(i<10) { i=i+1; } return i;'

assert 10 'i=0; for(;i<10;) { i=i+1; } return i;'
assert 10 'i=0; for(;i<10;i=i+1) {} return i;'
assert 55 '{ i=0; j=0; for (i=0; i<=10; i=i+1) j=i+j; return j; }'
assert 3 '{ for (;;) {return 3;} return 5; }'
assert 3 'for (i = 0;;) { i = i + 1; i = i + 2; return i;}'

assert 7 'a = 1 + ret1(); return a + ret5();'
