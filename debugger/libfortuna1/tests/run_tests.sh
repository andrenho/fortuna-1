#!/bin/sh

./test-basic "$@" && \
./test-memory "$@" && \
./test-sdcard "$@" && \
./test-z80 "$@"
