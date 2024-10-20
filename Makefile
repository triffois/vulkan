LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
STB_INCLUDE_PATH = $(PWD)
CFLAGS = -std=c++17 -I$(STB_INCLUDE_PATH) -O2 -DNDEBUG

VulkanTest: main.cpp
	g++ $(CFLAGS) -o VulkanTest main.cpp $(LDFLAGS)

.PHONY: test clean

test: VulkanTest
	./VulkanTest

clean:
	rm -f VulkanTest