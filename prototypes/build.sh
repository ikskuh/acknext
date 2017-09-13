#!/bin/bash

dstdir=resources
srcdir=resources-source
rc=ackrc
files=`find $srcdir -type f`

function src2dst()
{
	echo $1 | sed -e "s/^$srcdir/$dstdir/"
}

for file in $files; do
	ext="${file##*.}"
	name="${file%.*}"
	dst="`src2dst $name`"
	mkdir -p `dirname $dst`
	printf "Building $dst."
	case $ext in
		bmp | cur | gif | ico | jpg | jpeg | lbm | pcx | png | pnm | tif | tiff | xcf | xpm | xv)
			echo atx
			$rc texture -o "$dst.atx" "$file"
			;;
	esac

done