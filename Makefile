CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Iinclude
TARGET = replit
SRCDIR = src
INCDIR = include
SOURCES = $(SRCDIR)/main.cpp $(SRCDIR)/lexer.cpp $(SRCDIR)/parser.cpp $(SRCDIR)/vm.cpp $(SRCDIR)/replit_engine.cpp

$(TARGET): $(SOURCES)
        $(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
        rm -f $(TARGET) temp*.rpl

test: $(TARGET)
        ./$(TARGET) examples/hello.rpl
        ./$(TARGET) examples/math.rpl

demo: $(TARGET)
        python3 demo.py

advanced-demo:
        python3 advanced_demo.py

install: $(TARGET)
        cp $(TARGET) /usr/local/bin/

.PHONY: clean test demo advanced-demo install