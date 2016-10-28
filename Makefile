#
#    Makefile for marunage
#

TARGET	= marunage
PREFIX	:= /usr/local
BINDIR	:= $(PREFIX)/bin
MAKE	:= make
CC	:= cc
RM	:= rm
CFLAGS	:= -O2 -g -Wall
LDFLAGS	:=
CMDLINE	:= 0
export

all clean install-bin:
	@$(MAKE) -C ./src	$@

install: install-bin

.PHONY:	all			\
	install			\
	install-bin		\
	clean
