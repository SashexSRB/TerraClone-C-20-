CXX = g++
CFLAGS = -std=c++20 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

# Automatically grab all .cpp files in current directory
SOURCES = $(wildcard *.cpp)
# Replace .cpp with .o for object files
OBJECTS = $(SOURCES:.cpp=.o)

TARGET = VulkanTest

# Final executable
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile each source into object
%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

test:
	./VulkanTest

clean:
	rm -f $(OBJECTS) $(TARGET)

