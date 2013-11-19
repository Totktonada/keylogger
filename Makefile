SRCMODULES = key_names.c timestamp.c autorun.c main.c
OBJMODULES = $(SRCMODULES:.c=.obj)
HEADERS = $(SRCMODULES:.c=.h)
EXEC_FILE = clock.exe

WINDRES = i686-mingw32-windres
CC = i686-mingw32-gcc

#DEFINE = $(DEFINE) -DDEBUG

CFLAGS = -Wall -Wextra -std=c99 -mwindows $(DEFINE)
CFLAGS_MAIN = $(CFLAGS) -Wno-unused-parameter

default: $(EXEC_FILE)

main.obj: main.c
	$(CC) $(CFLAGS_MAIN) -c $< -o $@

%.obj: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

resource.o: resource.rc
	$(WINDRES) $< -o $@

$(EXEC_FILE): $(OBJMODULES) resource.o
	$(CC) $(CFLAGS) $^ -o $@

ifneq (clean, $(MAKECMDGOALS))
ifneq (clang_analyze_clean, $(MAKECMDGOALS))
-include deps.mk
endif
endif

deps.mk: $(SRCMODULES) $(HEADERS)
	$(CC) -MM $^ > $@

clean:
	rm -f resource.o *.obj $(EXEC_FILE) deps.mk *.core core

clang_analyze_clean:
	rm -f *.h.gch *.plist
