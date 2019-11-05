# Paths

LPATH_QRNG = ../libQRNG/

# Flags

CC = g++

WARN = -Wall -Wno-format
COPTS = -O $(DEBUG) $(ARCH)

CFLAGS += $(WARN) $(COPTS) -I$(LPATH_QRNG)

# Rules

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

# Variables

BINS = qrngdownload
SRCS = qrngdownload.c
OBJS = $(SRCS:%.c=%.o)

# Main target

all: ARCH = -m32
all: $(BINS)

debug: DEBUG += -g
debug: TARGET = debug
debug: $(BINS)

amd64: ARCH = -m64
amd64: TARGET = amd64
amd64: $(BINS)

amd64-debug: DEBUG += -g
amd64-debug: ARCH = -m64
amd64-debug: TARGET = amd64-debug
amd64-debug: $(BINS)

# Dependencies
libQRNG.so:
	$(MAKE) -s -C $(LPATH_QRNG) $(TARGET)
	cp $(LPATH_QRNG)$@ $@

qrngdownload: libQRNG.so $(OBJS)
	$(CC) $(ARCH) -lssl -lcrypto -lpthread -o $@ ./libQRNG.so $(OBJS)

# Misc

release:
release:
	-rm libQRNG.so libQRNG.h
	$(MAKE) clean -C $(LPATH_QRNG)
	$(MAKE) clean
	$(MAKE) -C $(LPATH_QRNG)
	cp $(LPATH_QRNG)libQRNG.so $(LPATH_QRNG)libQRNG.h .
	$(MAKE)
	tar cfz qrngdownload-Linux-32Bit.tar.gz ../qrngdownload/qrngdownload ../qrngdownload/qrngdownload.c ../qrngdownload/Makefile ../qrngdownload/libQRNG.so ../qrngdownload/libQRNG.h
	-rm libQRNG.so libQRNG.h
	$(MAKE) clean -C $(LPATH_QRNG)
	$(MAKE) clean
	$(MAKE) amd64 -C $(LPATH_QRNG)
	cp $(LPATH_QRNG)libQRNG.so $(LPATH_QRNG)libQRNG.h .
	$(MAKE) amd64
	tar cfz qrngdownload-Linux-64Bit.tar.gz ../qrngdownload/qrngdownload ../qrngdownload/qrngdownload.c ../qrngdownload/Makefile ../qrngdownload/libQRNG.so ../qrngdownload/libQRNG.h

clean:
	rm -f *.o *~ ~* *.bck core
	rm -f $(BINS)
