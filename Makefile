PKGS = wayland-server wlroots-0.20
FLAGS = `pkg-config --libs --cflags $(PKGS)` -DWLR_USE_UNSTABLE

make:
	wayland-scanner server-header protocol/drm.xml protocol/drm-protocol.h
	wayland-scanner private-code protocol/drm.xml protocol/drm-protocol.c
	wayland-scanner server-header protocol/xdg-shell.xml protocol/xdg-shell-protocol.h
	wayland-scanner private-code protocol/xdg-shell.xml protocol/xdg-shell-protocol.c
	gcc -o cwc -g main.c $(FLAGS) -Iprotocol
