BUILD_DIR ?= build

CPPFLAGS ?=
CFLAGS ?= -std=c99 -Wall -Wextra -O2
LDFLAGS ?=
ARFLAGS ?= rcs
XINPUT_LIB ?= -lxinput9_1_0

CC ?= gcc
AR ?= ar

CC_X86 ?= i686-w64-mingw32-gcc
AR_X86 ?= i686-w64-mingw32-ar

CC_X64 ?= x86_64-w64-mingw32-gcc
AR_X64 ?= x86_64-w64-mingw32-ar

.PHONY: all native x86 x64 both dll dll-native dll-x86 dll-x64 clean

all: native

both: x86 x64

native: $(BUILD_DIR)/native/libxinput_framework.a

x86: $(BUILD_DIR)/x86/libxinput_framework.a

x64: $(BUILD_DIR)/x64/libxinput_framework.a

dll: dll-native

dll-native: $(BUILD_DIR)/native/xinput_framework.dll

dll-x86: $(BUILD_DIR)/x86/xinput_framework.dll

dll-x64: $(BUILD_DIR)/x64/xinput_framework.dll

$(BUILD_DIR)/native:
	mkdir -p $@

$(BUILD_DIR)/x86:
	mkdir -p $@

$(BUILD_DIR)/x64:
	mkdir -p $@

$(BUILD_DIR)/native/xinput.o: xinput.c xinput.h | $(BUILD_DIR)/native
	$(CC) $(CPPFLAGS) $(CFLAGS) -c xinput.c -o $@

$(BUILD_DIR)/native/libxinput_framework.a: $(BUILD_DIR)/native/xinput.o
	$(AR) $(ARFLAGS) $@ $<

$(BUILD_DIR)/native/xinput_framework.dll: $(BUILD_DIR)/native/xinput.o
	$(CC) -shared -o $@ $< $(LDFLAGS) $(XINPUT_LIB) -Wl,--out-implib,$(BUILD_DIR)/native/libxinput_framework.dll.a

$(BUILD_DIR)/x86/xinput.o: xinput.c xinput.h | $(BUILD_DIR)/x86
	$(CC_X86) $(CPPFLAGS) $(CFLAGS) -m32 -c xinput.c -o $@

$(BUILD_DIR)/x86/libxinput_framework.a: $(BUILD_DIR)/x86/xinput.o
	$(AR_X86) $(ARFLAGS) $@ $<

$(BUILD_DIR)/x86/xinput_framework.dll: $(BUILD_DIR)/x86/xinput.o
	$(CC_X86) -shared -m32 -o $@ $< $(LDFLAGS) $(XINPUT_LIB) -Wl,--out-implib,$(BUILD_DIR)/x86/libxinput_framework.dll.a

$(BUILD_DIR)/x64/xinput.o: xinput.c xinput.h | $(BUILD_DIR)/x64
	$(CC_X64) $(CPPFLAGS) $(CFLAGS) -m64 -c xinput.c -o $@

$(BUILD_DIR)/x64/libxinput_framework.a: $(BUILD_DIR)/x64/xinput.o
	$(AR_X64) $(ARFLAGS) $@ $<

$(BUILD_DIR)/x64/xinput_framework.dll: $(BUILD_DIR)/x64/xinput.o
	$(CC_X64) -shared -m64 -o $@ $< $(LDFLAGS) $(XINPUT_LIB) -Wl,--out-implib,$(BUILD_DIR)/x64/libxinput_framework.dll.a

clean:
	rm -rf $(BUILD_DIR)
