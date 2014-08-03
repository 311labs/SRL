
CC=g++
CFLAGS=-Wall -I../include

SOURCES= filetool.cpp nettool.cpp proclist.cpp random.cpp sysmonitor.cpp logging.cpp

EXECUTABLES=$(basename $(SOURCES))
LDFLAGS= -L../lib -lsrl

all: $(EXECUTABLES)
	@echo "\tExamples have been built"
	@echo "\texample binaries can be found in the examples directory or in the bin directory"

$(EXECUTABLES):
	@echo "||||  building $@  ||||"
	$(CC) $(CFLAGS) $(LDFLAGS) $@.cpp -o $@
	@mkdir -p ../bin/
	@-cp $@ ../bin/
	@echo 

clean:
	@echo "deleting example object files..."
	@find . -name "*.o" -delete;
	@echo "deleting example binaries..."
	@rm $(EXECUTABLES)


