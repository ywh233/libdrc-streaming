CXX_COMPILER ?= clang++

get_cflags = $(shell pkg-config --cflags $(1))
get_libflags = $(shell pkg-config --libs $(1))

CXXFLAGS = --std=c++14 $(call get_cflags, x264) $(call get_cflags, libswscale)
LDFLAGS =$(call get_libflags, x264) $(call get_libflags, libswscale)

LIBDRC_STREAMING_SRCS:=$(wildcard src/streamer/*.cpp)
LIBDRC_STREAMING_OBJS:=$(LIBDRC_STREAMING_SRCS:.cpp=.o)

ALL_SRCS:=$(LIBDRC_STREAMING_SRCS)
ALL_OBJS:=$(ALL_SRCS:.cpp=.o)
ALL_DEPENDS:=$(ALL_SRCS:.cpp=.d)

all: demo

demo: make_out
	$(CXX_COMPILER) $(CXXFLAGS) $(ALL_SRCS) -I include/ -o out/demo $(LDFLAGS)

clean:
	rm -r out

make_out:
	mkdir -p out
