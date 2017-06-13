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
## ----------------------------------------------------------
CC=gcc52
CFLAGS= -lsem182 -DDEBUG -Wall -pedantic -Werror -Wextra -Wstrict-prototypes -fno-common -g -O3 -std=gnu11 
CFLAGScleaner= -lsem182 

CP=cp
CD=cd
MV=mv
GREP=grep

OBJECTS_EMPF=myempfaenger.c grp_0-shared.c grp_0-shared.h 
OBJECTS_SEND=mysender.c grp_0-shared.c grp_0-shared.h 
OBJECTS_CLEANER=cleaner.c grp_0-shared.c grp_0-shared.h

EXCLUDE_PATTERN=footrulewidth

all: sender empfaenger cleaner

sender: $(OBJECTS_SEND)
		$(CC) -o $@ $^ $(CFLAGS)

empfaenger: $(OBJECTS_EMPF)
		$(CC) -o $@ $^ $(CFLAGS)

cleaner: $(OBJECTS_CLEANER)
		$(CC) -o $@ $^ $(CFLAGScleaner)
