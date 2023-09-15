#!/bin/sh

set -xe

#gcc ./animal.c -O0 -g -Wall -o ./animal && valgrind --leak-check=full --show-leak-kinds=all ./animal

gcc ./animal.c -Wall -o ./animal
