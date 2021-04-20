TARGET = memorystickutility
OBJS = crt0.o main.o

INCDIR = include
CFLAGS = -Os -G0
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LIBDIR = lib
LDFLAGS += -nostdlib
LIBS = -lpsprtc -lvlfgui -lvlfgu -lvlfutils -lvlflibc -lpspusb -lpspusbstor -lpspusbdevice

PSP_EBOOT_TITLE = Memory Stick Utility
PSP_EBOOT_ICON = data/images/ICON0.PNG
PSP_EBOOT_PIC1 = data/images/PIC1.PNG
EXTRA_TARGETS = EBOOT.PBP

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
