#
# Simple makefile for monochrome BMP to RLE data
#
all: crunch_bmp_rle

crunch_bmp_rle:	crunch_bmp_rle.cpp
	$(CXX) -Wall -Wextra -Os -o $@ $<

clean:
	$(RM) crunch_bmp_rle crunch_bmp_rle.exe
