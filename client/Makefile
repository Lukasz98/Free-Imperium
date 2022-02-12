src = $(wildcard src/*.cpp) \
	  $(wildcard src/graphics/*.cpp) \
	  $(wildcard src/gui_last/*.cpp) \
	  $(wildcard ../shared/*.cpp)

obj = $(src:.cpp=.o)
dep = $(obj:.o=.d)

OUT=imperium
#COMPILER=g++
COMPILER=clang++
#COMPILER=$(CXX)
CXX=$(COMPILER)
WARNFLAGS=-std=c++17
#WARNFLAGS=-std=c++17 -O0 -g -Wunused-function -Wformat=2 -Wnull-dereference \
#		 -fdelete-null-pointer-checks -Wno-if-not-aligned -Wmisleading-indentation \
#		 -Wmissing-attributes -Wmissing-braces -Wmultistatement-macros -Wno-return-local-addr \
#		 -Wreturn-type -Wunused-parameter -Wunused -Wuninitialized -Wmaybe-uninitialized 
#-Wshadow
INCLUDES=-I/usr/include/ -I/usr/include/freetype2/ -I../shared/ -Isrc/

LDFLAGS=-L/usr/lib/
LDLIBS=-lGL -lGLU -lglfw -lGLEW -lpng16 -lstdc++fs -lfreetype -lsfml-system \
		-lsfml-network

#-MMD generetes .d files
CXXFLAGS=-MMD $(WARNFLAGS) $(INCLUDES)

CPPCHECK_OUT_DIR = cppcheck_html
CPPCHECK_OUT = report.xml
#CLANG_OUT_DIR = clang_html





$(OUT): $(obj)
	$(CXX)  -o  $@ $^ $(LDFLAGS) $(LDLIBS)

-include $(dep)


sca: $(CPPCHECK_OUT_DIR)
scaclang: $(CLANG_OUT_DIR)

$(CPPCHECK_OUT_DIR): $(CPPCHECK_OUT)
	cppcheck-htmlreport --file=$< --report-dir=$@ --source-dir=.

$(CPPCHECK_OUT):
	cppcheck --xml --xml-version=2 --inconclusive --enable=all -v $(src) 2> $@

#$(CLANG_OUT_DIR): CXX = scan-build -plist-html -o $(CLANG_OUT_DIR) $(COMPILER)
#$(CLANG_OUT_DIR): cl $(OUT) 

.PHONY: clean
clean:
	rm -f $(obj) imperium CPPCHECK_OUT CPPCHECK_OUT_DIR

.PHONY: cleandep
cleandep:
	rm -f $(dep)


