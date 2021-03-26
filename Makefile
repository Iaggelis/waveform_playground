ROOTCXXFLAGS   := $(shell root-config --cflags)
CXXFLAGS       :=$(ROOTCXXFLAGS) -std=c++17 -O3 -Wall -Wextra -Wshadow \
	              -Wpedantic
CXXFLAGS_debug :=$(ROOTCXXFLAGS) -std=c++17 -O0 -ggdb -Wall -Wextra -Wshadow \
	              -Wpedantic
ROOTLIBS       := $(shell root-config --ldflags --glibs)
LIBS           := $(ROOTLIBS)

TARGETS        := waveform2csv rootfileConverter 

.PHONY: all cl ean
all: $(TARGETS) 

$(TARGETS): % : %.cpp 
	@echo "  Compiling  $(notdir $@)"
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)


clean:
	rm -f $(TARGETS); \
	rm -f *.root;
