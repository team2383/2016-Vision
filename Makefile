# Source: http://stackoverflow.com/questions/9870297/makefile-to-compile-opencv-code-in-c-on-ubuntu-linux
# usage: make filename

CFLAGS = `pkg-config --cflags opencv libfreenect`
LIBS = `pkg-config --libs opencv libfreenect`

MACHINE := $(shell uname -m)

% : %.cpp
	g++ $(CFLAGS) $(USERCFLAGS) -std=c++11 $< -o $@ $(LIBS)
