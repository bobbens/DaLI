

USE_PTHREADS := true

NAME		:= dali

CFILES	:= test.c dali.c dali_dist.c dali_mesh.c

CFLAGS_FFTW3 := $(shell pkg-config --cflags fftw3)
LDFLAGS_FFTW3 := $(shell pkg-config --libs fftw3)
CFLAGS_ARPACK := $(shell pkg-config --cflags arpack)
LDFLAGS_ARPACK := $(shell pkg-config --libs arpack)

CFLAGS	:= -g3 -D_GNU_SOURCE=1 -W -Wall -Wextra -Wunused -Wshadow -Wpointer-arith -Wmissing-prototypes -Winline -Wcast-align -Wmissing-declarations -Wredundant-decls -Wno-long-long -Wcast-align $(CFLAGS_FFTW3) $(CFLAGS_ARPACK)
LDFLAGS	:= $(LDFLAGS_FFTW3) -lceigs $(LDFLAGS_ARPACK) -lm -lcxsparse -lumfpack -lvl #-lcblas -lf77blas -latlas

ifeq ($(USE_PTHREADS),true)
CFLAGS	+= -DHAVE_PTHREADS=1
LDFLAGS	+= -lpthread
endif

#CFLAGS	+= -DDEBUG

.PHONY: all octave docs clean

all: octave test

test: dali.h $(CFILES)
		$(CC) $(CFLAGS) $(CFILES) -o $@ $(LDFLAGS)

octave:
	(cd octave; $(MAKE))

docs:
	doxygen
	(cd latex; $(MAKE))
	cp latex/refman.pdf $(NAME).pdf

clean:
	$(RM) test
