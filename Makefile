PKGS = wayland-server wlroots-0.20
FLAGS = `pkg-config --libs --cflags $(PKGS)` -DWLR_USE_UNSTABLE

make:
	wayland-scanner server-header protocol/drm.xml include/drm-protocol.h
	wayland-scanner server-header protocol/xdg-shell.xml include/xdg-shell-protocol.h
	gcc -o cwc -g main.c $(FLAGS) -Iinclude
