ifeq ($(OS),Windows_NT)
    MKDIR := mkdir
	CP := copy
	RM := rd /s /q
	SEP := \\
	LINK := -lm -lglfw3 -lgdi32
else
    MKDIR := mkdir -p
	CP := cp
	RM := rm -rf
	SEP := /
	LINK := -lm -lglfw -lGL
endif

build:
	$(MKDIR) bin
	$(MKDIR) bin$(SEP)data
	gcc -O2 -std=c99 -Wall -I include -L lib gui/*.c utils.c data/*.c services/*.c gui/edit/*.c gui/page/*.c gui/detail/*.c -o bin/gui $(LINK)
	$(MKDIR) bin$(SEP)resources
	$(CP) gui/resources$(SEP)* bin$(SEP)resources$(SEP)

run:
	bin$(SEP)gui

clean:
	$(RM) bin
