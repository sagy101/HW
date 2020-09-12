# Makefile for the bank program
CXX = g++
CXXFLAGS = -g -Wall -std=c++11
CCLINK = $(CXX) -pthread
OBJS = account.o bank.o atm.o manager.o 
RM = rm -f
# Creating the  executable
bank: $(OBJS)
	$(CCLINK) -o Bank $(OBJS)
# Creating the object files
account.o: account.cpp account.h
atm.o: atm.cpp atm.h bank.h account.h
bank.o: bank.cpp bank.h account.h
manager.o: manager.cpp bank.h account.h atm.h

# Cleaning old files before new make
clean:
	$(RM) $(TARGET) *.o *~ "#"* core.*

