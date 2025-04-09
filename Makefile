CXX = g++
CXXFLAGS = -std=c++17 -Wall \
	-I./src \
	-I./src/OrbitalBody \
	-I./src/Vessel \
	-I./src/Vector3 \
	-I./src/ThrustModel

SRC := $(wildcard src/*.cpp src/OrbitalBody/*.cpp src/Vessel/*.cpp src/Vector3/*.cpp src/ThrustModel/*.cpp)
TARGET = PhysicsSim

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
