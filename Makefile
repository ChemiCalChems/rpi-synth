#
# Makefile
#

OBJS	= main.o kernel.o synthesizer.o midi.o sequencer.o midiinput.o mixer.o

LIBS	= $(CIRCLEHOME)/lib/usb/libusb.a \
	  $(CIRCLEHOME)/lib/input/libinput.a \
	  $(CIRCLEHOME)/lib/fs/libfs.a \
	  $(CIRCLEHOME)/lib/libcircle.a

include Rules.mk

-include $(DEPS)
