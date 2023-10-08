ALL_BIN=test

all: $(ALL_BIN)

test: test.cpp
	$(CXX) -g $^ -I ../include -L ../lib -lpthread -ljson -o $@

clean:
	rm $(ALL_BIN)
