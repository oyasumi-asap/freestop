TARGET		=	freestop

PRX_EXPORTS	=	exports.exp
USE_KERNEL_LIBC	=	1
USE_KERNEL_LIBS	=	1

OBJS		=	exports.o 	\
			syslibc.o 	 \
			common.o	 \
			main.o		\
			stub.o		\
			thread.o \
			conf.o

INCDIR		=	
CFLAGS = -O3 -G0 -Wall -fshort-wchar
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS		=	$(CFLAGS)

LIBDIR		= 
LDFLAGS		=	-mno-crt0 -nostartfiles
LIBS		=	-lmenuj -lpspsystemctrl_kernel -lpsppower


PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak

