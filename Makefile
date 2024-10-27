LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
INCLUDE_PATH = $(PWD)/include
CFLAGS = -std=c++17 -I$(INCLUDE_PATH) -O2 -DNDEBUG
SRC=$(PWD)/src/*

VulkanTest: $(SRC)
	g++ $(CFLAGS) -o VulkanTest ${SRC} $(LDFLAGS)

.PHONY: test clean

test: clean VulkanTest
	./VulkanTest

clean:
	rm -f VulkanTest