PKGS = wayland-server wlroots-0.20
FLAGS = `pkg-config --libs --cflags $(PKGS)` -DWLR_USE_UNSTABLE -Iinclude/

make:
	gcc -o cwc -g main.c $(FLAGS)
