#!/bin/bash

exit

scriptroot=`dirname $0`
dstdir=$scriptroot/resources
srcdir=$scriptroot/resources-source
rc=ackrc
files=`find $srcdir -type f -printf "%P\n" | sort`

for file in $files; do
	ext="${file##*.}"
	name="${file%.*}"
	dst=$dstdir/$name
	src=$srcdir/$name.$ext
	mkdir -p `dirname $dst`
	printf "Building $dst."
	case $ext in
		bmp | cur | gif | ico | jpg | jpeg | lbm | pcx | png | pnm | tif | tiff | xcf | xpm | xv)
			echo atx
			$rc texture -o "$dst.atx" "$src"
			;;
		material)
			echo amf
			$rc material -o "$dst.amf" "$src"
			;;
		shader)
			echo asp
			$rc shader -o "$dst.asp" "$src"
			;;
		obj | 3ds | dae | fbx)
			echo amd
			$rc model -o "$dst.amd" "$src"
			;;
		wav | ogg)
			echo asn
			$rc sound -o "$dst.asn" "$src"
			;;
	esac

done