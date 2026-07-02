
Debian
====================
This directory contains files used to package filopowd/filopow-qt
for Debian-based Linux systems. If you compile filopowd/filopow-qt yourself, there are some useful files here.

## filopow: URI support ##


filopow-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install filopow-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your filopow-qt binary to `/usr/bin`
and the `../../share/pixmaps/filopow128.png` to `/usr/share/pixmaps`

filopow-qt.protocol (KDE)

