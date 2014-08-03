SOURCES=test.cpp TypeTest.cpp StringTest.cpp SystemTest.cpp ThreadTest.cpp 
SOURCES+=fs/FsTests.cpp util/DictTest.cpp util/VectorTest.cpp net/SocketTest.cpp
SOURCES+=net/HttpTests.cpp XmlTest.cpp JsonTest.cpp

CC=g++
CFLAGS=-Wall -I../include

EXECUTABLE=run_tests
LIB_DIR=$(shell pwd)
OBJECTS=$(SOURCES:.cpp=.o)
LDFLAGS= -L../lib -lsrl

all: $(SOURCES) $(EXECUTABLE)
	@echo "\ttest binary has been build"
	@echo "\texecuting test binary"
#./$(EXECUTABLE)


$(EXECUTABLE): $(OBJECTS)
	@echo ====  creating $@  ====
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	@echo copying $@ to srl/bin/
	@mkdir -p ../bin
	@cp $@ ../bin/
	@echo $@ has been built

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@
	@echo

clean:
	@echo "removing all object files..."
	@find . -name "*.o" -delete;
	@echo "removing executable files..."
	@rm $(EXECUTABLE)
	@rm ../bin/$(EXECUTABLE)
	@echo done