#!/bin/bash
#set -e
SCRIPT_DIR=$(dirname ${BASH_SOURCE})
#source "$SCRIPT_DIR/repo.sh" build --fetch-only $@ || exit $?

"$SCRIPT_DIR/repo.sh" build --fetch-only --no-docker $@ 

cd _build/packages

echo "Installing Warp to Python"
../target-deps/python/python -m pip install -e .

echo "Running tests"
../target-deps/python/python test_lib.py