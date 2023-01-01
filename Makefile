#
# Makefile
#

CIRCLESTDLIBHOME = /home/chemicalchems/circle-stdlib

include $(CIRCLESTDLIBHOME)/Config.mk

CIRCLEHOME = $(CIRCLESTDLIBHOME)/libs/circle
NEWLIBDIR = $(CIRCLESTDLIBHOME)/install/$(NEWLIB_ARCH)

OBJS	= main.o kernel.o midi.o sequencer.o midiinput.o mixer.o waveform.o clock.o \
voice.o midimapper.o

OPTIMIZE = -Ofast
STANDARD = -std=c++20

include $(CIRCLEHOME)/Rules.mk

CFLAGS += -I "$(NEWLIBDIR)/include" -I $(STDDEF_INCPATH) -I ../../include
LIBS := "$(NEWLIBDIR)/lib/libm.a" "$(NEWLIBDIR)/lib/libc.a" "$(NEWLIBDIR)/lib/libcirclenewlib.a" \
	$(CIRCLEHOME)/addon/SDCard/libsdcard.a \
	$(CIRCLEHOME)/lib/usb/libusb.a \
	$(CIRCLEHOME)/lib/input/libinput.a \
	$(CIRCLEHOME)/lib/sound/libsound.a \
	$(CIRCLEHOME)/addon/fatfs/libfatfs.a \
	$(CIRCLEHOME)/lib/fs/libfs.a \
	$(CIRCLEHOME)/lib/net/libnet.a \
	$(CIRCLEHOME)/lib/sched/libsched.a \
	$(CIRCLEHOME)/lib/libcircle.a \
	$(NEWLIBDIR)/lib/libnosys.a



-include $(DEPS)
