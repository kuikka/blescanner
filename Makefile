SRC_CXX := blescanner.cpp HciDev.cpp Socket.cpp \
		HciRequest.cpp MainLoop.cpp HciSocket.cpp \
		BLEAddress.cpp BLEDevice.cpp L2CAPSocket.cpp \
		BLEGATT.cpp UUID.cpp ByteBuffer.cpp utils.cpp

OBJ = $(SRC_CXX:.cpp=.o)
CXXFLAGS=-W -Wall -Weffc++ -g -std=c++14

all: blescanner

blescanner: $(OBJ)
	$(CXX) $(CFLAGS) $(CXXFLAGS) -o $@ $^

clean:
	rm -f blescanner $(OBJ) $(SRC_CXX:.cpp=.d)

%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MM -MT $(patsubst %.cpp,%.o,$<) $< -MF $@

-include $(SRC_CXX:.cpp=.d)
