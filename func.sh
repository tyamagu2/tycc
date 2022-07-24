#!/bin/bash

cat <<EOF | cc -x c -c -o tmp2.o -
int ret1() { return 1; }
int ret5() { return 5; }
EOF

assert() {
  expected="$1"
  input="$2"

  ./tycc "$input" > tmp.s
  cc -o tmp tmp.s tmp2.o
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

