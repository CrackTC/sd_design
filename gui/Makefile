build:
	mkdir -p ../bin
	clang -std=c99 -Wall -lm -lGLEW -lGL -lglfw main.c ../utils.c -o ../bin/gui
	mkdir -p ../bin/resources
	cp resources/*.tt[cf] ../bin/resources/

run:
	../bin/gui

clean:
	rm -rf ../bin
