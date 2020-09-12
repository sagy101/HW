# Makefile for the cache program
CC = g++
CFLAGS = -g -Wall
CCLINK = $(CC)
OBJS = cache.o cacheSim.o
RM = rm -f
# Creating the  executable
cacheSim: $(OBJS)
	$(CCLINK) -o cacheSim $(OBJS)
# Creating the object files
cache.o: cache.cpp cache.h
cacheSim.o: cacheSim.cpp
# Cleaning old files before new make
clean:
	$(RM) $(TARGET) *.o *~ "#"* core.*

