CXX = g++
CXXFLAGS = $(COMMONFLAGS) --std=c++0x

 
COMMONFLAGS = -O2 -Wall -Wextra -pedantic -Wno-deprecated-register -Wno-switch
LDFLAGS = -lsfml-system -lsfml-graphics -lsfml-window 

BINFILE = codi

CXXFILES := $(shell find src -mindepth 1 -maxdepth 4 -name "*.cpp")
 
INFILES := $(CXXFILES)
 
OBJFILES := $(CXXFILES:src/%.cpp=%)
DEPFILES := $(CXXFILES:src/%.cpp=%)
OFILES := $(OBJFILES:%=obj/%.o)

all: $(BINFILE)

.PHONY: clean all depend
.SUFFIXES:
obj/%.o: src/%.cpp
	@echo C++-compiling $<
	@if [ ! -d `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(CXX) -o $@ -c $< $(CXXFLAGS)
Makefile.dep: $(CXXFILES)
	@true Depend
	for i in $(^); do $(CXX) $(CXXFLAGS) -MM "$${i}" -MT obj/`basename $${i%.*}`.o; done > $@
 
	
$(BINFILE): $(OFILES)
	@echo Linking $@
	$(CXX) -o $@ $(OFILES) $(LDFLAGS)
clean:
	@echo Removing files
	rm -f $(BINFILE) obj/*
