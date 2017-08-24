
RESOURCES=$(shell rcc --list builtin.qrc)

all: shaders resource.o
	
depends:
	echo $(addprefix /home/felix/projects/acknext/resource/,$(RESOURCES))

%.o: %.zip
	objcopy \
		-B i386:x86-64 \
		-I binary \
		-O elf64-x86-64 \
		$< $@

resource.zip: $(RESOURCES)
	rm -f $@
	7z -tzip -mx=0 -bd a $@ $^
	wc -c $@

shaders: fragmentshaders vertexshaders

fragmentshaders: shaders/ackpbr.glsl shaders/gamma.glsl \
								 shaders/lighting.glsl shaders/object.frag
	glslangValidator -S frag $^

vertexshaders: shaders/object.vert
	glslangValidator -S vert $^
	
clean:
	rm resource.zip

qmake: resource.o
	mv $< $(OUTFILE)
	
.PHONY: clean depends