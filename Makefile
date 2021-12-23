# Make file to compile C++ Codes for Servers and Clients for the
# Social Matching Service. Sample taken from Makefile documentation

# shared for multiple .o files
CPP = g++
# Added constant to remove output flag from commands
COUTFLAGS  = -o

# default command to compile all files
default: all

# Compile all files when default execution is picked
all:  common.o serverC.o serverT.o serverS.o serverP.o clientA.o clientB.o

# Compiles Commons code
common.o:  common.h 
	$(CPP) $(COUTFLAGS) common common.h

# Compiles ServerP
serverP.o:  serverP.cpp
	$(CPP) $(COUTFLAGS) serverP serverP.cpp

# Compiles and executes ServerP
serverP:  common.o serverP.o
	./serverP

# Compiles ServerS
serverS.o:  serverS.cpp 
	$(CPP) $(COUTFLAGS) serverS serverS.cpp

# Compiles and executes ServerS
serverS:  common.o serverS.o
	./serverS

# Compiles ServerT
serverT.o:  serverT.cpp
	$(CPP) $(COUTFLAGS) serverT serverT.cpp

# Compiles and executes ServerT
serverT:  common.o serverT.o
	./serverT

# Compiles ServerC
serverC.o:  central.cpp
	$(CPP) $(COUTFLAGS) central central.cpp

# Compiles and executes ServerC
serverC:  common.o serverC.o
	./central

# Compiles Client A
clientA.o:  clientA.cpp
	$(CPP) $(COUTFLAGS) clientA clientA.cpp

# Compiles and executes Client A
clientA:  common.o clientA.o
	./clientA

# Compiles Client B
clientB.o:  clientB.cpp
	$(CPP) $(COUTFLAGS) clientB clientB.cpp

# Compiles and executes Client B
clientB: common.o clientB.o
	./clientB

# Removes all executable files and backup files created
clean: 
	$(RM) serverC serverT serverS serverP clientA clientB *~