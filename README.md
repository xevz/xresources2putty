What?
=====

xresources2putty is a small utility which takes an Xresources file and converts it into a Windows registery file which changes the theme (colors) of one or more PuTTY sessions.

Building
========

Edit config.mk if needed.

Build the software:

    $ make

Build with debug symbols:

    $ make debug

Regenerate literal colors.h:

    $ make literal_colors

Running
=======

Using the XTerm class and $HOME/.Xdefaults, for PuTTY sessions
"default" and "server":

    $ xresources2putty -class XTerm -sessions default,server $HOME/.Xdefaults

xresources2putty will output the registery file to standard output, so it's wise to redirect the output:

    $ xresources2putty -class XTerm -sessions default,server $HOME/.Xdefaults > putty_colors.reg

Then copy the file to your Windows box of choice and import it, by either double clicking it in Explorer, or use regedit.

