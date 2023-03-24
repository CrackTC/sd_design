ifeq ($(OS),Windows_NT)
    MKDIR := mkdir
	CP := copy
	RM := rd /s /q
	SEP := \\
else
    MKDIR := mkdir -p
	CP := cp
	RM := rm -rf
	SEP := /
endif

build:
	$(MKDIR) ..$(SEP)bin
	gcc -std=c99 -Wall -I ../include -L ../lib ./*.c ../utils.c ../data/*.c ../services/*.c ./edit/*.c ./page/*.c ./detail/*.c -o ../bin/gui -lm -lglfw3 -lgdi32
	$(MKDIR) ..$(SEP)bin$(SEP)resources
	$(CP) resources$(SEP)* ..$(SEP)bin$(SEP)resources$(SEP)

run:
	..$(SEP)bin$(SEP)gui

clean:
	$(RM) ..$(SEP)bin