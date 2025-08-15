#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-server.h>
#include <wayland-util.h>
#include <wlr/backend.h>
#include <wlr/render/allocator.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_input_device.h>
#include <wlr/types/wlr_keyboard.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_output_layout.h>
#include <wlr/types/wlr_pointer.h>
#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/types/wlr_subcompositor.h>
#include <wlr/types/wlr_xdg_shell.h>
#include <xdg-shell-protocol.h>

#define wl_array_insert(array_p, elm) { \
	if (array_p->size + sizeof(elm) > alloc) \
		array_p->data = wl_array_add(array_p, array_p->alloc); \
	((typeof(elm) *)array_p->data)[array_p->size / sizeof(elm)] = elm; \
	array_p->size += sizeof(elm); \
}

/* Consise unsigned typedefs */
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

/*
 * Wayland interface implementations
 */
struct wl_state {
	struct wl_list outputs;
	struct wl_list compositors;
	struct wl_list subcompositors;
	struct wl_list shms;
};

struct shm {
	struct wl_list link;
	struct wl_resource *resource;
	struct wl_state *state;
	void *addr;
	int size;
};

struct output {
	struct wl_list link;
	struct wl_resource *resource;
	struct wl_state *state;
};

struct compositor {
	struct wl_list link;
	struct wl_resource *resource;
	struct wl_state *state;
};

struct subcompositor {
	struct wl_list link;
	struct wl_resource *resource;
	struct wl_state *state;
};


/*
 * Global variables
 */
struct wl_state global_state = {
#define WL_LIST_HEAD(name) { .prev = &name, .next = &name, }
	.outputs = WL_LIST_HEAD(global_state.outputs),
	.compositors = WL_LIST_HEAD(global_state.compositors),
	.shms = WL_LIST_HEAD(global_state.shms),
};

static int log_fd = 1;
#define report(str) write(log_fd, str "\n", sizeof(str))
#define die(str) { report(str); exit(1); }


void
server_new_output(struct wl_listener *listener, void *data)
{
}


int
main(int argc, char *argv[])
{
	/* Open log file*/
	//log_fd = open("/home/basil/c/log", O_WRONLY | O_CREAT);
	//if (log_fd == -1)
	//	return 1;


	/* Initialise wayland display */
	struct wl_display *display = wl_display_create();
	if (display == NULL)
		die("Failed to create wayland display");

	/* Create UNIX socket */
	const char *socket = "wayland-1";
	int r = wl_display_add_socket(display, socket);
	if (r == -1)
		die("Fucking give up");

	/* Get reference to event loop */
	struct wl_event_loop *event_loop = wl_display_get_event_loop(display);
	if (event_loop == NULL)
		die("Failed to obtain reference to the wl event loop");

	/* TODO: Set up renderer and input devices */
	struct wlr_backend *backend = wlr_backend_autocreate(event_loop, NULL);
	struct wlr_renderer *renderer = wlr_renderer_autocreate(backend);
	wlr_renderer_init_wl_display(renderer, display);

	struct wlr_allocator *allocator = wlr_allocator_autocreate(backend, renderer);
	wlr_compositor_create(display, 6, renderer);
	wlr_subcompositor_create(display);
	wlr_data_device_manager_create(display);

	struct wl_list outputs = { &outputs, &outputs };
	struct wlr_output_layout *output_layout = wlr_output_layout_create(display);
	struct wl_listener new_output = {
		.notify = server_new_output,
	};
	wl_signal_add(&backend->events.new_output, &new_output);

	struct wlr_scene *scene = wlr_scene_create();
	struct wlr_scene_output_layout *
	scene_layout = wlr_scene_attach_output_layout(scene, output_layout);

	struct wl_list toplevels = { &toplevels, &toplevels };
	struct wlr_xdg_shell *xdg_shell = wlr_xdg_shell_create(display, 3);


	setenv("WAYLAND_DISPLAY", socket, 1);
	wl_display_run(display);


	/* Cleanup */
	wl_display_destroy(display);
	return 0;
}
