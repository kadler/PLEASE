OS:=$(shell uname)


ifeq ($(OS), Linux)
all:
	make -C linux
endif

ifeq ($(OS), OS400)
all:
	make -C pase
endif
