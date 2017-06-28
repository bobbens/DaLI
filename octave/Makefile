
#CFLAGS	:= -g -O3 -DHAVE_PTHREADS=1 -DDEBUG
#CFLAGS	:= -g -O3 -DHAVE_PTHREADS=1
CFLAGS	:= -g -DHAVE_PTHREADS=0
LDFLAGS	:= -lm -lceigs -larpack -lcxsparse -lumfpack -lfftw3 -lvl

.PHONY: all octave matlab clean

all: octave

octave: dali_compute.mex dali_distance.mex

matlab: dali_compute.mexa64 dali_distance.mexa64

dali_compute.mexa64: ../dali.h ../dali.c ../dali_mesh.c dali_compute.c
	CFLAGS="$(CFLAGS)" mex -g -I.. $(LDFLAGS) dali_compute.c ../dali.c ../dali_mesh.c

dali_distance.mexa64: ../dali.h ../dali.c ../dali_dist.c ../dali_mesh.c dali_distance.c
	CFLAGS="$(CFLAGS)" mex -g -I.. $(LDFLAGS) dali_distance.c ../dali.c ../dali_dist.c ../dali_mesh.c

dali_compute.mex: ../dali.h ../dali.c ../dali_mesh.c dali_compute.c
	CFLAGS="$(CFLAGS)" mkoctfile --mex -g -I.. $(LDFLAGS) dali_compute.c ../dali.c ../dali_mesh.c

dali_distance.mex: ../dali.h ../dali.c ../dali_dist.c ../dali_mesh.c dali_distance.c
	CFLAGS="$(CFLAGS)" mkoctfile --mex -g -I.. $(LDFLAGS) dali_distance.c ../dali.c ../dali_dist.c ../dali_mesh.c

clean:
	$(RM) dali_distance.o dali_compute.o dali_sift.o dali_distance.mex dali_compute.mex dali_sift.mex dali_mesh.mex
