include ../config.mk

EXECUTABLE=xresources2putty

OBJS=xresources2putty.o

CFLAGS=`pkg-config --cflags x11`
LDFLAGS=`pkg-config --libs x11`

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	@echo LD $<
	@$(CC) $(DEBUG) -o $(EXECUTABLE) $(LDFLAGS) $(OBJS)

%.o: %.c
	@echo CC $<
	@$(CC) $(DEBUG) -c $< $(CFLAGS)

.PHONY: install
install: all
	$(MKDIR) -p $(DESTDIR)$(BINDIR)
	$(INSTALL) -m 755 $(EXECUTABLE) $(DESTDIR)$(BINDIR)

.PHONY: debug
debug:
	@$(MAKE) DEBUG=-g all

.PHONY: clean
clean:
	@echo Cleaning src/
	@rm -f $(EXECUTABLE) $(OBJS) *~

