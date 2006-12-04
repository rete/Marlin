#########################################################
#
# top level GNUmakefile to build Marlin with addtional 
# user packages containing processors
#
#  usage: all additional packages are expected to live 
#  in the packages subdirectory with the following structure:
#         ./packages/MyPackage/include
#                             ./src/
#                             ./src/GNUmakefile
#                             ./lib
#
#  where the GNUmakefile has a target 'lib; that creates
#  a library   ./packages/MyPackage/lib/libMyPackage.a  
#  see e.g.:  $MARLIN/examples/mymarlin
#  
#
# @author Frank Gaede, DESY
# @version $Id: GNUmakefile,v 1.10 2006-12-04 14:11:59 gaede Exp $
#
#########################################################


#
# ---------------- additional user libs are defined in userlibs.gmk  ------------------- 
#
USERINCLUDES =  
USERLIBS = 

include ./userlib.gmk

export USERINCLUDES
export USERLIBS

#
#-------------------------------------------------------------------
#


subdirs := $(patsubst packages/%/src/GNUmakefile,packages/%, $(wildcard packages/*/src/GNUmakefile) )


packages := $(patsubst packages/%,%,$(subdirs)) 

MARLINLIB := -Wl,-whole-archive $(foreach package,$(packages),  -L $(MARLIN)/packages/$(package)/lib -l$(package) ) -Wl,-no-whole-archive
export MARLINLIB

packagelibs := $(foreach package,$(packages), packages/$(package)/lib/lib$(package).a )
export packagelibs


.PHONY: lib clean bin doc $(subdirs) test



ifdef MARLIN_GUI
all: lib bin gui
else
all: lib bin
endif


ifdef MARLIN_GUI
.PHONY: gui

gui: ./bin/MarlinGUI

./bin/MarlinGUI: ./bin/Marlin
	@if [ $(MARLIN_GUI) ] ; then \
	if [ -f "${QTDIR}/bin/qmake" ] ; then \
	echo "Building Marlin GUI..."; \
	rm ./bin/MarlinGUI ; \
	cd "${MARLIN}/gui"; qmake ; $(MAKE) ; fi ; fi


endif

lib:
	$(MAKE) -C src lib
	@for i in $(subdirs); do \
	if [ -f "$$i/src/GNUmakefile" ] ; then \
	echo "Building library for $$i..."; \
	(cd $$i/src; $(MAKE) $(MFLAGS) $(MYMAKEFLAGS) lib); fi ; done

bin: lib ./bin/Marlin

./bin/Marlin: ./lib/libMarlin.a $(packagelibs)
	$(MAKE) -C src rebuild


doc:
	$(MAKE) -C src doc


clean:
	$(MAKE) -C src clean
	@for i in $(subdirs); do \
	if [ -f "$$i/src/GNUmakefile" ] ; then \
	echo "Clearing in $$i..."; \
	(cd $$i/src; $(MAKE) $(MFLAGS) $(MYMAKEFLAGS) clean); fi ; done ; \
	if [ -f "gui/Makefile" ] ; then \
	echo "Clearing in gui..."; \
	(cd $(MARLIN)/gui; $(MAKE) clean); rm -f Makefile; fi

test:
	@echo $(subdirs)
	@echo $(packages)
	@echo $(packagelibs)
	@ls -l $(MARLIN)/bin/Marlin
	@for i in $(subdirs); do \
	echo "ls -l $$i" ; \
	ls -l $$i/lib ; done

