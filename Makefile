# Master makefile for fastcap and related programs and documentation

# assumed places for unaliased executables
SHELL = /bin/sh
AWK = awk
RM = /bin/rm
CAT = /bin/cat

# changed using `config [type]' - called makefiles switched around
TYPE = other

SRC = src
DOC = doc

default:
	@echo Specify what to make:
	@echo " fastcap - capacitance calculation program"
	@echo " cubegen - cubic capacitor input file generator"
	@echo " capgen - parallel plate capacitor input file generator"
	@echo " busgen - bus crossing input file generator"
	@echo " pipedgen - parallelepiped input file generator"
	@echo " pyragen - pyramid input file generator"
	@echo " all - all of the above"
	@echo " manual - run latex to produce the manual in .dvi format"
	@echo " clean - remove object files"

fastcap: setmake
	cd $(SRC) ; $(MAKE) fastcap

cubegen: setmake
	cd $(SRC) ; $(MAKE) cubegen

capgen: setmake
	cd $(SRC) ; $(MAKE) capgen

busgen: setmake
	cd $(SRC) ; $(MAKE) busgen

pipedgen: setmake
	cd $(SRC) ; $(MAKE) pipedgen

pyragen: setmake
	cd $(SRC) ; $(MAKE) pyragen

all: fastcap cubegen capgen busgen pipedgen pyragen

setmake: 
	@cd $(SRC) ; $(MAKE) setmake TYPE=$(TYPE)

manual:
#	@echo no manual at this time
	cd $(DOC) ; $(MAKE) manual

clean:
	cd $(SRC) ; $(MAKE) clean
