# MicroControllers
Headers and libraries for at home micro controller projects

Currently a storage place for all the work I'm doing with Arduino's connected to various bits of hardware.

# Arduino Headers
Collection of headder files written specifically for arduino.

Includes some convenience objects and interfaces intended to simplify firmware creation.

# Complete
Projects that were completed into a workign device of some sort.  This may be include a PCB layout in addition to the source code controllign it.

# Fritzing Parts
PCBs designed through Fritzing are being used to test these drivers and work on the projects.  Customized parts for these circuit boards are stroed in this directory.

# GenIface Drivers
One of the projects underway is a set of generic C interfaces to use to communicate with peripherals.  These interfaces stipulate a minimum set of functionality that the bus can provide in a standard set of functions.  Peripheral drivers are built upon this interface to abstract them from microprocessor specific code.  The end goal was to have a set of drivers that can be quickly brought from processor to processor.

This directory contains only the peripheral drivers that were built upon this set of interfaces.

# Generic Libs
Header files and libraries for code that is not tied to a particular environment or processor.

Also includes the general interface header files that specify the standard function and feature set for each communication bus.

# Nucleo Headers
Header files built for use on STM32 Nucleo boards.  Includes implementations of the generic interfaces.

# RasPiHeaders
Header files built for use on Raspberry Pi's.  Includes implementationsof the generic interface components.

# Projects
Folders containing specific projects being built.