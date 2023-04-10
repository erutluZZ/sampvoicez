OUTFILE = "sampvoice.so"
BASS_PATH = $(shell pwd)

<<<<<<< HEAD
COMMON_FLAGS = -m32 -O3 -Ofast -w -fpermissive
COMPILE_FLAGS = $(COMMON_FLAGS) -c -idirafter "include"
PRELINK_FLAGS = $(COMMON_FLAGS) -shared -static-libstdc++
=======
# base compile flags
COMPILE_FLAGS=-m32 -fpack-struct=1 -fpermissive -c -O3 -w -idirafter "sdk"

# base link flags
LINK_FLAGS = -m32 -fpack-struct=1 -fpermissive -shared -O3 -static-libstdc++

# bass link flags
LINK_FLAGS += -L$(BASS_PATH) -lbass -lm -Wl,-rpath,$(BASS_PATH)
>>>>>>> parent of 40cfcff (upgrade to 3.0)

all:
	gcc $(COMPILE_FLAGS) sdk/pawn/amx/*.h
	g++ $(COMPILE_FLAGS) -std=c++11 sdk/pawn/*.cpp
	g++ $(COMPILE_FLAGS) -std=c++11 sdk/raknet/*.cpp
	g++ $(COMPILE_FLAGS) -std=c++14 *.cpp
	g++ $(LINK_FLAGS) -o $(OUTFILE) *.o
	rm *.o
	strip -s $(OUTFILE)
