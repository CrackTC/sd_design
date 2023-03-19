build:
	mkdir -p ../bin
	gcc -std=c99 -Wall -Og -lm -lGLEW -lGL -lglfw *.c ../utils.c ../data/*.c -o ../bin/gui
	mkdir -p ../bin/resources
	cp resources/*.tt[cf] ../bin/resources/

run:
	../bin/gui

clean:
	rm -rf ../bin
