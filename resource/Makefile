
RESOURCES=beep.wav

all: resource.o

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

clean:
	rm resource.zip