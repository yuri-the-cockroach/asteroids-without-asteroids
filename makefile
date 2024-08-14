##
# Artillery Game
#
# @file
# @version 0.1

export PLATFORM=PLATFORM_DESKTOP
export GLFW_LINUX_ENABLE_X11=FALSE
export GLFW_LINUX_ENABLE_WAYLAND=TRUE
run: build
	./a-build

build:
	 gcc -Wall -g -lraylib -lGL -lm -lpthread -ldl -lrt -lglfw -o a-build main.c

runner:
	gcc

# end
