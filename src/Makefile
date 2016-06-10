
# File: Makefile
# File history:
#      v0.0: 13/05/2016: Creation
#
# Description:   
#
# Makefile for the SDG2 project.
#
# Author: Karen Flores y Adrián García
#------------------------------------------------------------------------------

# Macros
CC=gcc
CFLAGS=-g -Wall -O -I. -DNDEBUG
LDFLAGS=-lrt
LDLIBS=-lwiringPi

all: main

main: main.o fsm.o tmr.o led_control.o players.o

clean:
	$(RM) *.o *~ main libwiringPi.a

libwiringPi.a: wiringPi.o
	ar rcs $@ $^
