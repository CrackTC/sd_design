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
	$(MKDIR) ..$(SEP)bin
	$(MKDIR) ..$(SEP)bin$(SEP)data
	gcc -std=c99 -Wall -I ../include -L ../lib ./*.c ../utils.c ../data/*.c ../services/*.c ./edit/*.c ./page/*.c ./detail/*.c -o ../bin/gui $(LINK)
	$(MKDIR) ..$(SEP)bin$(SEP)resources
	$(CP) resources$(SEP)* ..$(SEP)bin$(SEP)resources$(SEP)

run:
	..$(SEP)bin$(SEP)gui

clean:
	$(RM) ..$(SEP)bin
