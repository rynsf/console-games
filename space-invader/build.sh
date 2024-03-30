#!/bin/sh

set -xe

gcc -g -c ../fender.c
gcc -g -c space-invader.c
gcc space-invader.o fender.o -lncursesw -o space-invader.out
