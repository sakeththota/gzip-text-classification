LIB_DIR := .
CXX := g++
CXXFLAGS := -g -lz --std=c++17 -pedantic

# Check for valgrind
export VALGRIND :=
ifneq "$(shell command -v valgrind 2> /dev/null)" ""
  export VALGRIND := valgrind -q --leak-check=full --error-exitcode=1
endif

agnews: 
	@echo "Training with Ag News"
	$(CXX) $(CXXFLAGS) -I$(LIB_DIR) -o main main.cpp
	$(VALGRIND) ./main
	@echo

clean:
	rm -f *.o
	rm -rf -f tests/*.dSYM *.dSYM