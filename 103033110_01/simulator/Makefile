GTEST_DIR = googletest
SRC_DIR = src
TEST_DIR = tests

# Set Google Test's header directory as a system directory, such that
# the compiler doesn't generate warnings in Google Test headers.
CPPFLAGS += -isystem $(GTEST_DIR)/include -I$(SRC_DIR) -MD -MP

CXXFLAGS += -march=native -flto -fno-strict-aliasing -O3 -std=c++11 -g -Wall -Wextra -Wno-unused-variable -pthread

PERFXX = -static -march=native -flto -fno-strict-aliasing -O3 -std=c++11 -Wall -Wextra -Wno-unused-variable

# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

test_sources = $(wildcard $(TEST_DIR)/*.cc)

# Builds gtest.a and gtest_main.a.

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

default: single_cycle

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

single_cycle: main.cc $(wildcard src/*.h)
	$(CXX) $(PERFXX) $< -o $@

test.out : gtest_main.a $(sources:.cc=.o) $(test_sources:.cc=.o)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $@

.PHONY: test
test: test.out
	./test.out

.PHONY: clean
clean :
	rm -f $(TESTS) $(wildcard *.a) $(wildcard *.o) $(wildcard *.d) \
		$(wildcard $(SRC_DIR)/*.d) \
		$(wildcard $(TEST_DIR)/*.d) $(wildcard $(TEST_DIR)/*.o) \
		single_cycle test.out

-include main.d $(test_sources:.cc=.d)
