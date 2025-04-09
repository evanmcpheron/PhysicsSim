CXX = g++
CXXFLAGS = -std=c++17 -Wall \
	-I./src \
	-I./src/OrbitalBody \
	-I./src/Vessel \
	-I./src/Vector3 \
	-I./src/ThrustModel \
	-I./src/Atmosphere \
	-I./src/HeatShield \
	-I./src/Parachute 

SRC := $(wildcard src/*.cpp src/OrbitalBody/*.cpp src/Vessel/*.cpp src/Vector3/*.cpp src/ThrustModel/*.cpp src/Atmosphere/*.cpp src/HeatShield/*.cpp src/Parachute/*.cpp)
TARGET = PhysicsSim

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
