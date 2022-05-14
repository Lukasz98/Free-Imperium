src = $(wildcard src/*.cpp) \
	  $(wildcard src/graphics/*.cpp) \
	  $(wildcard src/gui_last/*.cpp) \
	  $(wildcard ../shared/*.cpp)

obj = $(src:.cpp=.o)
dep = $(obj:.o=.d)

OUT=imperium
COMPILER=g++
CXX=$(COMPILER)
WARNFLAGS=-std=c++17 -g
INCLUDES=-I/usr/include/ -I/usr/include/freetype2/ -I../shared/ -Isrc/

LDFLAGS=-L/usr/lib/
LDLIBS=-lGL -lglfw -lGLEW -lpng16 -lstdc++fs -lfreetype -lsfml-system \
		-lsfml-network

CXXFLAGS=-MMD $(WARNFLAGS) $(INCLUDES)

CPPCHECK_OUT_DIR = cppcheck_html
CPPCHECK_OUT = report.xml





$(OUT): $(obj)
	$(CXX)  -o  $@ $^ $(LDFLAGS) $(LDLIBS)

-include $(dep)


sca: $(CPPCHECK_OUT_DIR)
scaclang: $(CLANG_OUT_DIR)

$(CPPCHECK_OUT_DIR): $(CPPCHECK_OUT)
	cppcheck-htmlreport --file=$< --report-dir=$@ --source-dir=.

$(CPPCHECK_OUT):
	cppcheck --xml --xml-version=2 --inconclusive --enable=all -v $(src) 2> $@


.PHONY: clean
clean:
	rm -f $(obj) imperium CPPCHECK_OUT CPPCHECK_OUT_DIR

.PHONY: cleandep
cleandep:
	rm -f $(dep)


