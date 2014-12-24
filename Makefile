CXX ?= clang++
CXXFLAGS = \
	-fno-rtti \
	-fno-exceptions \
	-Wall \
	-g3

BIN = test

SOURCES = \
	ast.cpp \
	lexer.cpp \
	parser.cpp \
	main.cpp

OBJECTS = $(SOURCES:.cpp=.o)

all: $(BIN)

$(BIN): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@

.cpp.o:
	$(CXX) -MD -c $(CXXFLAGS) $< -o $@
	@cp $*.d $*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
			-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
		rm -f $*.d

clean:
	rm -f $(OBJECTS) $(OBJECTS:.o=.P)
	rm -f $(BIN)

-include *.P
