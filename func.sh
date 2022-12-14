#!/bin/bash

assert() {
  expected="$1"
  input="$2"

  ./tycc "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

run() {
  input="$1"

  ./tycc "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  echo $?
}

