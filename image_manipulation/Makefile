all: imagetest.cpp
	g++ -g -O2 -o imagetest imagetest.cpp linkedlist.c `Magick++-config --cppflags --cxxflags --ldflags --libs`

clean:
	rm imagetest
