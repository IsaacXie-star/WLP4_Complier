#! /bin/bash

clear
g++ wlp4type.cc
wlp4scan < program.wlp4 | wlp4parse > program.wlp4i
wlp4type < program.wlp4i > expected.txt
./a.out < program.wlp4i > output.txt
diff output.txt expected.txt
