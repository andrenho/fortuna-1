#!/bin/sh

./test-basic "$@" && \
./test-load "$@" && \
./test-memory "$@" && \
./test-sdcard "$@" && \
./test-z80 "$@"
