#!/bin/sh
git submodule update --init --recursive

sudo apt update && sudo apt install pkg-config libjson-c-dev
