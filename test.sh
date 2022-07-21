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
