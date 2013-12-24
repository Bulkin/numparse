CXXFLAGS = --std=c++11 -g
PROG = numparse
OBJS = main.o dumb_lexer.o operations.o expgen.o

all: $(PROG)

clean:
	rm -f $(OBJS)
	rm -f $(PROG)

$(OBJS): %.o:%.cpp
	$(CXX) $(CXXFLAGS) -c $<	

$(PROG): $(OBJS)
	$(CXX) -o $(PROG) $(CXXFLAGS) $(OBJS)
