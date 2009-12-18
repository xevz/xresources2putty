PERL=perl

all: literal_colors

.PHONY: literal_colors
literal_colors: generate_literal.pl
	@echo Generating literal_colors.h
	@$(PERL) generate_literal.pl

