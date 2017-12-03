#!/bin/bash
objdump -t /tmp/build-acknext-Desktop-Debug/acknext/libacknext.so \
	| grep -v "\s_" \
	| sed -E "s/\s+/ /g" \
	| cut -d " " -f 6 \
	| grep -v "\." \
	| sort \
	| uniq
