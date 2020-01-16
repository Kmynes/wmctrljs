#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/Xmu/WinUtil.h>
#include <stdbool.h>
#include <ctype.h>

#define _NET_WM_STATE_REMOVE        0    /* remove/unset property */
#define _NET_WM_STATE_ADD           1    /* add/set property */
#define _NET_WM_STATE_TOGGLE        2    /* toggle property  */

#define MAX_PROPERTY_VALUE_LEN 4096
#define SELECT_WINDOW_MAGIC ":SELECT:"
#define ACTIVE_WINDOW_MAGIC ":ACTIVE:"

struct geometry {
    unsigned long x;
    unsigned long y;
    unsigned long width;
    unsigned long height;
};

struct type_desc {
        char *flag;
        Atom number;
};

struct action_desc {
        char *flag;
        Atom number;
};

struct state_desc {
        char *flag;
        Atom number;
};

struct window_info {
    Window win_id;
    unsigned long win_pid;
    unsigned long desktop_number;
    unsigned long showing_desktop;
    char *win_client_machine;
    char *win_class;
    struct type_desc *win_types;
    size_t nbr_type;
    struct action_desc *win_actions;
    size_t nbr_action;
    struct state_desc *win_states;
    size_t nbr_state;
    char *win_name;
    char *win_visible_name;
    char *win_icon_name;
    char *win_visible_icon_name;
    struct geometry *win_geometry;
    XSizeHints *WM_NORMAL_HINTS;
    long wm_normal_hints_supplied;
    XSizeHints *WM_HINTS;
    long wm_hints_supplied;

    Atom *net_wm_strut;
    size_t nbr_net_wm_strut;
};

struct window_list{
    struct window_info *client_list;
    size_t client_list_size;
};

enum STATES {
        CLIENT_LIST_GET = 0,
        CAN_NOT_GET_CLIENT_LIST = 1,

        CAN_NOT_ALLOCATE_MEMORY = 2,

        CAN_NOT_OPEN_DISPLAY = 3,
        CAN_NOT_CLOSE_DISPLAY = 4,

        VIEWPORT_CHANGED = 5,
        CAN_NOT_CHANGE_VIEWPORT = 6,

        GEOMETRY_CHANGED = 7,
        CAN_NOT_CHANGE_GEOMETRY = 8,

        WINDOW_ACTIVATED = 9,
        CAN_NOT_ACTIVATE_WINDOW = 10,

        WINDOWS_ACTIVATED = 11,

        WINDOW_CLOSED = 12,
        CAN_NOT_CLOSE_WINDOW = 13,

        WINDOWS_CLOSED = 14,

        WINDOW_ICON_NAME_SET = 15,
        CAN_NOT_SET_WINDOW_ICON_NAME = 16,

        WINDOWS_ICON_NAME_SET = 17,

        WINDOW_TITLE_SET = 18,
        CAN_NOT_SET_WINDOW_TITLE = 19,

        WINDOWS_TITLE_SET = 20,

        WINDOW_STATE_SET = 21,
        CAN_NOT_SET_WINDOW_STATE = 22,

        WINDOWS_STATE_SET = 23,
        NO_WINDOW_FOUND = 24,
        UNKNOW_WINID = 25,

        WINDOW_MOVED_RESIZED = 26,
        CAN_NOT_MOVE_RESIZE_WINDOW = 27,

        NO_WINDOW_ACTIVE_FOR_NOW = 28,

        WINDOW_MOVED_TO_DESKTOP = 29,
        CAN_NOT_MOVE_WINDOW_TO_DESKTOP = 30,

        CAN_NOT_GET_CURRENT_DESKTOP_PROPERTY = 31,
        SCREEN_GET = 32,
        TOO_MANY_WINDOW_OPENED = 33
};

//NAPI
struct window_list *list_windows_napi();

//SCREEN
Screen *get_screen(Display *disp, enum STATES *st);
void free_screen(Screen *sc);

//UTILS
struct window_list *list_windows(Display *disp, enum STATES *st);
Display *create_display(Display *disp, bool *dispLocal);
bool free_local_display(Display *disp, bool dispLocal);
char *get_error_message(enum STATES st);

void initializeWindowInfo(struct window_info *wi);
struct window_info *create_window_info(Display *disp, Window win);
void fill_window_info(Display *disp, struct window_info *wi, Window win);
void free_window_info_properties(struct window_info *wi);
void free_window_info(struct window_info *wi);
void free_window_list(struct window_list *wl);
void print_window_info(struct window_info *wi);
void copy_window_info(struct window_info *dest_wi, struct window_info *src_wi);

char *get_output_str (char *str);
bool client_msg(Display *disp, Window win, char *msg,
        unsigned long data0, unsigned long data1, 
        unsigned long data2, unsigned long data3,
        unsigned long data4);
Window *get_client_list (Display *disp, unsigned long *size);

//WINDOW-MANAGER
struct window_info *get_wm_info(Display *disp);
bool wm_supports(Display *disp, const char *prop);

//DESKTOP
int switch_desktop (Display *disp, unsigned long target);
int change_number_of_desktops (Display *disp, unsigned long n);
//int list_desktops (Display *disp);
int showing_desktop (Display *disp, unsigned long state);

//WINDOW-PROPERTIES
char *get_property (Display *disp, Window win,
        Atom xa_prop_type, char *prop_name, unsigned long *size);
char *get_window_name (Display *disp, Window win);
char *get_window_visible_name (Display *disp, Window win);
char *get_window_icon_name (Display *disp, Window win);
char *get_window_visible_icon_name(Display *disp, Window win);
unsigned long get_window_desktop(Display *disp, Window win);
struct type_desc *get_window_types(Display *disp, Window win, size_t *size);
struct action_desc *get_window_allowed_actions(Display *disp, Window win, size_t *size);
struct state_desc *get_window_states(Display *disp, Window win, size_t *size);
char *get_window_client_machine(Display *disp, Window win);
struct geometry *get_window_geometry(Display *disp, Window win);
unsigned long get_window_shwing_desktop(Display *disp, Window win);
char *get_window_class (Display *disp, Window win);
unsigned long get_window_pid(Display *disp, Window win);
Atom *get_window_net_wm_strut(Display *disp, Window win, size_t *size);

//WINDOW
struct window_info *get_active_window(Display *disp, enum STATES *st);
struct window_list *get_windows_by_pid(Display *disp, unsigned long pid, enum STATES *st);
struct window_list *get_windows_by_class_name(Display *disp, char *class_name, enum STATES *st);

//WINDOW-EDIT
enum STATES active_window_by_id(Display *disp, Window win);
enum STATES active_windows_by_pid(Display *disp, unsigned long pid);
enum STATES active_windows_by_class_name(Display *disp, char *class_name);
enum STATES close_window_by_id(Display *disp, Window win);
enum STATES close_windows_by_pid(Display *disp, unsigned long pid);
enum STATES close_windows_by_class_name(Display *disp, char *class_name);
enum STATES window_state(Display *disp, Window win, unsigned long action, 
        char *prop1_str, char *prop2_str);
enum STATES window_set_title(Display *disp, Window win,
        const char *_net_wm_name);
enum STATES window_set_icon_name(Display *disp, Window win, 
        const char *_net_wm_icon_name);

enum STATES change_geometry(Display *disp, unsigned long x, unsigned long y);
enum STATES change_viewport(Display *disp, unsigned long x, unsigned long y);

Window Select_Window(Display *dpy);

//WINDOW-MOVE
enum STATES window_to_desktop (Display *disp, Window win, int desktop);
enum STATES window_to_current_desktop(Display *disp, Window win);
enum STATES window_move_resize(Display *disp, Window win, int32_t grav, 
    int32_t x, int32_t y, 
    int32_t w, int32_t h);