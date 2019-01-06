.PHONY : build
.PHONY : build_all
.PHONY : clean

LIB_NAME=spotj

# specific flags for spotj library
DARWIN_XCFLAGS=-fPIC
LINUX_X86_XCFLAGS=-fPIC
LINUX_X86_64_XCFLAGS=-fPIC
WINDOWS_X86_XCFLAGS=-fPIC
WINDOWS_X86_64_XCFLAGS=-fPIC

ifneq ($(wildcard ../setosspecific),)
include ../setosspecific
else
ifneq ($(wildcard ../../setosspecific),)
include ../../setosspecific
endif
endif

OBJECTS := $(patsubst %.cc,%.o,$(wildcard *.cc))

HEADERS := $(wildcard *.h)

.cc.o: $(OBJECTS) $(HEADERS)
	$(CPP) -c  $< $(XCFLAGS) -lspot -lbddx

LIB_THERE:=$(CLASSDIR)/$(OSDIRPART)/$(DYN_PREFIX)$(LIB_NAME).$(DYN_EXT)
ifeq ("$(wildcard $(LIB_THERE))","")
build: $(OBJECTS) $(HEADERS)
	$(CPP) $(XCFLAGS) -shared *.o -o $(DYN_PREFIX)$(LIB_NAME).$(DYN_EXT) -lspot -lbddx 
	$(STRIP) -x $(DYN_PREFIX)$(LIB_NAME).$(DYN_EXT)
	mkdir -p $(CLASSDIR)/$(OSDIRPART)
	cp $(DYN_PREFIX)$(LIB_NAME).$(DYN_EXT) $(CLASSDIR)/$(OSDIRPART)
	rm $(DYN_PREFIX)$(LIB_NAME).$(DYN_EXT)
endif

build_all:
	make -f Makefile clean
	make -f Makefile build FOR_OS=Darwin_x86_64 CLASSDIR=$(CLASSDIR)
	make -f Makefile clean
	make -f Makefile build FOR_OS=Linux_i686 CLASSDIR=$(CLASSDIR)
	make -f Makefile clean
	make -f Makefile build FOR_OS=Linux_x86_64 CLASSDIR=$(CLASSDIR)
	make -f Makefile clean
	make -f Makefile build FOR_OS=Windows_i686 CLASSDIR=$(CLASSDIR)
	make -f Makefile clean
	make -f Makefile build FOR_OS=Windows_x86_64 CLASSDIR=$(CLASSDIR)

clean:
	rm -f *.o
	rm -f $(DYN_PREFIX)$(LIB_NAME).$(DYN_EXT)

distclean:
	rm -f *.o
	rm -f $(DYN_PREFIX)$(LIB_NAME).$(DYN_EXT)
