include /usr/local/etc/PcapPlusPlus.mk

CXX = g++

HEADERS = $(shell find -name "*.h")
SRCS = $(shell find -name "*.cpp")
OBJS = $(SRCS:.cpp=.o)

DEBUG_BUILD_FLAGS = -O0 -g -Wall -std=c++11

# %.o: %.cpp $(HEADERS)
# 	$(CXX) -c -o $@ $< $(PCAPPP_BUILD_FLAGS) $(PCAPPP_INCLUDES)
%.o: %.cpp $(HEADERS)
	$(CXX) -c -o $@ $< $(DEBUG_BUILD_FLAGS) $(PCAPPP_INCLUDES)

main: $(OBJS)
	$(CXX) -o $@ $^ $(PCAPPP_LIBS_DIR) -static-libstdc++ $(PCAPPP_LIBS)
	
clean:
	rm -rf $(OBJS) main
