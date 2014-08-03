.PHONY: srl examples test

all: examples

srl:
	@echo ==== BUILDING SRL SHARED LIBRARY ====
	@cd src && make

examples: srl
	@echo ==== BUILDING EXAMPLES ====
	@cd examples && make
        
tests: srl
	@echo ==== BUILDING TESTS ====
	@cd test && make


clean: FORCE
	@cd src && make clean
	@cd examples && make clean
	@cd test && make clean

install: srl
	@echo "==== INSTALLING SRL ===="
	@echo "\tinstalling include directory to /usr/local/include/srl"
	@rsync -r --force --delete --exclude ".svn" include/srl /usr/local/include/
	@echo "\tinstalling SRL libraries to /usr/local/lib"
	@cp -v lib/libsrl* /usr/local/lib/
	@env-update
FORCE:

