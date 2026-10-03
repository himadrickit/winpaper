CC      = gcc
CFLAGS  = -O2 -Wall -Wextra -std=c11
LDLIBS  = -ladvapi32 -luser32
TARGET  = wp.exe

# Native Windows (cmd.exe) vs POSIX-style shell (MSYS2, Git Bash, Linux)
ifeq ($(OS),Windows_NT)
ifneq ($(shell echo %OS%),%OS%)
WINCMD = 1
endif
endif

ifdef WINCMD
PREFIX ?= $(USERPROFILE)\bin
fixpath = $(subst /,\,$1)
MKDIR   = if not exist "$(call fixpath,$1)" mkdir "$(call fixpath,$1)"
CP      = copy /Y "$(call fixpath,$1)" "$(call fixpath,$2)" >nul
RM      = if exist "$(call fixpath,$1)" del /Q /F "$(call fixpath,$1)"
else
PREFIX ?= $(HOME)/bin
MKDIR   = mkdir -p "$1"
CP      = cp "$1" "$2"
RM      = rm -f "$1"
endif

all: $(TARGET)

config.h:
	$(call CP,config.def.h,config.h)

$(TARGET): winpapers.c config.h
	$(CC) $(CFLAGS) -o $(TARGET) winpapers.c $(LDLIBS)

install: $(TARGET)
	$(call MKDIR,$(PREFIX))
	$(call CP,$(TARGET),$(PREFIX)/$(TARGET))

clean:
	$(call RM,$(TARGET))

.PHONY: all install clean
