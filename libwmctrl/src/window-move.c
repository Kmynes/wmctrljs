#include "./wmctrl.h"

enum STATES window_to_desktop(Display *disp, Window win, int desktop) {
    unsigned long *cur_desktop = NULL;
    Window root = DefaultRootWindow(disp);

    if (desktop == -1) {
        if (! (cur_desktop = (unsigned long *)get_property(disp, root,
                XA_CARDINAL, "_NET_CURRENT_DESKTOP", NULL))) {
            if (! (cur_desktop = (unsigned long *)get_property(disp, root,
                    XA_CARDINAL, "_WIN_WORKSPACE", NULL))) {
                fputs("Cannot get current desktop properties. "
                      "(_NET_CURRENT_DESKTOP or _WIN_WORKSPACE property)"
                      "\n", stderr);
                return CAN_NOT_GET_CURRENT_DESKTOP_PROPERTY;
            }
        }
        desktop = *cur_desktop;
    }
    free(cur_desktop);

    bool res = client_msg(disp, win, "_NET_WM_DESKTOP", (unsigned long)desktop,
            0, 0, 0, 0);
    return res ? WINDOW_MOVED_TO_DESKTOP : CAN_NOT_MOVE_WINDOW_TO_DESKTOP;
}

enum STATES window_to_current_desktop(Display *disp, Window win) {
    enum STATES st = window_to_desktop(disp, win, -1);
    if (st == WINDOW_MOVED_TO_DESKTOP) {
        usleep(100000); // 100 ms - make sure the WM has enough
        //time to move the window, before we activate it /
        active_window_by_id(disp, win);
    }
    return st;
}

//_NET_MOVERESIZE_WINDOW
enum STATES window_move_resize(Display *disp, Window win, int32_t grav, 
    int32_t x, int32_t y, 
    int32_t w, int32_t h) {
    int32_t grflags;
    bool dispLocal;
    disp = create_display(disp, &dispLocal);
    if (!disp)
        return CAN_NOT_OPEN_DISPLAY;

    if (grav < 0)
        return CAN_NOT_MOVE_RESIZE_WINDOW;

    grflags = grav;
    if (x != -1) grflags |= (1 << 8);
    if (y != -1) grflags |= (1 << 9);
    if (w != -1) grflags |= (1 << 10);
    if (h != -1) grflags |= (1 << 11);

    if (wm_supports(disp, "_NET_MOVERESIZE_WINDOW")){
        bool res = client_msg(disp, win, "_NET_MOVERESIZE_WINDOW", grflags, x, y, w, h);
        free_local_display(disp, dispLocal);
        return res ? WINDOW_MOVED_RESIZED : CAN_NOT_MOVE_RESIZE_WINDOW;
    }
    else {
        if ((w < 1 || h < 1) && (x >= 0 && y >= 0))
            XMoveWindow(disp, win, x, y);

        else if ((x < 0 || y < 0) && (w >= 1 && h >= -1))
            XResizeWindow(disp, win, w, h);
        else if (x >= 0 && y >= 0 && w >= 1 && h >= 1)
            XMoveResizeWindow(disp, win, x, y, w, h);

        free_local_display(disp, dispLocal);
        return WINDOW_MOVED_RESIZED;
    }
}