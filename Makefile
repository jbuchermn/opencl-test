CXX = clang
OPENCLC=/System/Library/Frameworks/OpenCL.framework/Libraries/openclc
ROOT = /Users/jonasbucher/Desktop/Daten/OpenCL/imgfilter

.SUFFIXES:
KERNEL_ARCH=i386 x86_64 gpu_32 gpu_64

KERNELS = kernel.cl
OBJECTS = filters.o filters_cl.o imageio.o main.o $(KERNELS:.cl=.cl.o)
LIBRARIES = -L$(ROOT)/libjpeg -ljpeg
FRAMEWORKS = -framework OpenCL

BITCODES=$(patsubst %, kernel.cl.%.bc, $(KERNEL_ARCH))


default: main

kernel.cl.o: kernel.cl.c
	$(CXX) -Os -Wall -arch x86_64 -o $@ -c $<

kernel.cl.c kernel.cl.h: kernel.cl
	$(OPENCLC) -x cl -cl-std=CL1.1 -cl-auto-vectorize-enable -emit-gcl $<

kernel.cl.%.bc: kernel.cl
	$(OPENCLC) -x cl -cl-std=CL1.1 -Os -arch $* -emit-llvm -o $@ -c $<


%.o: %.c $(KERNELS:.cl=.cl.h)
	$(CXX) $(LIBRARIES) -Os -Wall -arch x86_64 -o $@ -c $<

main: $(OBJECTS) $(BITCODES)
	$(CXX) $(LIBRARIES) $(FRAMEWORKS) $(OBJECTS) -o $@

.PHONY: clean
clean:
	rm -f *.gch
	rm -f $(KERNELS:.cl=cl.c) $(KERNELS:.cl=.cl.h)
	rm -f $(OBJECTS)
	rm -f main
