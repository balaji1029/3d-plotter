CXX := g++
CXXFLAGS := -std=c++11 -Wall -Wextra
OPENGLLIB= -lGL
GLEWLIB= -lGLEW
GLFWLIB = -lglfw
LDFLAGS := $(OPENGLLIB) $(GLEWLIB) $(GLFWLIB)

SRCS := main.cpp gl_framework.cpp
OBJS := $(SRCS:.cpp=.o)
TARGET := 3d_plotter

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean