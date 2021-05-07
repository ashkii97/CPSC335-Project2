

#
CC := g++
CFLAGS := -std=c++17 -g


#
default: all


#
menu:
	@echo
	@echo "*** Project 2 Main Menu ***"
	@echo
	@echo "make menu            ==> This menu"
	@echo
	@echo "make all             ==> Run all targets"
	@echo "make test            ==> Run tests"
	@echo
	@echo "make maxarmor_test   ==> Build the maxarmor test"
	@echo "make maxarmor        ==> Build maxarmor"
	@echo


#
all: maxdefense test

test: maxdefense_test
	./maxdefense_test

maxdefense_test: maxdefense.hh rubrictest.hh maxdefense_test.cc
	$(CC) $(CFLAGS) maxdefense_test.cc -o $@

maxdefense: maxdefense.hh timer.hh maxdefense_main.cc
	$(CC) $(CFLAGS) maxdefense_main.cc -o experiment

clean:
	-rm -f experiment maxdefense maxdefense_test
