CXX = g++                   # The C++ compiler to use
CXXFLAGS = -std=c++11 -Wall # Compiler flags: use C++11 standard and show all warnings

TARGET = sim                # The name of the final executable
SOURCES = sim.cpp memoryManager.cpp requestGen.cpp  # Source files to compile
OBJECTS = $(SOURCES:.cpp=.o)                        # Object files generated from source files

all: $(TARGET)              # Default target: build the executable

$(TARGET): $(OBJECTS)       # Link object files to create the executable
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp                  # Compile each .cpp file to a .o file
	$(CXX) $(CXXFLAGS) -c $<

clean:                      # Remove object files and the executable
	rm -f $(OBJECTS) $(TARGET)