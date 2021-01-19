TARGET = DispTest.exe
COMMONDEPS = CommonUtils.o TimeGeneralInterface.o GPIOGeneralInterface.o I2CGeneralInterface.o SPIGeneralInterface.o UARTGeneralInterface.o NetworkGeneralInterface.o
LINUXDEPS = GPIO_RaspberryPi.o I2C_RaspberryPi.o SPI_RaspberryPi.o UART_RaspberryPi.o Network_RaspberryPi.o
DRIVERS = US2066Driver.o

#determine operating system to set environment
ifndef OS
	OS = $(shell uname -s)
endif

ifeq ($(OS),Windows_NT)
	ENV = Windows/MinGW
	CC = gcc
	CPP = g++
	DEL = del /Q
	CCARGS += -std=c11 -lwsock32
	CCDBG = -ggdb
	CPPARGS += -std=c++11 -lwsock32
	CPPDBG = -ggdb
	DEPS = $(COMMONDEPS) $(WINDOWSDEPS) $(DRIVERS)
endif

ifeq ($(OS),Linux)
	ENV = Linux
	CC = gcc
	CPP = g++
	DEL = rm -f
	CCARGS += -std=c11
	CCDBG = -ggdb
	CPPARGS += -std=c++11
	CPPDBG = -ggdb
	DEPS = $(COMMONDEPS) $(LINUXDEPS) $(DRIVERS)
endif

#Targets that are not file dependents
.PHONY: all clean debug drivers main dispenv

#Target to build entire project
main: dispenv $(DEPS) $(DRIVERS) $(TARGET)
	@ echo "----------------------------------------------------------"
	@ echo "Compiled All Components"
	@ echo "Deps: $(DEPS)"
	@ echo "Target: $(TARGET)"

#Workspace handling targets
all: clean main

debug: 
	@ echo "----------------------------------------------------------"
	@ echo "Building with debug symbols: $(CCDBG) / $(CPPDBG)"
	@ echo ""
	$(eval CCARGS = $(CCDBG) $(CCARGS))
	$(eval CPPARGS = $(CPPDBG) $(CPPARGS))

dispenv: 
	@ echo "----------------------------------------------------------"
	@ echo "Using Environment: $(ENV)"
	@ echo ""

clean:
	@ echo "----------------------------------------------------------"
	@ echo "Cleaning Build Space"
	$(DEL) $(TARGET)
	$(DEL) $(DEPS)
	$(DEL) $(DRIVERS)
	@ echo ""

drivers: 
	@ echo "----------------------------------------------------------"
	@ echo "Cleaning Peripheral Drivers"
	$(DEL) $(DRIVERS)
	$(DEL) $(TARGET)
	@ echo ""
	#Re-enter make with any custom parameters still set
	make CCARGS="$(CCARGS)" CPPARGS="$(CPPARGS)" 

#Dependency targets
%.o: %.c
	@ echo "----------------------------------------------------------"
	@ echo "Compiling $@"
	$(CC)  -c $^ $(CCARGS)
	@ echo ""
	
%.o: %.cpp
	@ echo "----------------------------------------------------------"
	@ echo "Compiling $@"
	$(CPP)  -c $^ $(CPPARGS)
	@ echo ""

%.exe: %.c
	@ echo "----------------------------------------------------------"
	@ echo "Compiling $@"
	$(CC) $^ $(DEPS) $(CCARGS) -o $@
	@ echo ""
	
%.exe: %.cpp
	@ echo "----------------------------------------------------------"
	@ echo "Compiling $@"
	$(CPP) $^ $(DEPS) $(CPPARGS) -o $@
	@ echo ""
