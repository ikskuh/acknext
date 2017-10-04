#!/bin/bash
set -o pipefail
glslangValidator "$1" \
	| grep -E "ERROR:[[:space:]]*[[:digit:]]+:[[:digit:]]+" \
	| sed  -E 's/.*([[:digit:]]+):([[:digit:]]+).*:[[:space:]]*(.*)/"\2:1: \3"/' \
	| xargs -n1 printf "%s:%s\n" "$1"
	>&2 # into stderr!
exit $?
