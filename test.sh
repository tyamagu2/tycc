#!/bin/bash

source ./func.sh

assert 0 '0;'
assert 42 '42;'

assert 10 '7 + 3;';
assert 4 '7-3;';
assert 14 '13+7-5-1;';

assert 47 '5+6*7;'
assert 10 '5*6/3;'
assert 15 '5 * (9 - 6);'
assert 4 '( 3+5 )/2;'

assert 10 '-10+20;'
assert 10 '- -10;'
assert 10 '- - +10;'

assert 5 '3; 1+4;'

assert 3 'a=3;'
assert 8 'a=3; z=5; a+z;'
assert 6 'a=b=3; a+b;'

assert 3 'ab123=3;'
assert 8 'abc1=3; xyz9=5; abc1+xyz9;'
assert 6 'abcd=bcde=3; abcd+bcde;'
assert 5 '_abc=_xyz3=ddd=5;'