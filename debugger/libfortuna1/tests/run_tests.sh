#!/bin/sh

./test-compiler && \
./test-basic "$@" && \
./test-memory "$@" && \
./test-sdcard "$@" && \
./test-z80 "$@"
