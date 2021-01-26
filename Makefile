CXXFLAGS      := -std=c++17 -ggdb -O0 -Wall -Wextra -Wshadow \
	             -Wpedantic
LIBS          := -lfftw3
ROOTCXXFLAGS  := $(shell root-config --cflags)
ROOTLIBS      := $(shell root-config --ldflags --glibs)


peak_finding: peak_finding.cpp
	$(CXX) $(CXXFLAGS) $(ROOTCXXFLAGS) $< -o $@ $(LIBS) $(ROOTLIBS)


.PHONY: clean
clean :
	rm -f peak_finding
