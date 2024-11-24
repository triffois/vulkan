LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
INCLUDE_PATH = $(PWD)/include
#CFLAGS = -std=c++17 -I$(INCLUDE_PATH) -O0 -g #Debug
CFLAGS = -std=c++17 -I$(INCLUDE_PATH) -DNDEBUG -O3 #Release
SRC=$(PWD)/src/*

VulkanTest: $(SRC)
	g++ $(CFLAGS) -o VulkanTest ${SRC} $(LDFLAGS)

.PHONY: test clean

test: clean VulkanTest
	echo "Running engine!"
	./VulkanTest

run:
	./VulkanTest

all: clean VulkanTest
	echo "Only building stuff"

clean:
	rm -f VulkanTest