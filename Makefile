#
#    Makefile for marunage
#

TARGET	= marunage
PREFIX	:= /usr/local
BINDIR	:= $(PREFIX)/bin
PSRDIR	:= $(PREFIX)/share/marunage/parser
LOGDIR	:= /var/log/marunage
MAKE	:= make
CC	:= cc
RM	:= rm
CFLAGS	:= -O2 -g -Wall
LDFLAGS	:=
CMDLINE	:= 0
export

all clean install-bin:
	@$(MAKE) -C ./src	$@

install-parser:
	@$(MAKE) -C ./parser	$@

make-logdir:
	install -pd $(LOGDIR)
	touch $(LOGDIR)/marunage.log

install: install-bin install-parser make-logdir

.PHONY:	all			\
	install			\
	install-bin		\
	install-parser		\
	make-logdir		\
	clean
