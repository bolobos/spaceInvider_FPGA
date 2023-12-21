# Makefile for info mode user
# Date : 29/09/2023
# Author : Pascal Aygalinc <Pascal.Aygalinc@univ-smb.fr>
# modify 10/10/23: yves.marboutin@univ-smb.fr

ARCH = arm 
CROSS_COMPILE = arm-none-linux-gnueabihf-
CC = $(CROSS_COMPILE)gcc
INSTALL = install
CFLAGS =  -g -Wall
LDFLAGS = -g -Wall


PROGS     = prjOled

ifeq ($(prefix),)
    prefix	 = /home/geii/DE1-Soc/rootfs
endif

INSTDIR   = $(prefix)/home/root/
INSTMODE  = 0775
INSTOWNER = geii
INSTGROUP = geii

# SRCR : list of C source code for compiler 
SRCS	= prjOled.c libraryPrj.c readPrgm.c
# OBJS : list of objet code for linker
OBJS	= prjOled.o libraryPrj.o readPrgm.o

all:	$(PROGS)

$(PROGS): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(PROGS) *.o *~ core

install: $(PROGS)
	$(INSTALL) $(PROGS) -m $(INSTMODE) -t $(INSTDIR) 
#	$(INSTALL) -m $(INSTMODE) #-o $(INSTOWNER) -g $(INSTGROUP) $(PROGS) $(INSTDIR)

uninstall:
	rm -f $(INSTDIR)$(PROGS)
 
depend:
	makedepend -Y -- $(CFLAGS) -- $(SRCS) 2>/dev/null

# The following is used to automatically generate dependencies.
# DO NOT DELETE
prjOled.o: prjOled.h

libraryPrj.o: prjOled.h

readPrgm.o: prjOled.h

