ROOTCXXFLAGS  := $(shell root-config --cflags)
CXXFLAGS      :=$(ROOTCXXFLAGS) -std=c++17 -O3 -Wall -Wextra -Wshadow \
	             -Wpedantic
ROOTLIBS      := $(shell root-config --ldflags --glibs)
LIBS          := $(ROOTLIBS)

TARGETS       := waveform2csv rootfileConverter 

.PHONY: all clean
all: $(TARGETS) 

$(TARGETS): % : %.cpp 
	@echo "  Compiling  $(notdir $@)"
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)


clean:
	rm -f $(TARGETS); \
	rm -f *.root;
