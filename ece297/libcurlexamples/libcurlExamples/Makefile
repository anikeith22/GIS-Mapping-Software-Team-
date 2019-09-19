CFLAGS := $(shell curl-config --cflags)
LFLAGS := $(shell curl-config --libs)

all: libcurl_ex1 libcurl_ex2 libcurl_ex3 libcurl_ex4 libcurl_ex5 libcurl_ex6

libcurl_ex1: libcurl_ex1.cpp
	g++ $(CFLAGS) $^ $(LFLAGS) -o $@

libcurl_ex2: libcurl_ex2.cpp
	g++ $(CFLAGS) $^ $(LFLAGS) -o $@

libcurl_ex3: libcurl_ex3.cpp
	g++ $(CFLAGS) $^ $(LFLAGS) -o $@

libcurl_ex4: libcurl_ex4.cpp
	g++ $(CFLAGS) $^ $(LFLAGS) -o $@

libcurl_ex5: libcurl_ex5.cpp
	g++ $(CFLAGS) $^ $(LFLAGS) -o $@

libcurl_ex6: libcurl_ex6.cpp
	g++ $(CFLAGS) $^ $(LFLAGS) -o $@

clean:
	rm libcurl_ex1 libcurl_ex2 libcurl_ex3 libcurl_ex4 libcurl_ex5 libcurl_ex6
