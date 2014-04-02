SRC_CXX := blescanner.cpp HciDev.cpp Socket.cpp \
		HciRequest.cpp MainLoop.cpp HciSocket.cpp \
		BLEAddress.cpp BLEDevice.cpp L2CAPSocket.cpp \
		BLEGATT.cpp
OBJ = $(SRC_CXX:.cpp=.o)
#CXXFLAGS=-std=c++0x
CXXFLAGS=-W -g

blescanner: $(OBJ)
	$(CXX) -o $@ $^

clean:
	rm -f blescanner $(OBJ) $(SRC_CXX:.cpp=.d)

%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MM -MT $(patsubst %.cpp,%.o,$<) $< -MF $@

include $(SRC_CXX:.cpp=.d)
