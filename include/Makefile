# 
#   Compile all the include directories
#

SUBDIRS = sockets server client 
all:
	@set -e; for i in $(SUBDIRS); do (cd $$i; make) || exit 1; done

clean_subdirs:
	@set -e; for i in $(SUBDIRS); do (cd $$i; make clean) || exit 1; done

clean: clean_subdirs

