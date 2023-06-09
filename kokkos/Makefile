KOKKOS_PATH    ?= ${HOME}/Kokkos/kokkos
KOKKOS_DEVICES ?= "Cuda,OpenMP"
EXE_NAME = "gauss-seidel"

SRC = $(wildcard *.cpp)

default: build
	echo "Start Build"


ifneq (,$(findstring Cuda,$(KOKKOS_DEVICES)))
CXX = ${KOKKOS_PATH}/bin/nvcc_wrapper
EXE = ${EXE_NAME}.cuda
KOKKOS_ARCH = "Pascal60"
KOKKOS_CUDA_OPTIONS = "enable_lambda"
else
CXX = g++
EXE = ${EXE_NAME}.host
KOKKOS_ARCH = "SKX"
endif

CXXFLAGS  = -O3
CXXFLAGS += $(shell pkg-config --cflags opencv4)

LINK      = ${CXX}
LINKFLAGS = $(shell pkg-config --libs opencv4)

DEPFLAGS = -M

OBJ = $(SRC:.cpp=.o)
LIB = 

include $(KOKKOS_PATH)/Makefile.kokkos

build: $(EXE)

$(EXE): $(OBJ) $(KOKKOS_LINK_DEPENDS)
	$(LINK) $(KOKKOS_LDFLAGS) $(LINKFLAGS) $(EXTRA_PATH) $(OBJ) $(KOKKOS_LIBS) $(LIB) -o $(EXE)

clean: kokkos-clean
	rm -f *.o *.cuda *.host

# Compilation rules

%.o:%.cpp $(KOKKOS_CPP_DEPENDS)
	$(CXX) $(KOKKOS_CPPFLAGS) $(KOKKOS_CXXFLAGS) $(CXXFLAGS) $(EXTRA_INC) -c $<

run: $(EXE)
	srun -N 1 --reservation=CHPS -p short --gres=gpu:1 ./gauss-seidel.cuda ../img/flowers.jpg