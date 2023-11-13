CC=gcc
LINKER=-lcurl -liup

ifeq ($(OS), Windows_NT)
	exe=STranslate.exe
else
	exe=STranslate
endif

build:
	$(CC) STranslate.c lib/cJSON.c -o $(exe) $(LINKER)

install:
	cp $(exe) /usr/bin

uninstall:
	rm /usr/bin/$(exe)

run:
	./$(exe)

clean:
	rm $(exe)
