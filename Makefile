.PHONY: all
all:
	@$(MAKE) -C src all

.PHONY: install
install:
	@$(MAKE) -C src install

.PHONY: debug
debug:
	@$(MAKE) -C src debug

.PHONY: literal_colors
literal_colors:
	@$(MAKE) -C scripts literal_colors

.PHONY: clean
clean:
	@echo Cleaning files
	@$(MAKE) -C src clean

