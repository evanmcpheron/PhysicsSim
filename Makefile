CXX = g++
CXXFLAGS = -std=c++17 -Wall \
	-I./src \
	-I./src/OrbitalBody \
	-I./src/Vessel

SRC := $(wildcard src/*.cpp src/OrbitalBody/*.cpp src/Vessel/*.cpp)
TARGET = PhysicsSim

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
