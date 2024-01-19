# Check for valgrind
export VALGRIND :=
ifneq "$(shell command -v valgrind 2> /dev/null)" ""
  export VALGRIND := valgrind -q --leak-check=full --error-exitcode=1
endif

agnews: 
	@echo "Training with Ag News"
	g++ --std=c++17 -g -o test main.cpp -lz
	$(VALGRIND) ./test
	@echo

clean:
	rm -f test
	rm -f *.o
	rm -rf -f tests/*.dSYM *.dSYM
