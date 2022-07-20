#!/bin/bash

source ./assert.sh

assert 0 '0'
assert 42 '42'

assert 10 '7 + 3';
assert 4 '7-3';
assert 14 '13+7-5-1';
