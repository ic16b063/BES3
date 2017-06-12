##
## @file my-empfaenger
## Betriebssysteme my-empfaenger Makefile
## Beispiel 3
##
## @author 
## @author 
## @date 2005/02/22
##
## @version $Revision: 1689 $
##
##	@versuch 1 Anpassung an myfind von Steinmetz, Posch, Stanescu
##
##
## @todo
##
## URL: $HeadURL: https://svn.petrovitsch.priv.at/ICSS-BES/trunk/2015/CIS/lu/hello/Makefile $
##
## Last Modified: $Author: tom $
##

##
## ------------------------------------------------------------- variables --
##

CC=gcc52
##CC=gcc
##CFLAGS=-DDEBUG -Wall -pedantic -Werror -Wextra -Wstrict-prototypes -fno-common -g -O3 -std=gnu11
CFLAGS= -lsem182 -DDEBUG -Wall -pedantic -Werror -Wextra -Wstrict-prototypes -fno-common -g -O3 -std=gnu11 
##CFLAGS= -lsem182


CP=cp
CD=cd
MV=mv
GREP=grep

##OBJECTS=mypopen.c mypopen.h mytest.c
##OBJECTS=mypopen.c mypopen.h 
OBJECTS_EMPF=my-empfaenger.c grp_0-shared.c grp_0-shared.h 
OBJECTS_SEND=my-sender.c grp_0-shared.c grp_0-shared.h 

EXCLUDE_PATTERN=footrulewidth

##
## ----------------------------------------------------------------- rules --
##

##%.o: %.c 
##		$(CC)  -c $< $(CFLAGS)

##
## --------------------------------------------------------------- targets --
##


all: sender empfaenger

sender: $(OBJECTS_SEND)
		$(CC) -o $@ $^ $(CFLAGS)

empfaenger: $(OBJECTS_EMPF)
		$(CC) -o $@ $^ $(CFLAGS)


#popentest: popentest
#		$(CC) $(CFLAGS) -o mypopen.o -lpopentest

		

##   $(OBJECTS)
##	$(CC) $(CFLAGS) $@ $^


## ---------------------------------------------------------- dependencies --
##

##
## =================================================================== eof ==
##