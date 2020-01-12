#include "./wmctrl.h"

static bool activate_window(Display *disp, Window win,
        bool switch_desktop) {
    unsigned long *desktop;

    /* desktop ID */
    if ((desktop = (unsigned long *)get_property(disp, win,
            XA_CARDINAL, "_NET_WM_DESKTOP", NULL)) == NULL) {
        if ((desktop = (unsigned long *)get_property(disp, win,
                XA_CARDINAL, "_WIN_WORKSPACE", NULL)) == NULL) {
            printf("Cannot find desktop ID of the window.\n");
        }
    }

    if (switch_desktop && desktop) {
        if (client_msg(disp, DefaultRootWindow(disp), 
                    "_NET_CURRENT_DESKTOP", 
                    *desktop, 0, 0, 0, 0) != True) {
            printf("Cannot switch desktop.\n");
        }
    }

    XMapRaised(disp, win);
    if (desktop)
        free(desktop);
    return client_msg(disp, win, "_NET_ACTIVE_WINDOW", 
            0, 0, 0, 0, 0);;
}

//ACTIVE SECTION
int active_window_by_id(Display *disp, Window wid) {
    return activate_window(disp, wid, false);
}

int active_windows_by_pid(Display *disp, unsigned long pid) {
    struct window_list *wl = get_windows_by_pid(disp, pid);
    if (!wl)
        return 0;

    int total_sucess = 1;
    for (size_t i = 0; i < wl->client_list_size; i++) {
        if (!active_window_by_id(disp, wl->client_list[i].win_id))
            total_sucess = 0;
    }
    free_window_list(wl);
    return total_sucess;
}

int active_windows_by_class_name(Display *disp, char *class_name) {
    struct window_list *wl = get_windows_by_class_name(disp, class_name);
    if (!wl)
        return 0;

    int total_sucess = 1;
    for (size_t i = 0; i < wl->client_list_size; i++) {
        if(!active_window_by_id(disp, wl->client_list[i].win_id))
            total_sucess = 0;
    }
    free_window_list(wl);
    return total_sucess;
}

//CLOSE SECTION
int close_window(Display *disp, Window win) {
    return client_msg(disp, win, "_NET_CLOSE_WINDOW", 
            0, 0, 0, 0, 0);
}

static int close_window_by(Display *disp, char mode, void *data) {
    struct window_list *wl = NULL;
    switch (mode) {
        case 'p':
            wl = get_windows_by_pid(disp, (unsigned long) data);
        break;
        case 'c':
            wl = get_windows_by_class_name(disp, (char *) data);
        break;
        default:
            return 0;
        break;
    }

    if (!wl)
        return 0;

    int total_sucess = 1;
    for (unsigned long i = 0; i < wl->client_list_size; i++) {
        if (close_window(disp, wl->client_list[i].win_id))
            total_sucess = 0;
    }
    free_window_list(wl);
    return total_sucess;
}

int close_windows_by_pid(Display *disp, unsigned long pid) {
    return close_window_by(disp, 'p',&pid);
}

int close_windows_by_class_name(Display *disp, char *class_name) {
    return close_window_by(disp, 'c',class_name);
}

int window_set_icon_name(Display *disp, Window win, 
    const char *_net_wm_icon_name) {
    return XChangeProperty(disp, win, XInternAtom(disp, "_NET_WM_ICON_NAME", False), 
            XInternAtom(disp, "UTF8_STRING", False), 8, PropModeReplace,
            (const unsigned char *)_net_wm_icon_name, strlen(_net_wm_icon_name));
}

int window_set_title(Display *disp, Window win,
    const char *_net_wm_name) {
    return XChangeProperty(disp, win, XInternAtom(disp, "_NET_WM_NAME", False), 
            XInternAtom(disp, "UTF8_STRING", False), 8, PropModeReplace,
            (const unsigned char *)_net_wm_name, strlen(_net_wm_name));
}

/**
 * action _NET_WM_STATE_REMOVE | _NET_WM_STATE_ADD | _NET_WM_STATE_TOGGLE
 * prop1, prop2: modal_sticky | maximized_vert | maximized_horz |
    shaded | skip_taskbar | skip_pager | hidden | fullscreen | above
**/
int window_state (Display *disp, Window win, unsigned long action, 
        char *prop1_str, char *prop2_str) {

    char *uppercase_prop1 = strdup(prop1_str);
    for (size_t i = 0; uppercase_prop1[i]; i++)
        uppercase_prop1[i] = toupper(uppercase_prop1[i]);

    char *tmp_prop1 = malloc(sizeof(char) * (strlen(uppercase_prop1) + 64));
    sprintf(tmp_prop1, "_NET_WM_STATE_%s", uppercase_prop1);

    Atom prop1 = XInternAtom(disp, tmp_prop1, False);
    free(uppercase_prop1);
    free(tmp_prop1);

    char *uppercase_prop2 = strdup(prop2_str);
    for (size_t i = 0; uppercase_prop2[i]; i++)
        uppercase_prop2[i] = toupper(uppercase_prop2[i]);
    char *tmp_prop2 = malloc(sizeof(char) * (strlen(prop2_str) + 64));
    sprintf(tmp_prop2, "_NET_WM_STATE_%s", uppercase_prop2);

    Atom prop2 = XInternAtom(disp, tmp_prop2, False);
    free(uppercase_prop2);
    free(tmp_prop2);

    return client_msg(disp, win, "_NET_WM_STATE", 
        action, (unsigned long)prop1, (unsigned long)prop2, 0, 0);
}

/*
int action_window (Display *disp, Window win, char mode) {

        
        case 't':
            // move the window to the specified desktop => -r -t /
            return window_to_desktop(disp, win, atoi(options.param));
        
        case 'R':
            // move the window to the current desktop and activate it => -r /
            if (window_to_desktop(disp, win, -1) == EXIT_SUCCESS) {
                usleep(100000); // 100 ms - make sure the WM has enough
                    //time to move the window, before we activate it /
                return activate_window(disp, win, FALSE);
            }
            else {
                return EXIT_FAILURE;
            }

        case 'N': case 'I': case 'T':
            window_set_title(disp, win, options.param, mode);
            return EXIT_SUCCESS;

        default:
            fprintf(stderr, "Unknown action: '%c'\n", mode);
            return EXIT_FAILURE;
    }
}

static int action_window_pid (Display *disp, char mode) {
    unsigned long wid;

    if (sscanf(options.param_window, "0x%lx", &wid) != 1 &&
            sscanf(options.param_window, "0X%lx", &wid) != 1 &&
            sscanf(options.param_window, "%lu", &wid) != 1) {
        fputs("Cannot convert argument to number.\n", stderr);
        return EXIT_FAILURE;
    }

    return action_window(disp, (Window)wid, mode);
}

static int action_window_str (Display *disp, char mode) {
    Window activate = 0;
    Window *client_list;
    unsigned long client_list_size;
    int i;
    
    if (strcmp(SELECT_WINDOW_MAGIC, options.param_window) == 0) {
        activate = Select_Window(disp);
        if (activate) {
            return action_window(disp, activate, mode);
        }
        else {
            return EXIT_FAILURE;
        }
    }
    if (strcmp(ACTIVE_WINDOW_MAGIC, options.param_window) == 0) {
        activate = get_active_window(disp);
        if (activate)
        {
            return action_window(disp, activate, mode);
        }
        else
        {
            return EXIT_FAILURE;
        }
    }
    else {
        if ((client_list = get_client_list(disp, &client_list_size)) == NULL) {
            return EXIT_FAILURE; 
        }
        
        for (i = 0; i < client_list_size / sizeof(Window); i++) {
 			char *match_utf8;
 			if (options.show_class) {
 	            match_utf8 = get_window_class(disp, client_list[i]); // UTF8 /
 			}
 			else {
 				match_utf8 = get_window_title(disp, client_list[i]); // UTF8 /
 			}
            if (match_utf8) {
                char *match;
                char *match_cf;
                char *match_utf8_cf = NULL;
                if (envir_utf8) {
                    match = g_strdup(options.param_window);
                    match_cf = g_utf8_casefold(options.param_window, -1);
                }
                else {
                    if (! (match = g_locale_to_utf8(options.param_window, -1, NULL, NULL, NULL))) {
                        match = g_strdup(options.param_window);
                    }
                    match_cf = g_utf8_casefold(match, -1);
                }
                
                if (!match || !match_cf) {
                    continue;
                }

                match_utf8_cf = g_utf8_casefold(match_utf8, -1);

                if ((options.full_window_title_match && strcmp(match_utf8, match) == 0) ||
                        (!options.full_window_title_match && strstr(match_utf8_cf, match_cf))) {
                    activate = client_list[i];
                    g_free(match);
                    g_free(match_cf);
                    g_free(match_utf8);
                    g_free(match_utf8_cf);
                    break;
                }
                g_free(match);
                g_free(match_cf);
                g_free(match_utf8);
                g_free(match_utf8_cf);
            }
        }
        g_free(client_list);

        if (activate) {
            return action_window(disp, activate, mode);
        }
        else {
            return EXIT_FAILURE;
        }
    }
}*/