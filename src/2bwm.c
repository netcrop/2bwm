/* 2bwm, a fast floating WM  with the particularity of having 2 borders written
 * over the XCB library and derived from mcwm written by Michael Cardell.
 * Heavily modified version of http://www.hack.org/mc/hacks/mcwm/
 * Copyright (c) 2010, 2011, 2012 Michael Cardell Widerkrantz, mc at the domain hack.org.
 * Copyright (c) 2014, 2015 Patrick Louis, patrick at the domain iotek dot org.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. */
#include "2bwm.h"
void
fixwin()
{
    fixwindow(path[curpi]);
}

void
unkillablewin()
{
    unkillablewindow(path[curpi]);
}

void
delmonitor(monitor_t *mon)
{
    // recycle monitor node.
    denode(mon->node);
}

void
raise_current_window(void)
{
    if(!path[curpi])
        return;
    client_t *client = path[curpi]->data;
    raisewindow(client->id);
}

inline void
delfromworkspace(node_t *node, uint32_t ws)
{
    printf("%s nodeid: %d ws: %d wspi(ws): %d\n",__func__,node->id,ws,wspi(ws));
    // remove from workspace ws and location.
    for(int i = wspi(ws); i < wspi(ws) + maxloci;i++){
        unlinknode(i,node);
    }
}
void
changeworkspace(const Arg *arg)
{
    changews_help(arg->i);
}

void
nextworkspace()
{
    curwsi == WORKSPACES - 1 ? changews_help(0)
        :changews_help(curwsi+1);
}

void
prevworkspace()
{
    curwsi > 0 ? changews_help(curwsi - 1)
        : changews_help(WORKSPACES-1);
}

void
saveorigsize(client_t *client)
{
    client->origsize.x      = client->x;
    client->origsize.y      = client->y;
    client->origsize.width  = client->width;
    client->origsize.height = client->height;
}

void
centerpointer(xcb_drawable_t win, client_t *cl)
{
    int16_t cur_x, cur_y;

    cur_x = cur_y = 0;

    switch(CURSOR_POSITION) {
        case BOTTOM_RIGHT:
        cur_x += cl->width;
        case BOTTOM_LEFT:
        cur_y += cl->height; break;
        case TOP_RIGHT:
        cur_x += cl->width;
        case TOP_LEFT:
        break;
        default:
        cur_x = cl->width / 2;
        cur_y = cl->height / 2;
    }

    xcb_warp_pointer(conn, XCB_NONE, win, 0, 0, 0, 0, cur_x, cur_y);
}
void
showclientlist(int headpi)
{
    debug2(__func__,"");
    uint32_t len = 0;
    xcb_window_t *children = NULL;
    client_t *client = NULL;
    /* can only be called after the first window has been spawn */
    xcb_query_tree_reply_t *reply =
    xcb_query_tree_reply(conn, xcb_query_tree(conn, screen->root), 0);
    len = xcb_query_tree_children_length(reply);
    children = xcb_query_tree_children(reply);
    for (int i=0; i < len; i ++) {
        findclient(headpi,&children[i]);
    }
    delocate(reply);
}
void
updateclientlist(void)
{
    debug2(__func__,"");
    uint32_t len = 0;
    xcb_window_t *children = NULL;
    client_t *cl = NULL;
    /* can only be called after the first window has been spawn */
    xcb_query_tree_reply_t *reply =
    xcb_query_tree_reply(conn, xcb_query_tree(conn, screen->root), 0);
    xcb_delete_property(conn, screen->root, ewmh->_NET_CLIENT_LIST);
    xcb_delete_property(conn, screen->root, ewmh->_NET_CLIENT_LIST_STACKING);
    if (!reply) {
        addtoclientlist(0);
        return;
    }
    len = xcb_query_tree_children_length(reply);
    children = xcb_query_tree_children(reply);
    for (int i=0; i < len; i ++) {
//        printf("%s i: %d childi: %d\n",__func__,i,children[i]);
        if((cl = findclient(winpi,&children[i])))
            addtoclientlist(cl->id);
    }
    delocate(reply);
}

/* get screen of display */
xcb_screen_t *
xcb_screen_of_display(xcb_connection_t *con, int screen)
{
    xcb_screen_iterator_t iter;
    iter = xcb_setup_roots_iterator(xcb_get_setup(con));
    for (; iter.rem; --screen, xcb_screen_next(&iter))
        if (screen == 0)
            return iter.data;

    return NULL;
}

void
movepointerback(const int16_t startx, const int16_t starty,
        const client_t *client)
{
    if (startx > (0 - conf.borderwidth - 1) && startx < (client->width
                + conf.borderwidth + 1) && starty > (0
                    - conf.borderwidth - 1) && starty
            < (client->height + conf.borderwidth + 1))
        xcb_warp_pointer(conn, XCB_NONE, client->id,0,0,0,0, startx,
                starty);
}

/* Set keyboard focus to follow mouse pointer. Then exit. We don't need to
 * bother mapping all windows we know about. They should all be in the X
 * server's Save Set and should be mapped automagically. */
void
cleanup(void)
{
    delocate(ev);
    delall();
    if (ewmh){
        xcb_ewmh_connection_wipe(ewmh);
        delocate(ewmh);
    }
    if(!conn)
        return;
    xcb_set_input_focus(conn, XCB_NONE,XCB_INPUT_FOCUS_POINTER_ROOT,
            XCB_CURRENT_TIME);
    xcb_flush(conn);
    xcb_disconnect(conn);
}

/* Rearrange windows to fit new screen size. */
void
arrangewindows(void)
{
    client_t *client;
    node_t *node = path[winpi];
    /* Go through all windows and resize them appropriately to
     * fit the screen. */
    if(!node)
        return;
    do{
        fitonscreenwin(node->data);
    }while((node = next(node,winpi)) && node != path[winpi]);
}

uint32_t getwmdesktop(xcb_drawable_t win)
{   // Get EWWM hint so we might know what workspace window win should be visible on.
    // Returns either workspace, NET_WM_FIXED if this window should be
    // visible on all workspaces or TWOBWM_NOWS if we didn't find any hints.
    xcb_get_property_cookie_t cookie = xcb_get_property(conn,
        false, win, ewmh->_NET_WM_DESKTOP,
        XCB_GET_PROPERTY_TYPE_ANY, 0, sizeof(uint32_t));
    xcb_get_property_reply_t *reply = xcb_get_property_reply(conn, cookie, NULL);
    if (NULL==reply || 0 == xcb_get_property_value_length(reply))
    { /* 0 if we didn't find it. */
        if(NULL!=reply) delocate(reply);
        return TWOBWM_NOWS;
    }
    uint32_t wsp = *(uint32_t *)xcb_get_property_value(reply);
    if(NULL!=reply)
        delocate(reply);
    return wsp;
}

/* check if the window is unkillable, if yes return true */
bool
get_unkil_state(xcb_drawable_t win)
{
    xcb_get_property_cookie_t cookie;
    xcb_get_property_reply_t *reply;
    uint8_t wsp;

    cookie = xcb_get_property(conn, false, win, ewmh->_NET_WM_STATE_DEMANDS_ATTENTION,
            XCB_GET_PROPERTY_TYPE_ANY, 0,sizeof(uint8_t));

    reply  = xcb_get_property_reply(conn, cookie, NULL);

    if (reply == NULL || xcb_get_property_value_length(reply) == 0) {
        if(reply != NULL )
            delocate(reply);
        return false;
    }

    wsp = *(uint8_t *) xcb_get_property_value(reply);

    if (reply != NULL)
        delocate(reply);

    if (wsp == 1)
        return true;
    else
        return false;
}

void
check_namewin(client_t *client)
{
    xcb_get_property_reply_t *reply ;
    unsigned int reply_len;
    char *wm_name_window;
    unsigned int i;
    uint32_t values[1] = { 0 };

    if (NULL == client)
        return;

    reply = xcb_get_property_reply(conn, xcb_get_property(conn, false,
                client->id, getatom(LOOK_INTO) ,
                XCB_GET_PROPERTY_TYPE_ANY, 0,60), NULL
    );

    if (reply == NULL || xcb_get_property_value_length(reply) == 0) {
        if (NULL!=reply)
            delocate(reply);
        return;
    }

    reply_len = xcb_get_property_value_length(reply);
    alocate((void**)&wm_name_window,sizeof(char*) * (reply_len+1));
    memcpy(wm_name_window, xcb_get_property_value(reply), reply_len);
    wm_name_window[reply_len] = '\0';

    if(NULL != reply)
        delocate(reply);

    for(i=0;i<sizeof(ignore_names)/sizeof(__typeof__(*ignore_names));i++)
        if (strstr(wm_name_window, ignore_names[i]) !=NULL) {
            client->ignore_borders = true;
            xcb_configure_window(conn, client->id,
                    XCB_CONFIG_WINDOW_BORDER_WIDTH, values);
            break;
        }

    delocate(wm_name_window);
}
/* Add a window, specified by client, to workspace ws. */
inline void
addtoworkspace(client_t *client, uint32_t ws)
{
    if (!client)
        return;
    linknode(pathindex(curmoni,ws,incloci),client->winnode);
    debug(client->winnode->headpi,client->winnode,__func__,"");
}
static void
addtoclientlist(const xcb_drawable_t id)
{
//    printf("%s id: %d add: %p\n",__func__,id,&id);
    xcb_change_property(conn, XCB_PROP_MODE_APPEND , screen->root, ewmh->_NET_CLIENT_LIST, XCB_ATOM_WINDOW, 32, 1,&id);
    xcb_change_property(conn, XCB_PROP_MODE_APPEND , screen->root, ewmh->_NET_CLIENT_LIST_STACKING, XCB_ATOM_WINDOW, 32, 1,&id);
}
void
changews_help(const uint32_t ws)
{
    xcb_query_pointer_reply_t *pointer;
    client_t *client = NULL;
    node_t *tail,*node = NULL;
    if (ws == curwsi)
        return;
    debug2(__func__,"start");
    xcb_ewmh_set_current_desktop(ewmh, 0, ws);
    if((node = path[fixpi])){
        do{
            unlinknode(node->dimpi,node);
            linknode(pathindex(curmoni,ws,loci(node->dimpi)),node);
        }while((node = next(node,fixpi)) && node != path[fixpi]);
    }
    // go through current worksapce and location.
    for(int i = wspi(curwsi); i < wspi(curwsi) + maxloci;i++){
        // through location
        if(!(node = path[i]))
            continue;
        do{
            client = node->data;
            setborderswin(client,false);
            xcb_unmap_window(conn,client->id);
            debug(i,node,__func__,"unmap");
        }while((node = next(node,i)) && node != path[i]);
    }
    client = NULL;
    // go through workspace ws and location.
    for(int i = wspi(ws); i < wspi(ws) + maxloci;i++){
        if(!(node = path[i]))
            continue;
        do{
            client = node->data;
            xcb_map_window(conn,client->id);
            debug(i,node,__func__,"map");
        }while((node = next(node,i)) && node != path[i]);
    }
    curwsi = ws;
    /* Don't focus any node from recycle path.*/
    if(path[wsfocpi] && !next(path[wsfocpi],recpi)){
        // focus on previous focused window on workspace curwsi.
        debug(curpi,path[wsfocpi],__func__,"wsfocpi");
        setfocuswin(path[wsfocpi]->data);
    }else{
        //focus on any head in workspace ws.
        setfocuswin(client);
    }
    debug2(__func__,"ended");
}
void
always_on_topwin()
{
    debug2(__func__,"");
    client_t *cl = NULL;
    node_t *node = path[curpi];
    if(!node)
        return;
    cl = node->data;
    if(cl->id == top_win){
        top_win = 0;
        setborderswin(cl,true);
        return;
    }
    if (top_win != 0 && (cl = findclient(curwspi,&top_win)))
        setborderswin(cl, false);
    cl = node->data;
    top_win = cl->id;
    raisewindow(top_win);
    setborderswin(cl,true);
}
void
fixwindow(node_t *node)
{
    client_t *client = NULL;
    if(!node)
        return;
    if(next(node,fixpi)){
        unlinknode(fixpi,node);
        setborderswin(node->data,true);
        return;
    }
    client = node->data;
    raisewindow(client->id);
    linknode(fixpi,node);
    setborderswin(node->data,true);
}
/* Make unkillable or killable a window client. If setcolour is */
void
unkillablewindow(node_t* node)
{
    client_t *client = NULL;
    if(!node)
        return;
    client = node->data;
    if (next(node,unkpi)){
        unlinknode(unkpi,node);
        xcb_delete_property(conn, client->id, ewmh->_NET_WM_STATE_DEMANDS_ATTENTION);
        setborderswin(client,true);
        return;
    }
    linknode(unkpi,node);
    raisewindow(client->id);
    xcb_change_property(conn, XCB_PROP_MODE_REPLACE, client->id,
            ewmh->_NET_WM_STATE_DEMANDS_ATTENTION, XCB_ATOM_CARDINAL, 8, 1,
            next(node,unkpi));
    setborderswin(client,true);
}
void
sendtoworkspace(const Arg *arg)
{
    client_t *client = NULL;
    node_t *tail,*node = path[curpi];
    if(!node)
        return;
    if(!next(node,selpi))
        linknode(selpi,node);
    for(node = path[selpi];node;
        tail = node,node = next(node,selpi),unlinknode(selpi,tail)){
        // node is handled before change to next.
        if(wsi(node->dimpi) == arg->i || next(node,fixpi))
            continue;
        client = node->data;
        unlinknode(node->dimpi,node);
        linknode(pathindex(curmoni,arg->i,loci(node->dimpi)),node);
        xcb_unmap_window(conn,client->id);
        setborderswin(client,false);
    }
    // focus is required for storing old focus.
    focusnextwin();
}

void
sendtonextworkspace(const Arg *arg)
{
    Arg arg2 = { .i=0 };
    Arg arg3 = { .i=curwsi + 1 };
    curwsi == WORKSPACES - 1 ? sendtoworkspace(&arg2):sendtoworkspace(&arg3);
}

void
sendtoprevworkspace(const Arg *arg)
{
    Arg arg2 = {.i=curwsi-1};
    Arg arg3 = {.i=WORKSPACES-1};
    curwsi > 0 ? sendtoworkspace(&arg2) : sendtoworkspace(&arg3);
}

/* Get the pixel values of a named colour colstr. Returns pixel values. */
uint32_t
getcolor(const char *hex)
{
    uint32_t rgb48;
    char strgroups[7] = {
        hex[1], hex[2], hex[3], hex[4], hex[5], hex[6], '\0'
    };

    rgb48 = strtol(strgroups, NULL, 16);
    return rgb48 | 0xff000000;
}

void
getmonsize(bool with_offsets, int16_t *mon_x, int16_t *mon_y,
        uint16_t *mon_width, uint16_t *mon_height,
        const client_t *client)
{
    if (!client || !client->monitor){
        /* Window isn't attached to any monitor, so we use
         * the root window size. */
        *mon_x = 0;
        *mon_y = 0;
        *mon_width  = screen->width_in_pixels;
        *mon_height = screen->height_in_pixels;
    } else {
        *mon_x      = client->monitor->x;
        *mon_y      = client->monitor->y;
        *mon_width  = client->monitor->width;
        *mon_height = client->monitor->height;
    }
    if (with_offsets){
        *mon_x      += offsets[0];
        *mon_y      += offsets[1];
        *mon_width  -= offsets[2];
        *mon_height -= offsets[3];
    }
}

void
noborderwin(int16_t *temp,client_t *client, bool set_unset)
{
    if (!client->ignore_borders)
        return;
    if (set_unset){
        *temp = conf.borderwidth;
        conf.borderwidth = 0;
        return;
    }
    conf.borderwidth = *temp;
}
void
maximizewin_helper(client_t *client,uint16_t mon_x, uint16_t mon_y,
        uint16_t mon_width, uint16_t mon_height)
{
    uint32_t values[4];

    values[0] = 0;
    saveorigsize(client);
    xcb_configure_window(conn, client->id, XCB_CONFIG_WINDOW_BORDER_WIDTH,
            values);

    client->x = mon_x;
    client->y = mon_y;
    client->width = mon_width;
    client->height = mon_height;

    moveresizewin(client->id, client->x, client->y, client->width,
            client->height);
    client->maxed = true;
}

/* Fit client on physical screen, moving and resizing as necessary. */
void
fitonscreenwin(client_t *client)
{
    int16_t mon_x, mon_y,temp=0;
    uint16_t mon_width, mon_height;
    bool willmove,willresize;
    willmove = willresize = client->vertmaxed = client->hormaxed = false;
    getmonsize(true, &mon_x, &mon_y, &mon_width, &mon_height,client);
    if (client->maxed){
        client->maxed = false;
        setborderswin(client,false);
    } else {
        /* not maxed but look as if it was maxed, then make it maxed */
        if (client->width==mon_width && client->height==mon_height)
            willresize = true;
        else {
            getmonsize(false, &mon_x, &mon_y, &mon_width, &mon_height,
                    client);
            if (client->width == mon_width && client->height
                    == mon_height)
                willresize = true;
        }
        if (willresize) {
            client->x = mon_x;
            client->y = mon_y;
            client->width -= conf.borderwidth * 2;
            client->height-= conf.borderwidth * 2;
            maximizewin_helper(client, mon_x, mon_y, mon_width,
                    mon_height);
            return;
        } else {
            getmonsize(true, &mon_x, &mon_y, &mon_width,
                    &mon_height,client);
        }
    }
    if (client->x > mon_x + mon_width || client->y > mon_y + mon_height
            || client->x < mon_x||client->y < mon_y) {
        willmove = true;
        /* Is it outside the physical monitor? */
        if (client->x > mon_x + mon_width)
            client->x = mon_x + mon_width
                - client->width-conf.borderwidth*2;
    if (client->y > mon_y + mon_height)
        client->y = mon_y + mon_height - client->height
            - conf.borderwidth*2;
    if (client->x < mon_x)
        client->x = mon_x;
    if (client->y < mon_y)
        client->y = mon_y;
    }
    /* Is it smaller than it wants to  be? */
    if (0 != client->min_height && client->height < client->min_height) {
        client->height = client->min_height;
        willresize = true;
    }
    if (0 != client->min_width && client->width < client->min_width) {
        client->width = client->min_width;
        willresize = true;
    }
    noborderwin(&temp, client, true);
    /* If the window is larger than our screen, just place it in
     * the corner and resize. */
    if (client->width + conf.borderwidth * 2 > mon_width) {
        client->x = mon_x;
        client->width = mon_width - conf.borderwidth * 2;;
        willmove = willresize = true;
    } else
        if (client->x + client->width + conf.borderwidth * 2
                > mon_x + mon_width) {
            client->x = mon_x + mon_width - (client->width
                    + conf.borderwidth * 2);
            willmove = true;
        }
    if (client->height + conf.borderwidth * 2 > mon_height) {
        client->y = mon_y;
        client->height = mon_height - conf.borderwidth * 2;
        willmove = willresize = true;
    } else
        if (client->y + client->height + conf.borderwidth*2 > mon_y
                + mon_height) {
            client->y = mon_y + mon_height - (client->height
                    + conf.borderwidth * 2);
            willmove = true;
        }
    if (willmove)
        movewindow(client->id, client->x, client->y);
    if (willresize)
        resizewin(client->id, client->width, client->height);
    noborderwin( &temp, client, false);
}


/* Set position, geometry and attributes of a new window and show it on
 * the screen.*/
void
newwinrequest(xcb_generic_event_t *ev)
{
    debug2(__func__,"start");
    xcb_map_request_event_t *e = (xcb_map_request_event_t *) ev;
    client_t *client = NULL;
    long data[] = { XCB_ICCCM_WM_STATE_NORMAL, XCB_NONE };
    // unmaped window.
    if(findclient(winpi,&e->window))
        return;
    if (!(client = setupwin(e->window)))
        return;
    /* Add this window to the current workspace. */
    addtoworkspace(client, curwsi);
    subwindowposition(client);
    windowposition(client);
    /* Find the physical output this window will be on if RANDR is active */
    if (-1 != randrbase){
//        client->monitor = findmonbycoord(client->x, client->y);
        client->monitor = path[monpi]->data;
        fitonscreenwin(client);
        if (!client->monitor && path[monpi])
            /* Window coordinates are outside all physical monitors.
             * Choose the first screen.*/
            client->monitor = path[monpi]->data;
    }
    fitonscreenwin(client);
    /* Show window on screen. */
    xcb_map_window(conn,client->id);
    xcb_change_property(conn, XCB_PROP_MODE_REPLACE, client->id,
            ewmh->_NET_WM_STATE, ewmh->_NET_WM_STATE, 32, 2, data);
    raisewindow(client->id);
    setfocuswin(client);
    debug2(__func__,"ended");
}

/* Set border colour, width and event mask for window. */
client_t *
setupwin(xcb_window_t win)
{
    debug2(__func__,"start");
    unsigned int i;
    uint32_t values[2],ws;
    xcb_atom_t a;
    xcb_size_hints_t hints;
    xcb_ewmh_get_atoms_reply_t win_type;
    node_t *node = NULL;
    client_t *client = NULL;
    if(xcb_ewmh_get_wm_window_type_reply(ewmh,
        xcb_ewmh_get_wm_window_type(ewmh, win), &win_type, NULL) == 1)
    {
        for (i = 0; i < win_type.atoms_len; i++) {
            a = win_type.atoms[i];
            if (a == ewmh->_NET_WM_WINDOW_TYPE_TOOLBAR || a
                    == ewmh->_NET_WM_WINDOW_TYPE_DOCK || a
                    == ewmh->_NET_WM_WINDOW_TYPE_DESKTOP ) {
                xcb_map_window(conn,win);
                return NULL;
            }
        }
        xcb_ewmh_get_atoms_reply_wipe(&win_type);
    }
    values[0] = XCB_EVENT_MASK_ENTER_WINDOW;
    xcb_change_window_attributes(conn, win, XCB_CW_BACK_PIXEL,&conf.empty_col);
    xcb_change_window_attributes_checked(conn, win, XCB_CW_EVENT_MASK, values);

    /* Add this window to the X Save Set. */
    xcb_change_save_set(conn, XCB_SET_MODE_INSERT, win);

    /* Remember window and store a few things about it. */
    node = addnode(sizeof(client_t));
    if (!node)
        return NULL;
    client = node->data;
    client->id = win;
    client->x = client->y = client->width = client->height
        = client->min_width
        = client->min_height
        = client->base_width
        = client->base_height
        = client->position
        = 0;

    client->max_width     = screen->width_in_pixels;
    client->max_height    = screen->height_in_pixels;
    client->width_inc     = client->height_inc = 1;
    client->usercoord     = client->vertmaxed = client->hormaxed
        = client->maxed
        = client->ignore_borders
        = false;

    client->monitor       = path[monpi]->data;
    client->winnode       = node;
    /* Get window geometry. */
    getgeom(&client->id, &client->x, &client->y, &client->width,
            &client->height, &client->depth);

    /* Get the window's incremental size step, if any.*/
    xcb_icccm_get_wm_normal_hints_reply(conn,
            xcb_icccm_get_wm_normal_hints_unchecked(conn, win),
            &hints, NULL
    );

    /* The user specified the position coordinates.
     * Remember that so we can use geometry later. */
    if (hints.flags &XCB_ICCCM_SIZE_HINT_US_POSITION)
        client->usercoord = true;

    if (hints.flags &XCB_ICCCM_SIZE_HINT_P_MIN_SIZE) {
        client->min_width  = hints.min_width;
        client->min_height = hints.min_height;
    }

    if (hints.flags &XCB_ICCCM_SIZE_HINT_P_MAX_SIZE) {
        client->max_width  = hints.max_width;
        client->max_height = hints.max_height;
    }

    if (hints.flags &XCB_ICCCM_SIZE_HINT_P_RESIZE_INC) {
        client->width_inc  = hints.width_inc;
        client->height_inc = hints.height_inc;
    }

    if (hints.flags &XCB_ICCCM_SIZE_HINT_BASE_SIZE) {
        client->base_width  = hints.base_width;
        client->base_height = hints.base_height;
    }
    linknode(winpi,node);
    check_namewin(client);
    addtoclientlist(client->id);
    debug2(__func__,"ended");
    return client;
}

/* wrapper to get xcb keycodes from keysymbol */
xcb_keycode_t *
xcb_get_keycodes(xcb_keysym_t keysym)
{
    xcb_key_symbols_t *keysyms;
    xcb_keycode_t *keycode;

    if (!(keysyms = xcb_key_symbols_alloc(conn)))
        return NULL;

    keycode = xcb_key_symbols_get_keycode(keysyms, keysym);
    xcb_key_symbols_free(keysyms);

    return keycode;
}

/* the wm should listen to key presses */
void
grabkeys(void)
{
    xcb_keycode_t *keycode;
    int i,k,m;
    unsigned int modifiers[] = {
        0,
        XCB_MOD_MASK_LOCK,
        numlockmask,
        numlockmask | XCB_MOD_MASK_LOCK
    };

    xcb_ungrab_key(conn, XCB_GRAB_ANY, screen->root, XCB_MOD_MASK_ANY);

    for (i=0; i<LENGTH(keys);i++,delocate(keycode)) {
        // alocate keycodes will be freed by delocate(keycode).
        keycode = xcb_get_keycodes(keys[i].keysym);
        for (k=0; keycode[k] != XCB_NO_SYMBOL; k++)
            for (m=0; m<LENGTH(modifiers); m++)
                xcb_grab_key(conn, 1, screen->root, keys[i].mod
                        | modifiers[m], keycode[k],
                        XCB_GRAB_MODE_ASYNC,//pointer mode
                        XCB_GRAB_MODE_ASYNC //keyboard mode
                        );
    }
}

bool
setup_keyboard(void)
{
    xcb_get_modifier_mapping_reply_t *reply;
    xcb_keycode_t *modmap, *numlock;
    unsigned int i,j,n;

    reply = xcb_get_modifier_mapping_reply(conn,
                xcb_get_modifier_mapping_unchecked(conn), NULL);

    if (!reply)
        return false;

    modmap = xcb_get_modifier_mapping_keycodes(reply);

    if (!modmap)
        return false;

    numlock = xcb_get_keycodes(XK_Num_Lock);

    for (i=4; i<8; i++) {
        for (j=0; j<reply->keycodes_per_modifier; j++) {
            xcb_keycode_t keycode= modmap[i * reply->keycodes_per_modifier + j];

            if (keycode == XCB_NO_SYMBOL)
                continue;

            if(numlock != NULL)
                for (n=0; numlock[n] != XCB_NO_SYMBOL; n++)
                    if (numlock[n] == keycode) {
                        numlockmask = 1 << i;
                        break;
                    }
        }
    }

    delocate(reply);
    delocate(numlock);

    return true;
}

/* Walk through all existing windows and set them up. returns true on success */
bool
setupscreen(void)
{
    xcb_get_window_attributes_reply_t *attr = NULL;
    client_t *client = NULL;
    uint32_t ws = 0;
    uint32_t len = 0;
    xcb_window_t *children = NULL;
    /* Get all children. */
    xcb_query_tree_reply_t *reply = xcb_query_tree_reply(conn, xcb_query_tree(conn, screen->root), 0);
    if (!reply)
        return false;
    len = xcb_query_tree_children_length(reply);
    children = xcb_query_tree_children(reply);
    /* Set up all windows on this root. */
    for (int i=0; i < len; i++,delocate(attr)){
//        printf("%s i: %d childi: %d\n",__func__,i,children[i]);
        if(!(attr = xcb_get_window_attributes_reply(conn,
                xcb_get_window_attributes(conn, children[i]), NULL)))
            continue;
         /* Don't set up or even bother windows in override redirect mode.
         * This mode means they wouldn't have been reported to us with a
         * MapRequest if we had been running, so in the normal case we wouldn't
         * have seen them. Only handle visible windows. */
        if (attr->override_redirect || attr->map_state != XCB_MAP_STATE_VIEWABLE)
            continue;
        if(!(client = setupwin(children[i])))
            continue;
        /* Find the physical output this window will be on if
         * RANDR is active. */
        if (-1 != randrbase)
            client->monitor = findmonbycoord(client->x, client->y);
        /* Fit window on physical screen. */
        fitonscreenwin(client);
        setborderswin(client, false);
        /* Check if this window has a workspace set already
         * as a WM hint. */
        ws = getwmdesktop(children[i]);
        if (get_unkil_state(children[i]))
            unkillablewindow(client->winnode);
        if (ws == NET_WM_FIXED){
            fixwindow(client->winnode);
            continue;
        }
        if (TWOBWM_NOWS != ws && ws < WORKSPACES) {
            addtoworkspace(client, ws);
            if (ws != curwsi)
                xcb_unmap_window(conn,client->id);
            continue;
        }
        addtoworkspace(client, curwsi);
        addtoclientlist(children[i]);
    }
    delocate(reply);
    return true;
}

/* Set up RANDR extension. Get the extension base and subscribe to events */
int
setuprandr(void)
{
    int base = 0;
    const xcb_query_extension_reply_t *extension
        = xcb_get_extension_data(conn, &xcb_randr_id);
    if (!extension->present)
        return -1;
    getrandr();
    base = extension->first_event;
    xcb_randr_select_input(conn, screen->root,
            XCB_RANDR_NOTIFY_MASK_SCREEN_CHANGE
            | XCB_RANDR_NOTIFY_MASK_OUTPUT_CHANGE
            | XCB_RANDR_NOTIFY_MASK_CRTC_CHANGE
            | XCB_RANDR_NOTIFY_MASK_OUTPUT_PROPERTY);
    return base;
}
void
changerandr(void)
{
    debug2(__func__,"start");
    int len = 0;
    xcb_randr_get_screen_resources_current_cookie_t rcookie
        = xcb_randr_get_screen_resources_current(conn, screen->root);
    xcb_randr_get_screen_resources_current_reply_t *res
        = xcb_randr_get_screen_resources_current_reply(conn, rcookie,NULL);
    if (!res)
        return;
    xcb_timestamp_t timestamp = res->config_timestamp;
    len = xcb_randr_get_screen_resources_current_outputs_length(res);
    xcb_randr_output_t *outputs
        = xcb_randr_get_screen_resources_current_outputs(res);
    delocate(res);
    /* Request information for all outputs. */
    changeoutputs(outputs,len,timestamp);
    debug2(__func__,"ended");
}
void
changeoutputs(xcb_randr_output_t *outputs,const int len, xcb_timestamp_t timestamp)
{
    xcb_randr_get_crtc_info_cookie_t icookie;
    xcb_randr_get_crtc_info_reply_t *crtc = NULL;
    xcb_randr_get_output_info_reply_t *output = NULL;
    monitor_t *mon = NULL;
    node_t *node = NULL;
    client_t *client = NULL;
    xcb_randr_get_output_info_cookie_t ocookie[len];
    xcb_window_t win;
    /* Loop through all outputs. */
    for (int i=0; i < len; i++,delocate(output)){
        ocookie[i] = xcb_randr_get_output_info(conn, outputs[i],timestamp);
        if (!(output = xcb_randr_get_output_info_reply(conn, ocookie[i], NULL)))
            continue;
        mon = findmonitor(outputs[i]);
        // when output-crtc plug out but mon structure remain.
        // all clients belonged to that monitor needs transfer.
        printf("%s crtc: %d mon: %p\n",__func__,output->crtc,mon);
        if (!output->crtc){
             if (!mon || !(node = path[winpi]))
                continue;
            printf("%s inside\n",__func__);
            // move all windows to next monitor.
             do{
                 if (!(client = node->data) || client->monitor != mon)
                     continue;
                 if (!next(client->monitor->node,winpi))
                     if (!path[monpi])
                         client->monitor = NULL;
                     else
                         client->monitor = path[monpi]->data;
                 else
                     client->monitor=next(client->monitor->node,monpi)->data;
                 fitonscreenwin(client);
             }while((node = next(node,winpi)) && node != path[winpi]);
             delmonitor(mon);
             continue;
        }
        icookie = xcb_randr_get_crtc_info(conn, output->crtc,timestamp);
        if(!(crtc = xcb_randr_get_crtc_info_reply(conn, icookie, NULL)))
            continue;
        // a new monitor pluged in via output-crtc.
        if (!mon){
            addmonitor(outputs[i],crtc);
            delocate(crtc);
            continue;
        }
        // still the same monitor with same output-crtc, just update info.
        mon->x = crtc->x;
        mon->y = crtc->y;
        mon->width = crtc->width;
        mon->height = crtc->height;
        arrbymon(mon);
        delocate(crtc);
    }
}
/* Get RANDR resources and figure out how many outputs there are. */
void
getrandr(void)
{
    debug2(__func__,"start");
    xcb_randr_get_screen_resources_current_cookie_t rcookie
        = xcb_randr_get_screen_resources_current(conn, screen->root);
    xcb_randr_get_screen_resources_current_reply_t *res
        = xcb_randr_get_screen_resources_current_reply(conn, rcookie,NULL);
    if (!res)
        return;
    xcb_timestamp_t timestamp = res->config_timestamp;
    int len = xcb_randr_get_screen_resources_current_outputs_length(res);
    xcb_randr_output_t *outputs
        = xcb_randr_get_screen_resources_current_outputs(res);
    xcb_randr_get_crtc_info_cookie_t icookie;
    xcb_randr_get_crtc_info_reply_t *crtc = NULL;
    xcb_randr_get_output_info_reply_t *output = NULL;
    monitor_t *mon = NULL;
    node_t *node = NULL;
    client_t *client = NULL;
    xcb_randr_get_output_info_cookie_t ocookie[len];
    xcb_window_t win;
    /* Loop through all outputs. active monitor means out->crtc != 0 */
    for (int i=0; i < len; i++,delocate(output)){
        ocookie[i] = xcb_randr_get_output_info(conn, outputs[i],timestamp);
        if (!(output = xcb_randr_get_output_info_reply(conn, ocookie[i], NULL)))
            continue;
        printf("%s crtc: %d monid: %d\n",__func__,output->crtc,outputs[i]);
        icookie = xcb_randr_get_crtc_info(conn, output->crtc,timestamp);
        if(!(crtc = xcb_randr_get_crtc_info_reply(conn, icookie, NULL)))
            continue;
        printf("%s x: %d y: %d width: %d height: %d\n",__func__,
            crtc->x, crtc->y, crtc->width,crtc->height);
        addmonitor(outputs[i],crtc);
        delocate(crtc);
    }
    delocate(res);
   debug2(__func__,"ended");
}
void
arrbymon(monitor_t *monitor)
{
    client_t *client;
    node_t *node = NULL;
    if((node = path[winpi]))
        return;
    do{
        if ((client = node->data) && client->monitor == monitor)
            fitonscreenwin(client);
    }while((node = next(node,winpi)) && node != path[winpi]);
}

monitor_t *
findmonitor(xcb_randr_output_t id)
{
    debug2(__func__,"start");
    monitor_t *mon = NULL;
    node_t *node = NULL;
    if(!(node = path[monpi]))
        return NULL;
    do{
        if ((mon = node->data) && mon->id == id)
            return mon;
    }while((node = next(node,monpi)) && node != path[monpi]);
    debug2(__func__,"ended");
    return NULL;
}
monitor_t *
findmonclones(xcb_randr_output_t id, const int16_t x, const int16_t y)
{
    monitor_t *clonemon = NULL;
    node_t *node = NULL;
   if(!(node = path[monpi]))
        return NULL;
    do{
        /* Check for same position. */
        if ((clonemon = node->data) && id != clonemon->id &&
            clonemon->x == x && clonemon->y == y)
            return clonemon;
    }while((node = next(node,monpi)) && node != path[monpi]);
    return NULL;
}
// this will benefit primary monitor, due to client width often
// between primary monitor and secound monitor x positions.
monitor_t *
findmonbycoord(const int16_t x, const int16_t y)
{
    monitor_t *mon = NULL;
    node_t *node = NULL;
    if(!(node = path[monpi]))
        return NULL;
    do{
        if(!(mon = node->data))
            continue;
        if ( x >= mon->x
            && x <= mon->x + mon->width
            && y >= mon->y
            && y <= mon->y+mon->height){
            return mon;
        }
    }while((node = next(node,monpi)) && node != path[monpi]);
    return NULL;
}
monitor_t *
addmonitor(xcb_randr_output_t id,xcb_randr_get_crtc_info_reply_t *crtc)
{
    node_t *node = NULL;
    monitor_t *mon = NULL;
    if(!(node = addnode(sizeof(monitor_t))))
        return NULL;
    mon = node->data;
    mon->node = node;
    mon->id = id;
    mon->name = NULL;
    mon->x = crtc->x;
    mon->y = crtc->y;
    mon->width  = crtc->width;
    mon->height = crtc->height;
    // increment monitor index.
    incmoni;
    // link to monitor path.
    linknode(monpi,node);
    return mon;
}

/* Raise window win to top of stack. */
void
raisewindow(xcb_drawable_t win)
{
    uint32_t values[] = { XCB_STACK_MODE_ABOVE };

    if (screen->root == win || 0 == win)
        return;

    xcb_configure_window(conn, win, XCB_CONFIG_WINDOW_STACK_MODE, values);
    xcb_flush(conn);
}

/* Set window client to either top or bottom of stack depending on
 * where it is now. */
void
raiseorlowerwin()
{
    uint32_t values[] = { XCB_STACK_MODE_OPPOSITE };
    client_t *client = NULL;
    if (!path[curpi])
        return;
    client = path[curpi]->data;
    xcb_configure_window(conn, client->id,XCB_CONFIG_WINDOW_STACK_MODE,
            values);
    xcb_flush(conn);
}

/* Keep the window inside the screen */
void
movelimwin(client_t *client)
{
    int16_t mon_y, mon_x,temp=0;
    uint16_t mon_height, mon_width;

    getmonsize(true, &mon_x, &mon_y, &mon_width, &mon_height, client);
    noborderwin(&temp, client, true);

    /* Is it outside the physical monitor or close to the side? */
    if (client->y-conf.borderwidth < mon_y)
        client->y = mon_y;
    else if (client->y < borders[2] + mon_y)
        client->y = mon_y;
    else if (client->y + client->height + (conf.borderwidth * 2) > mon_y
            + mon_height - borders[2])
        client->y = mon_y + mon_height - client->height
            - conf.borderwidth * 2;

    if (client->x < borders[2] + mon_x)
        client->x = mon_x;
    else if (client->x + client->width + (conf.borderwidth * 2)
            > mon_x + mon_width - borders[2])
        client->x = mon_x + mon_width - client->width
            - conf.borderwidth * 2;

    if (client->y + client->height > mon_y + mon_height
            - conf.borderwidth * 2)
        client->y = (mon_y + mon_height - conf.borderwidth * 2)
            - client->height;

    movewindow(client->id, client->x, client->y);
    noborderwin(&temp, client, false);
}

void
movewindow(xcb_drawable_t win, const int16_t x, const int16_t y)
{                                    // Move window win to root coordinates x,y.
    uint32_t values[2] = {x, y};

    if (screen->root == win || 0 == win)
        return;

    xcb_configure_window(conn, win, XCB_CONFIG_WINDOW_X
            | XCB_CONFIG_WINDOW_Y, values);

    xcb_flush(conn);
}
/* singel/multi-select windows for moving, resizing, teleporting. */
void
selectwindows(const Arg *arg)
{
    node_t *node = path[curpi];
    if(!node)
        return;
    if(next(node,selpi)){
        unlinknode(selpi,node);
        setborderswin(node->data,true);
        debug2(__func__,"unselected");
        return;
    }
    linknode(selpi,node);
    setborderswin(node->data,true);
    debug2(__func__,"selected");
    return;
}
void
selectallwindows(const Arg *arg)
{
       client_t *cl = NULL;
    node_t *node = NULL;
    for(int i = wspi(curwsi); i < wspi(curwsi) + maxloci;i++){
        if(!(node = path[i]))
            continue;
        do{
            if(next(node,selpi)){
                unlinknode(selpi,node);
                setborderswin(node->data,true);
                continue;
            }
            linknode(selpi,node);
            setborderswin(node->data,true);
        }while((node = next(node,i)) && node != path[i]);
    }
}
void
focusnextwin()
{
    client_t *cl = NULL;
    node_t *node = NULL;
    // looking for next header.
    for(int i = wspi(curwsi); i < wspi(curwsi) + maxloci; i++){
        if(i == curpi || !(node = path[i]))
            continue;
        cl = node->data;
        raisewindow(cl->id);
        setfocuswin(cl);
        return;
    }
    // due to lost headers, looking for any client in current workspace
    // inside winpi path.
    if(!(node = path[winpi]))
        return;
    do{
        if(wsi(node->dimpi) != curwsi || next(node,icopi))
            continue;
        cl = node->data;
        raisewindow(cl->id);
        setfocuswin(cl);
        return;
    }while((node = next(node,winpi)) && node != path[winpi]);
}
void
focuslocationwin(const Arg *arg)
{
    client_t *cl = NULL;
    node_t *node = path[curpi];
    if(!node || node == next(node,curpi) || !(node = next(node,curpi)))
        return;
    cl = node->data;
    raisewindow(cl->id);
    setfocuswin(cl);
}
void
setfocuswin(client_t *client)
{
    long data[] = { XCB_ICCCM_WM_STATE_NORMAL, XCB_NONE };
    // focus on pointer.
    if (!client) {
        debug2(__func__,"focus on root win");
        xcb_set_input_focus(conn, XCB_NONE,
                XCB_INPUT_FOCUS_POINTER_ROOT, XCB_CURRENT_TIME);
        xcb_window_t no_win = 0;
        xcb_change_property(conn, XCB_PROP_MODE_REPLACE, screen->root,
                ewmh->_NET_ACTIVE_WINDOW, XCB_ATOM_WINDOW, 32, 1,
                &no_win);
        xcb_flush(conn);
        return;
    }

    /* Don't bother focusing on the root window or on the same window
     * that already has focus. */
    if (client->id == screen->root)
        return;
    xcb_change_property(conn, XCB_PROP_MODE_REPLACE, client->id,
            ewmh->_NET_WM_STATE, ewmh->_NET_WM_STATE, 32, 2, data);
    xcb_set_input_focus(conn, XCB_INPUT_FOCUS_POINTER_ROOT, client->id,
            XCB_CURRENT_TIME); /* Set new input focus. */
    xcb_change_property(conn, XCB_PROP_MODE_REPLACE, screen->root,
            ewmh->_NET_ACTIVE_WINDOW, XCB_ATOM_WINDOW, 32, 1,&client->id);
    /* Remember the new window as the current focused window. */
    grabbuttons(client);
    setborderswin(client,true);
    if(path[wsfocpi] && path[wsfocpi]->data != client)
        setborderswin(path[wsfocpi]->data,false);
    // change curpi and path head to pointing to client.
    if(wsi(client->winnode->dimpi) == curwsi && !next(client->winnode,icopi)){
        if(loci(client->winnode->dimpi) != curloci)
            incloci;
        path[wsfocpi] = path[curpi] = client->winnode;
    printf("%s rechead:%p curpi:%d wsofcpi%d\n",__func__,(void*)path[recpi],curpi,wsfocpi);
    }
    debug(curpi,client->winnode,__func__,"");
}
/* Find client with client->id win in global window list or NULL. */
client_t *
findclient(int headpi,const xcb_drawable_t *win)
{
    client_t *client = NULL;
    node_t *node = NULL;
    int pi = 0;
    if(!(node = path[headpi]))
        return NULL;
    if(propi(headpi)){
        do{
            if(!(client = node->data) || client->id != *win)
                continue;
            printf("%s id: %d\n",__func__,*win);
            debug(headpi,node,__func__,"");
            return client;
        }while((node = next(node,headpi)) && node != path[headpi]);
        return NULL;
    }
    // go through worksapce and location.
    for(int i = wspi(headpi); i < wspi(headpi) + maxloci;i++){
        // through location
        if(!(node = path[i]))
            continue;
        do{
            if(!(client = node->data) || client->id != *win)
                continue;
            printf("%s id: %d\n",__func__,*win);
            debug(dimpi,node,__func__,"");
            return client;
        }while((node = next(node,i)) && node != path[i]);
    }
    return NULL;
}
void
start(const Arg *arg)
{
    if (fork())
        return;

//    if (conn)
//        close(screen->root);

    setsid();
    execvp((char*)arg->com[0], (char**)arg->com);
}

/* Resize with limit. */
void
resizelimwin(client_t *client)
{
    int16_t mon_x, mon_y,temp=0;
    uint16_t mon_width, mon_height;

    getmonsize(true,  &mon_x, &mon_y, &mon_width, &mon_height, client);
    noborderwin(&temp, client, true);

    /* Is it smaller than it wants to  be? */
    if (0 != client->min_height && client->height < client->min_height)
        client->height = client->min_height;
    if (0 != client->min_width && client->width < client->min_width)
        client->width = client->min_width;
    if (client->x + client->width + conf.borderwidth  > mon_x + mon_width)
        client->width = mon_width - ((client->x - mon_x)
                + conf.borderwidth * 2);
    if (client->y + client->height + conf.borderwidth > mon_y + mon_height)
        client->height = mon_height - ((client->y - mon_y)
                + conf.borderwidth * 2);

    resizewin(client->id, client->width, client->height);
    noborderwin(&temp, client, false);
}

void
moveresizewin(xcb_drawable_t win, const uint16_t x, const uint16_t y,
        const uint16_t width, const uint16_t height)
{
    uint32_t values[4] = { x, y, width, height };

    if (screen->root == win || 0 == win)
        return;

    xcb_configure_window(conn, win, XCB_CONFIG_WINDOW_X
            | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH
            | XCB_CONFIG_WINDOW_HEIGHT, values);

    xcb_flush(conn);
}

/* Resize window win to width,height. */
void
resizewin(xcb_drawable_t win, const uint16_t width, const uint16_t height)
{
    uint32_t values[2] = { width , height };

    if (screen->root == win || 0 == win)
        return;

    xcb_configure_window(conn, win, XCB_CONFIG_WINDOW_WIDTH
            | XCB_CONFIG_WINDOW_HEIGHT, values);

    xcb_flush(conn);
}

/* Resize window client in direction. */
void
resizestepwin(const Arg *arg)
{
    uint8_t stepx,stepy,cases = arg->i % 4;
    node_t *node = path[curpi];
    client_t *client = NULL;
    if (!node || !(client = node->data) || client->maxed)
        return;

    arg->i < 4 ? (stepx = stepy = movements[1])
        : (stepx = stepy = movements[0]);

    if (client->width_inc > 7 && client->height_inc > 7) {
        /* we were given a resize hint by the window so use it */
        stepx = client->width_inc;
        stepy = client->height_inc;
    }

    if (cases == TWOBWM_RESIZE_LEFT)
        client->width = client->width - stepx;
    else if (cases == TWOBWM_RESIZE_DOWN)
        client->height = client->height + stepy;
    else if (cases == TWOBWM_RESIZE_UP)
        client->height = client->height - stepy;
    else if (cases == TWOBWM_RESIZE_RIGHT)
        client->width = client->width + stepx;

    if (client->vertmaxed)
        client->vertmaxed = false;
    if (client->hormaxed)
        client->hormaxed  = false;

    resizelimwin(client);
// too much gliches when resizing.
// using focusnextwin after reizing to redraw boders.
//    raisewindow(client->id);
//    setborderswin(client,true);
}

/* Resize window and keep it's aspect ratio (exponentially grow),
 * and round the result (+0.5) */
void
resizestepwin_aspect(const Arg *arg)
{
    node_t *node = path[curpi];
    client_t *client = NULL;
    if (!node || !(client = node->data) || client->maxed)
        return;
    if (arg->i == TWOBWM_RESIZE_KEEP_ASPECT_SHRINK) {
        client->width = (client->width / resize_keep_aspect_ratio)
            +0.5;
        client->height = (client->height / resize_keep_aspect_ratio)
            +0.5;
    } else {
        client->height = (client->height * resize_keep_aspect_ratio)
            +0.5;
        client->width = (client->width * resize_keep_aspect_ratio)
            +0.5;
    }

    if (client->vertmaxed)
        client->vertmaxed = false;
    if (client->hormaxed)
        client->hormaxed  = false;

    resizelimwin(client);
//    raisewindow(client->id);
//    setborderswin(client,true);
}

/* Try to snap to other windows and monitor border */
static void
snapwindow(client_t *client)
{
    node_t *node = NULL;
    client_t *win = NULL;
    int16_t mon_x, mon_y;
    uint16_t mon_width, mon_height;
    if(!(node = path[curpi]))
        return;
    getmonsize(true, &mon_x,&mon_y,&mon_width,&mon_height,node->data);
    do{
        if (!(win = node->data) || client == win)
           continue;
        if (abs((win->x +win->width) - client->x
                + conf.borderwidth) < borders[2])
            if (client->y + client->height > win->y
                    && client->y < win->y
                    + win->height)
                client->x = (win->x + win->width)
                    + (2 * conf.borderwidth);

        if (abs((win->y + win->height) - client->y
                + conf.borderwidth)
                < borders[2])
            if (client->x + client->width >win->x
                    && client->x < win->x
                    + win->width)
                client->y = (win->y + win->height)
                    + (2 * conf.borderwidth);

        if (abs((client->x + client->width) - win->x
                + conf.borderwidth)
                < borders[2])
            if (client->y + client->height > win->y
                    && client->y < win->y
                    + win->height)
                client->x = (win->x - client->width)
                    - (2 * conf.borderwidth);

        if (abs((client->y + client->height) - win->y
                + conf.borderwidth)
                < borders[2])
            if (client->x + client->width >win->x
                    && client->x < win->x
                    + win->width)
                client->y = (win->y - client->height)
                    - (2 * conf.borderwidth);
    }while((node = next(node,curpi)) && node != path[curpi]);
}

/* Move window win as a result of pointer motion to coordinates rel_x,rel_y. */
void
mousemove(const int16_t rel_x, const int16_t rel_y)
{
//    debug2(__func__,"");
    node_t *node = path[curpi];
    client_t *client = NULL;
    if (!node)
        return;
    client = node->data;
    client->x = rel_x;
    client->y = rel_y;
    if (borders[2] > 0)
        snapwindow(client);
    movelimwin(client);
}

static void
mousemotion(const Arg *arg)
{
    debug2(__func__,"");
    int16_t mx=0, my=0, winx=0, winy=0, winw=0, winh=0;
    xcb_query_pointer_reply_t *pointer = NULL;
    xcb_grab_pointer_reply_t  *grab_reply = NULL;
    xcb_motion_notify_event_t *ev = NULL;
    xcb_generic_event_t *e  = NULL;
    bool ungrab = false;
    node_t *node = path[curpi];
    client_t *client = NULL;
    if (!node)
        return;
    client = node->data;
    pointer=xcb_query_pointer_reply(conn,xcb_query_pointer(conn,screen->root),0);
    if (!pointer || client->maxed) {
        delocate(pointer);
        return;
    }
    mx   = pointer->root_x;
    my   = pointer->root_y;
    winx = client->x;
    winy = client->y;
    winw = client->width;
    winh = client->height;
    xcb_cursor_t cursor;
    client_t example;
    raisewindow(client->id);
    if(arg->i == TWOBWM_MOVE)
        cursor = create_font_cursor (conn, 52 ); /* fleur */
    else {
        cursor  = create_font_cursor (conn, 120); /* sizing */
        example = create_back_win();
        xcb_map_window(conn,example.id);
    }
    grab_reply = xcb_grab_pointer_reply(conn, xcb_grab_pointer(conn, 0,
            screen->root, BUTTONMASK | XCB_EVENT_MASK_BUTTON_MOTION
            | XCB_EVENT_MASK_POINTER_MOTION, XCB_GRAB_MODE_ASYNC,
            XCB_GRAB_MODE_ASYNC, XCB_NONE, cursor, XCB_CURRENT_TIME)
            , NULL
    );
    if (grab_reply->status != XCB_GRAB_STATUS_SUCCESS) {
        delocate(grab_reply);
        if (arg->i == TWOBWM_RESIZE)
            xcb_unmap_window(conn,example.id);
        return;
    }
    delocate(grab_reply);
    ungrab = false;
    do {
        if (NULL!=e)
            delocate(e);
        while(!(e = xcb_wait_for_event(conn)))
            xcb_flush(conn);
        switch (e->response_type & ~0x80)
        {
            case XCB_CONFIGURE_REQUEST:
            case XCB_MAP_REQUEST:
                events[e->response_type & ~0x80](e);
                break;
            case XCB_MOTION_NOTIFY:
                ev = (xcb_motion_notify_event_t*)e;
                if (arg->i == TWOBWM_MOVE)
                    mousemove(winx + ev->root_x - mx,
                            winy + ev->root_y-my);
                else
                    mouseresize(&example, winw + ev->root_x - mx,
                            winh + ev->root_y - my);
                xcb_flush(conn);
                break;
            case XCB_KEY_PRESS:
            case XCB_KEY_RELEASE:
            case XCB_BUTTON_PRESS:
            case XCB_BUTTON_RELEASE:
                if (arg->i==TWOBWM_RESIZE) {
                    ev = (xcb_motion_notify_event_t*)e;
                    mouseresize(client, winw + ev->root_x - mx,
                            winh + ev->root_y - my);
                    setborderswin(client,true);
                }
                ungrab = true;
                break;
        }
    } while (!ungrab && client != NULL);
    delocate(pointer);
    delocate(e);
    xcb_free_cursor(conn,cursor);
    xcb_ungrab_pointer(conn, XCB_CURRENT_TIME);
    if (arg->i == TWOBWM_RESIZE){
        xcb_unmap_window(conn,example.id);
    }
    xcb_flush(conn);
}
void
mouseresize(client_t *client, const int16_t rel_x, const int16_t rel_y)
{
//    debug2(__func__,"");
    node_t *node = path[curpi];
    client_t *focuswin = NULL;
    if (!node)
        return;
    focuswin = node->data;
    if(focuswin->id == screen->root || focuswin->maxed)
        return;
    client->width  = abs(rel_x);
    client->height = abs(rel_y);

    if (resize_by_line) {
        client->width -= (client->width - client->base_width)
            % client->width_inc;
        client->height -= (client->height - client->base_height)
            % client->height_inc;
    }

    resizelimwin(client);
    client->vertmaxed = false;
    client->hormaxed  = false;
}
void
movestepwin(const Arg *arg)
{
    int16_t start_x, start_y;
    uint8_t step, cases=arg->i;
    node_t *node = path[curpi];
    client_t *client = NULL;
    if (!node || !(client = node->data) || client->maxed)
        return;
    /* Save pointer position so we can warp pointer here later. */
    if (!getpointer(&client->id, &start_x, &start_y))
        return;

    cases = cases%4;
    arg->i < 4 ? (step = movements[1]) : (step = movements[0]);

    if (cases == TWOBWM_MOVE_LEFT)
        client->x = client->x - step;
    else if (cases == TWOBWM_MOVE_DOWN)
        client->y = client->y + step;
    else if (cases == TWOBWM_MOVE_UP)
        client->y = client->y - step;
    else if (cases == TWOBWM_MOVE_RIGHT)
        client->x = client->x + step;
    raisewindow(client->id);
    movelimwin(client);
    movepointerback(start_x,start_y,client);
    xcb_flush(conn);
}
inline void
setborderswin(client_t *client,const bool isitfocused)
{
    uint32_t values[1];  /* this is the color maintainer */
    uint16_t half = 0;
    bool inv = conf.inverted_colors;
//    debug2(__func__,"start");
    if (client->maxed || client->ignore_borders)
        return;
    /* Set border width. */
    values[0] = conf.borderwidth;
    xcb_configure_window(conn, client->id,XCB_CONFIG_WINDOW_BORDER_WIDTH, values);
    if (top_win!=0 &&client->id ==top_win)
        inv = !inv;
    half = conf.outer_border;
    xcb_rectangle_t rect_inner[] = {
        {
            client->width,
            0,
            conf.borderwidth - half,
            client->height + conf.borderwidth - half
        },
        {
            client->width + conf.borderwidth + half,
            0,
            conf.borderwidth - half,
            client->height + conf.borderwidth - half
        },
        {
            0,
            client->height,
            client->width + conf.borderwidth - half,
            conf.borderwidth - half
        },
        {
            0,
            client->height + conf.borderwidth + half,
            client->width + conf.borderwidth - half,
            conf.borderwidth - half
        },
        {
            client->width + conf.borderwidth + half,
            conf.borderwidth + client->height + half,
            conf.borderwidth,
            conf.borderwidth
        }
    };

    xcb_rectangle_t rect_outer[] = {
        {
            client->width + conf.borderwidth - half,
            0,
            half,
            client->height + conf.borderwidth * 2
        },
        {
            client->width + conf.borderwidth,
            0,
            half,
            client->height + conf.borderwidth * 2
        },
        {
            0,
            client->height + conf.borderwidth - half,
            client->width + conf.borderwidth * 2,
            half
        },
        {
            0,
            client->height + conf.borderwidth,
            client->width + conf.borderwidth * 2,
            half
        },
        {
            1,1,1,1
        }
    };

    xcb_pixmap_t pmap = xcb_generate_id(conn);
    xcb_create_pixmap(conn, client->depth, pmap, client->id,
            client->width + (conf.borderwidth * 2),
            client->height + (conf.borderwidth * 2)
    );

    xcb_gcontext_t gc = xcb_generate_id(conn);
    xcb_create_gc(conn, gc, pmap, 0, NULL);

    if (inv) {
        xcb_rectangle_t fake_rect[5];

        for (uint8_t i=0;i<5;i++) {
            fake_rect[i]  = rect_outer[i];
            rect_outer[i] = rect_inner[i];
            rect_inner[i] = fake_rect[i];
        }
    }
    uint8_t base = 0;
    uint32_t subsetcolors[8] = {
                                    conf.outer_border_col,
                                    conf.fixedcol,
                                    conf.unkillcol,
                                    conf.fixed_unkil_col,
                                    conf.selected_col,
                                    conf.selected_col,
                                    conf.selected_col,
                                    conf.selected_col
                                };
    values[0] = subsetcolors[((base |
    (bool)next(client->winnode,selpi) << 1 |
    (bool)next(client->winnode,unkpi) << 1 |
    (bool)next(client->winnode,fixpi)))];
    xcb_change_gc(conn, gc, XCB_GC_FOREGROUND, &values[0]);
    xcb_poly_fill_rectangle(conn, pmap, gc, 5, rect_outer);

    values[0]   = conf.focuscol;

    if (!isitfocused)
        values[0] = conf.unfocuscol;

    xcb_change_gc(conn, gc, XCB_GC_FOREGROUND, &values[0]);
    xcb_poly_fill_rectangle(conn, pmap, gc, 5, rect_inner);
    values[0] = pmap;
    xcb_change_window_attributes(conn,client->id, XCB_CW_BORDER_PIXMAP,&values[0]);

    /* free the memory we allocated for the pixmap */
    xcb_free_pixmap(conn,pmap);
    xcb_free_gc(conn,gc);
    xcb_flush(conn);
//    debug2(__func__,"ended");
}

void
unmaxwin_helper(client_t *client)
{
    uint32_t values[5], mask = 0;

    if (NULL == client)
        return;

    client->x = client->origsize.x;
    client->y = client->origsize.y;
    client->width = client->origsize.width;
    client->height = client->origsize.height;

    /* Restore geometry. */
    values[0] = client->x;
    values[1] = client->y;
    values[2] = client->width;
    values[3] = client->height;

    client->maxed = client->hormaxed = 0;
    moveresizewin(client->id, client->x, client->y,
            client->width, client->height);

    centerpointer(client->id,client);
    setborderswin(client,true);
}

void
maximizewin(const Arg *arg)
{
    if(!path[curpi])
        return;
    maxwin(path[curpi]->data, true);
}

void
fullscreenwin(const Arg *arg)
{
    if(!path[curpi])
        return;
    maxwin(path[curpi]->data, false);
}


void
unmaxwin(client_t *client){
    unmaxwin_helper(client);
    client->maxed = false;
    setborderswin(client,true);
    xcb_change_property(conn, XCB_PROP_MODE_REPLACE, client->id,
            ewmh->_NET_WM_STATE, XCB_ATOM_ATOM, 32, 0, NULL);
}

void 
maxwin(client_t *client, bool with_offsets)
{
    uint32_t values[4];
    int16_t mon_x, mon_y;
    int16_t mon_width, mon_height;

    if (!client)
        return;
    /* Check if maximized already. If so, revert to stored geometry. */
    if (client->maxed) {
        unmaxwin(client);
        return;
    }
    getmonsize(with_offsets, &mon_x, &mon_y, &mon_width, &mon_height, client);
    maximizewin_helper(client, mon_x, mon_y, mon_width, mon_height);
    raise_current_window();
    if (!with_offsets) {
        xcb_change_property(conn, XCB_PROP_MODE_REPLACE, client->id,
            ewmh->_NET_WM_STATE, XCB_ATOM_ATOM, 32, 1,
            &ewmh->_NET_WM_STATE_FULLSCREEN);
    }
    xcb_flush(conn);
}

void
maxvertwin_hor(const Arg *arg)
{
    uint32_t values[2];
    int16_t mon_y, mon_x, temp = 0;
    uint16_t mon_height, mon_width;
    node_t *node = path[curpi];
    client_t *client = NULL;
    if (!node)
        return;
    client = node->data;
    if (client->vertmaxed || client->hormaxed) {
        unmaxwin(client);
        client->vertmaxed = client->hormaxed = false;
        fitonscreenwin(client);
        setborderswin(client, true);
        return;
    }

    getmonsize(true, &mon_x,&mon_y,&mon_width,&mon_height,client);
    saveorigsize(client);
    noborderwin(&temp, client,true);

    if (arg->i==TWOBWM_MAXIMIZE_VERTICALLY) {
        client->y = mon_y;
        /* Compute new height considering height increments
         * and screen height. */
        client->height = mon_height - (conf.borderwidth * 2);

        /* Move to top of screen and resize. */
        values[0] = client->y;
        values[1] = client->height;

        xcb_configure_window(conn, client->id, XCB_CONFIG_WINDOW_Y
                | XCB_CONFIG_WINDOW_HEIGHT, values);

        client->vertmaxed = true;
    } else if (arg->i==TWOBWM_MAXIMIZE_HORIZONTALLY) {
        client->x = mon_x;
        client->width = mon_width - (conf.borderwidth * 2);
        values[0] = client->x;
        values[1] = client->width;

        xcb_configure_window(conn, client->id, XCB_CONFIG_WINDOW_X
                | XCB_CONFIG_WINDOW_WIDTH, values);

        client->hormaxed = true;
    }

    noborderwin(&temp, client,false);
    raise_current_window();
    centerpointer(client->id,client);
    setborderswin(client,true);
}

void
maxhalfwin(const Arg *arg)
{
    uint32_t values[4];
    int16_t mon_x, mon_y, temp=0;
    uint16_t mon_width, mon_height;
    node_t *node = path[curpi];
    client_t *client = NULL;
    if (!node || !(client = node->data) || client->maxed)
        return;

    getmonsize(true, &mon_x,&mon_y,&mon_width,&mon_height,client);
    noborderwin(&temp, client, true);

    if (arg->i>4) {
        if (arg->i>6) {
            /* in folding mode */
            if (arg->i == TWOBWM_MAXHALF_FOLD_VERTICAL)
                client->height = client->height / 2
                    - (conf.borderwidth);
            else
                client->height = client->height * 2
                    + (2*conf.borderwidth);
        } else {
            client->y      =  mon_y;
            client->height =  mon_height - (conf.borderwidth * 2);
            client->width  = ((float)(mon_width) / 2)
                - (conf.borderwidth * 2);

            if (arg->i== TWOBWM_MAXHALF_VERTICAL_LEFT)
                client->x = mon_x;
            else
                client->x = mon_x + mon_width
                    - (client->width
                    + conf.borderwidth * 2);
        }
    } else {
        if (arg->i < 2) {
            /* in folding mode */
            if (arg->i == TWOBWM_MAXHALF_FOLD_HORIZONTAL)
                client->width = client->width / 2
                    - conf.borderwidth;
            else
                client->width = client->width * 2
                    + (2 * conf.borderwidth); //unfold
        } else {
            client->x     =  mon_x;
            client->width =  mon_width - (conf.borderwidth * 2);
            client->height = ((float)(mon_height) / 2)
                - (conf.borderwidth * 2);

            if (arg->i == TWOBWM_MAXHALF_HORIZONTAL_TOP)
                client->y = mon_y;
            else
                client->y = mon_y + mon_height
                    - (client->height
                    + conf.borderwidth * 2);
        }
    }

    moveresizewin(client->id, client->x, client->y,
            client->width, client->height);

    client->verthor = true;
    noborderwin(&temp, client, false);
    raise_current_window();
    fitonscreenwin(client);
    centerpointer(client->id, client);
    setborderswin(client, true);
}
void
unhidewin(void)
{
    node_t *tail,*node = NULL;
    client_t *client = NULL;
    if(!(node = path[icopi]))
        return;
    debug2(__func__,"start");
    for(int i = length[icopath]; i >= 0; i--){
        tail = node;
        client = tail->data;
        node = next(node,icopi);
        if(wsi(tail->dimpi) != curwsi)
            continue;
        addtoworkspace(client,curwsi);
        xcb_map_window(conn,client->id);
        unhidewin_helper(client);
        unlinknode(icopi,tail);
    }
    setfocuswin(client);
    debug2(__func__,"ended");
}
void
unhidewin_helper(client_t *client)
{
    long data[] = {
        XCB_ICCCM_WM_STATE_NORMAL,
        XCB_NONE,
    };
    xcb_change_property(conn, XCB_PROP_MODE_REPLACE, client->id,
        ewmh->_NET_WM_STATE, ewmh->_NET_WM_STATE, 32, 3, data);
    xcb_flush(conn);
}
void
hidewin(void)
{
    node_t *tail,*node = path[curpi];
    client_t *client = NULL;
    if(!node)
        return;
    debug2(__func__,"start");
    if(!next(node,icopi))
        linknode(selpi,node);
    for(node = path[selpi];node;
        tail = node,node = next(node,selpi),unlinknode(selpi,tail)){
        client = node->data;
        linknode(icopi,node);
        unlinknode(node->dimpi,node);
        xcb_unmap_window(conn,client->id);
        hidewin_helper(client);
    }
    focusnextwin();
    debug2(__func__,"ended");
}
void
hidewin_helper(client_t *client)
{
    long data[] = {
        XCB_ICCCM_WM_STATE_ICONIC,
        ewmh->_NET_WM_STATE_HIDDEN,
        XCB_NONE
    };
    xcb_change_property(conn, XCB_PROP_MODE_REPLACE, client->id,
        ewmh->_NET_WM_STATE, ewmh->_NET_WM_STATE, 32, 3, data);
    xcb_flush(conn);
}

bool
getpointer(const xcb_drawable_t *win, int16_t *x, int16_t *y)
{
    xcb_query_pointer_reply_t *pointer = xcb_query_pointer_reply(conn,xcb_query_pointer(conn, *win), 0);
    if (!pointer)
        return false;
    *x = pointer->win_x;
    *y = pointer->win_y;
    delocate(pointer);
    return true;
}

bool
getgeom(const xcb_drawable_t *win, int16_t *x, int16_t *y, uint16_t *width,
        uint16_t *height, uint8_t *depth)
{
    xcb_get_geometry_reply_t *geom = 
    xcb_get_geometry_reply(conn, xcb_get_geometry(conn, *win), NULL);
    if (!geom)
        return false;
    *x = geom->x;
    *y = geom->y;
    *width = geom->width;
    *height = geom->height;
    *depth = geom->depth;
    delocate(geom);
    return true;
}
void
subwindowposition(client_t *client)
{
    debug2(__func__,"start");
    if(client->usercoord)
        return;
    xcb_window_t prop;
    xcb_get_property_cookie_t cookie;
    client_t *parent = NULL;
    cookie = xcb_icccm_get_wm_transient_for_unchecked(conn,client->id);
    if(!cookie.sequence)
        return;
    if(!xcb_icccm_get_wm_transient_for_reply(conn,cookie,&prop,NULL))
        return;
    if(!(parent = findclient(curwspi,&prop)))
        return;
    client->x = parent->x+(parent->width/2.0) - (client->width/2.0);
    client->y = parent->y+(parent->height/2.0) - (client->height/2.0);
    client->usercoord = true;
    debug2(__func__,"ended");
}
void
windowposition(client_t *client)
{
    debug2(__func__,"start");    
    if(client->usercoord)
        return;
    int16_t pointx, pointy, mon_x, mon_y, temp = 0;
    uint16_t mon_width, mon_height;
    getmonsize(true, &mon_x, &mon_y, &mon_width, &mon_height,client);
    client->x = mon_x + mon_x + mon_width - (client->width + conf.borderwidth * 2);
    client->y = mon_y + mon_height - (client->height + conf.borderwidth * 2);
    client->x /= 2;
    client->y /= 2;
    movelimwin(client);
    setborderswin(client,true);
    debug2(__func__,"ended");
}

/* resize to X paper size. */
void
windowsize(const Arg *arg)
{
    node_t *tail,*node = path[curpi];
    client_t *cl = NULL;
    int16_t pointx, pointy, mon_x, mon_y, temp = 0;
    uint16_t mon_width, mon_height;
    if(node && !next(node,selpi))
        linknode(selpi,node);
    for(node = path[selpi];node;
        tail = node,node = next(node,selpi),unlinknode(selpi,tail)){
        /* node is handled before change to next. */
        cl = node->data;
        if (cl->maxed)
            continue;
        getmonsize(true, &mon_x, &mon_y, &mon_width, &mon_height,cl);
        cl->width = windowwidth[arg->i];
        cl->height = windowheight[arg->i];
        resizelimwin(cl);
        setborderswin(cl,true);
    }
    if(!(node = path[curpi]))
        return;
    cl = node->data;
    raisewindow(cl->id);
    setfocuswin(cl);
}
void
swapwin()
{
    client_t *cl = NULL;
    node_t *node = path[curpi];
    debug2(__func__,"start");
    if(!node)
        return;
    unlinknode(curpi,node);
    linknode(locpi(stepup(curloci,maxloci)),node);
    swapwin_helper(node->data);
    cl = node->data;
    raisewindow(cl->id);
    setfocuswin(cl);
    debug2(__func__,"ended");
}
void
swapwin_helper(client_t *cl)
{
    if(cl->position == CENTER)
        return;
    node_t *node = cl->winnode;
    int16_t pointx, pointy, mon_x, mon_y, temp = 0;
    uint16_t mon_width, mon_height, center, a4width, gap = 5;
    getmonsize(true, &mon_x, &mon_y, &mon_width, &mon_height,cl);
    noborderwin(&temp, cl,true);
    cl->x = mon_x + mon_x + mon_width;
    a4width = cl->width + conf.borderwidth * 2;
    center = (cl->x - a4width) / 2;
   if(cl->position == RIGHT){
        cl->x = center - a4width / 2 - gap;
        cl->position = LEFT;
    }else{
        cl->x = center + a4width / 2 + gap;
        cl->position = RIGHT;
    } 
    movewindow(cl->id, cl->x, cl->y);
    noborderwin(&temp, cl, false);
}
void
sidebysidewin(const Arg *arg)
{
    node_t *node = NULL;
    client_t *cl = NULL;
    for(int i = wspi(curwsi); i < wspi(curwsi) + maxloci;i++){
        if(!(node = path[i]))
            continue;
        do{
            if (!(cl = node->data) || cl->maxed)
                continue;
            sidebysidewin_helper(cl);
        }while((node = next(node,i)) && node != path[i]);
    }
    if(!(node = path[curpi]))
        return;
    cl = node->data;
    raisewindow(cl->id);
    setfocuswin(cl);
}
inline void
sidebysidewin_helper(client_t *cl)
{
    node_t *node = cl->winnode;
    int16_t pointx, pointy, mon_x, mon_y, temp = 0;
    uint16_t mon_width, mon_height, center, a4width, gap = 5;
    getmonsize(true, &mon_x, &mon_y, &mon_width, &mon_height,cl);
    noborderwin(&temp, cl,true);
    cl->x = mon_x + mon_x + mon_width;
    a4width = cl->width + conf.borderwidth * 2;
    center = (cl->x - a4width) / 2;
    if(cl->position != LEFT && cl->position != RIGHT){
        if(loci(node->dimpi)){
            cl->x = center + a4width / 2 + gap;
            cl->position = RIGHT;
        }else{
            cl->x = center - a4width / 2 - gap;
            cl->position = LEFT;
        }
    }else{
        cl->x = center;
        cl->position = CENTER;
    }
    movewindow(cl->id, cl->x, cl->y);
    movelimwin(cl);
    noborderwin(&temp, cl, false);
}
void
teleportwin(const Arg *arg)
{
    node_t *tail,*node = path[curpi];
    client_t *cl = NULL;
    if(!node)
        return;
    if(!next(node,selpi))
        linknode(selpi,node);
    for(node = path[selpi];node;
        tail = node,node = next(node,selpi),unlinknode(selpi,tail)){
        teleportwin_helper(arg,node->data);
    }
    if(!(node = path[curpi]))
        return;
    setborderswin(node->data,true);
}
inline void
teleportwin_helper(const Arg *arg,client_t *client)
{
    int16_t pointx, pointy, mon_x, mon_y, temp = 0;
    uint16_t mon_width, mon_height;
    if (!client || client->maxed)
        return;
    noborderwin(&temp, client,true);
    getmonsize(true, &mon_x, &mon_y, &mon_width, &mon_height,client);
    switch(arg->i){
        case TWOBWM_TELEPORT_CENTER:
            client->x = mon_x*2 + mon_width - (client->width + conf.borderwidth * 2);
            client->y = mon_y*2 + mon_height - (client->height + conf.borderwidth* 2);
            client->y /= 2;
            client->x /= 2;
            client->position = CENTER;
            break;
        case TWOBWM_TELEPORT_TOP_LEFT:
            client->x = mon_x;
            client->y = mon_y;
            break;
        case TWOBWM_TELEPORT_BOTTOM_LEFT:
            client->x = mon_x;
            client->y = mon_y + mon_height - (client->height + conf.borderwidth* 2);
            break;
        case TWOBWM_TELEPORT_BOTTOM:
            client->y =mon_y + mon_height - (client->height + conf.borderwidth* 2);
            break;
        case TWOBWM_TELEPORT_TOP:
            client->y = conf.borderwidth * 2;
            break;
        case TWOBWM_TELEPORT_CENTER_Y:
            client->y = mon_y*2+mon_height-(client->height + conf.borderwidth* 2);
            client->y /= 2;
            break;
        case TWOBWM_TELEPORT_CENTER_X:
            client->x = mon_x*2 +mon_width - (client->width + conf.borderwidth * 2);
            client->x /= 2;
            client->position = CENTER;
            break;
        case TWOBWM_TELEPORT_TOP_RIGHT:
            client->x = mon_x + mon_width - (client->width + conf.borderwidth * 2);
            client->y = mon_y;
            break;
        case TWOBWM_TELEPORT_BOTTOM_RIGHT:
            client->x = mon_x +  mon_width - (client->width + conf.borderwidth * 2);
            client->y = mon_y + mon_height - (client->height + conf.borderwidth * 2);
            break;
    }
    movewindow(client->id, client->x, client->y);
    noborderwin(&temp, client, false);
}
inline void
killclient(client_t *client)
{
    debug(curpi,client->winnode,__func__,"");
    xcb_kill_client(conn,client->id);
}
void
deletewin(void)
{
    debug2(__func__,"start");
    client_t *client = NULL;
    int len = 0;
    node_t *tail,*node = path[curpi];
    if(!node)
        return;
    if(!next(node,selpi))
        linknode(selpi,node);
    if(!(node = path[selpi]))
        return;
    for(int i = length[selpath]; i >= 0 ; i--){
        tail = node;
        node = next(node,selpi);
        if(next(tail,unkpi)){
            unlinknode(selpi,tail);
            continue;
        }
        // recycle node is required due to destorynotify may not handle it.
        deletewin_help(tail->data);
        denode(tail);
    }
    // confirm rest of the windows for testing purpose.
//    showclientlist(recpi);
    // focus is required for storing old focus.
    focusnextwin();
    debug2(__func__,"ended");
}
inline void
deletewin_help(client_t *client)
{
    xcb_icccm_get_wm_protocols_reply_t protocols;
    xcb_get_property_cookie_t cookie;
    /* Check if WM_DELETE is supported.  */
    cookie = xcb_icccm_get_wm_protocols_unchecked(conn, client->id,
            ewmh->WM_PROTOCOLS);
    if (client->id == top_win)
        top_win = 0;
    if (!xcb_icccm_get_wm_protocols_reply(conn, cookie, &protocols, NULL)){
        killclient(client);
        return;
    }
    for (int i = 0; i < protocols.atoms_len; i++){
        if (protocols.atoms[i] != ATOM[wm_delete_window])
            continue;
        xcb_client_message_event_t ev = {
            .response_type = XCB_CLIENT_MESSAGE,
            .format = 32,
            .sequence = 0,
            .window = client->id,
            .type = ewmh->WM_PROTOCOLS,
            .data.data32 = {
                ATOM[wm_delete_window],
                XCB_CURRENT_TIME
            }
        };
        xcb_send_event(conn, false, client->id,
            XCB_EVENT_MASK_NO_EVENT, (char *)&ev);
        xcb_icccm_get_wm_protocols_reply_wipe(&protocols);
        debug2(__func__,"event send");
        return;
    }
}
/* Extent current workspace to other monitor. */
void
changescreenwin(const Arg *arg)
{
    float xpercentage, ypercentage;
    node_t *node = path[curpi];
    client_t *client = NULL;
    if (!node)
        return;
    client = node->data;
    if (!client->monitor)
        return;
    if (arg->i == TWOBWM_NEXT_SCREEN)
        node = next(client->monitor->node,monpi);
    else
        node = prev(client->monitor->node,monpi);
    xpercentage  = (float)((client->x - client->monitor->x)/(client->monitor->width));
    ypercentage  = (float)((client->y-client->monitor->y)/(client->monitor->height));
    client->monitor = node->data;
    client->x = client->monitor->width * xpercentage + client->monitor->x + 0.5;
    client->y = client->monitor->height * ypercentage + client->monitor->y+0.5;
    raisewindow(client->id);
    fitonscreenwin(client);
    movelimwin(client);
    setborderswin(client, true);
    centerpointer(client->id, client);
    debug(monpi,node,__func__,"");
}

/* Function to make the cursor move with the keyboard */
void
cursor_move(const Arg *arg)
{
    uint16_t speed = 0;
    arg->i < 4 ? (speed = movements[3]) : (speed = movements[2]);
    switch(arg->i%4){
        case TWOBWM_CURSOR_UP:
        xcb_warp_pointer(conn, XCB_NONE, XCB_NONE,0, 0, 0, 0, 0, -speed);
        break;
        case TWOBWM_CURSOR_DOWN:
        xcb_warp_pointer(conn, XCB_NONE, XCB_NONE,0, 0, 0, 0, 0, speed);
        break;
        case TWOBWM_CURSOR_RIGHT:
        xcb_warp_pointer(conn, XCB_NONE, XCB_NONE,0, 0, 0, 0, speed, 0);
        break;
        case TWOBWM_CURSOR_LEFT:
        xcb_warp_pointer(conn, XCB_NONE, XCB_NONE,0, 0, 0, 0, -speed, 0);
        break;
    }
    xcb_flush(conn);
}

/* wrapper to get xcb keysymbol from keycode */
static xcb_keysym_t
xcb_get_keysym(xcb_keycode_t keycode)
{
    xcb_key_symbols_t *keysyms = NULL;
    if (!(keysyms = xcb_key_symbols_alloc(conn)))
        return 0;
    xcb_keysym_t keysym = xcb_key_symbols_get_keysym(keysyms, keycode, 0);
    xcb_key_symbols_free(keysyms);
    return keysym;
}

void
circulaterequest(xcb_generic_event_t *ev)
{
    debug2(__func__,"start");
    xcb_circulate_request_event_t *e = (xcb_circulate_request_event_t *)ev;
    /*
     * Subwindow e->window to parent e->event is about to be restacked.
     * Just do what was requested, e->place is either
     * XCB_PLACE_ON_TOP or _ON_BOTTOM.
     */
    xcb_circulate_window(conn, e->window, e->place);
}

void
handle_keypressrequest(xcb_generic_event_t *e)
{
    xcb_key_press_event_t *ev = (xcb_key_press_event_t *)e;
    xcb_keysym_t keysym = xcb_get_keysym(ev->detail);
    for (unsigned int i = 0; i < LENGTH(keys); i++){
        if (keysym != keys[i].keysym || CLEANMASK(keys[i].mod)
            != CLEANMASK(ev->state) || !keys[i].func)
            continue;
        keys[i].func(&keys[i].arg);
        return;
    }
}

/* Helper function to configure a window. */
void
configwin(xcb_window_t win, uint16_t mask, const struct winconf *wc)
{
    uint32_t values[7];
    int8_t i = -1;

    if (mask & XCB_CONFIG_WINDOW_X) {
        mask |= XCB_CONFIG_WINDOW_X;
        i++;
        values[i] = wc->x;
    }

    if (mask & XCB_CONFIG_WINDOW_Y) {
        mask |= XCB_CONFIG_WINDOW_Y;
        i++;
        values[i] = wc->y;
    }

    if (mask & XCB_CONFIG_WINDOW_WIDTH) {
        mask |= XCB_CONFIG_WINDOW_WIDTH;
        i++;
        values[i] = wc->width;
    }

    if (mask & XCB_CONFIG_WINDOW_HEIGHT) {
        mask |= XCB_CONFIG_WINDOW_HEIGHT;
        i++;
        values[i] = wc->height;
    }

    if (mask & XCB_CONFIG_WINDOW_SIBLING) {
        mask |= XCB_CONFIG_WINDOW_SIBLING;
        i++;
        values[i] = wc->sibling;
    }

    if (mask & XCB_CONFIG_WINDOW_STACK_MODE) {
        mask |= XCB_CONFIG_WINDOW_STACK_MODE;
        i++;
        values[i] = wc->stackmode;
    }

    if (i == -1)
        return;

    xcb_configure_window(conn, win, mask, values);
    xcb_flush(conn);
}

void
configurerequest(xcb_generic_event_t *ev)
{
    debug2(__func__,"start");
    xcb_configure_request_event_t *e = (xcb_configure_request_event_t *)ev;
    client_t *client = NULL;
    struct winconf wc;
    int16_t mon_x, mon_y;
    uint16_t mon_width, mon_height;
    uint32_t values[1];
    if (!(client = findclient(curwspi,&e->window))){
        /* Unmapped window, pass all options except border width. */
        wc.x = e->x;
        wc.y = e->y;
        wc.width = e->width;
        wc.height = e->height;
        wc.sibling = e->sibling;
        wc.stackmode = e->stack_mode;
        configwin(e->window,e->value_mask,&wc);
        debug2(__func__,"unmap ended");
        return;
    }
    getmonsize(true, &mon_x, &mon_y, &mon_width, &mon_height, client);
    if (e->value_mask & XCB_CONFIG_WINDOW_WIDTH)
        if (!client->maxed && !client->hormaxed)
            client->width = e->width;
    if (e->value_mask & XCB_CONFIG_WINDOW_HEIGHT)
        if (!client->maxed && !client->vertmaxed)
            client->height = e->height;
    if (e->value_mask & XCB_CONFIG_WINDOW_X)
        if (!client->maxed && !client->hormaxed)
            client->x = e->x;
    if (e->value_mask & XCB_CONFIG_WINDOW_Y)
        if (!client->maxed && !client->vertmaxed)
        client->y = e->y;
    /* Do we really need to pass on sibling and stack mode
     * configuration? Do we want to? */
    if (e->value_mask & XCB_CONFIG_WINDOW_SIBLING) {
        values[0] = e->sibling;
        xcb_configure_window(conn, e->window,
                XCB_CONFIG_WINDOW_SIBLING,values);
    }
    if (e->value_mask & XCB_CONFIG_WINDOW_STACK_MODE) {
        values[0] = e->stack_mode;
        xcb_configure_window(conn, e->window,
                XCB_CONFIG_WINDOW_STACK_MODE,values);
    }
    /* Check if window fits on screen after resizing. */
    if (!client->maxed) {
        resizelimwin(client);
        movelimwin(client);
        fitonscreenwin(client);
    }
    setborderswin(client,true);
    debug2(__func__,"ended");
}

xcb_cursor_t
create_font_cursor(xcb_connection_t *conn, uint16_t glyph)
{
    static xcb_font_t cursor_font;
    cursor_font = xcb_generate_id (conn);
    xcb_open_font(conn, cursor_font, strlen("cursor"), "cursor");
    xcb_cursor_t cursor = xcb_generate_id (conn);
    xcb_create_glyph_cursor(conn, cursor, cursor_font, cursor_font, glyph,
            glyph + 1,0x3232, 0x3232, 0x3232, 0xeeee, 0xeeee, 0xeeec
    );
    return cursor;
}

client_t
create_back_win(void)
{
    client_t temp_win;
    uint32_t values[1] = { conf.focuscol };
    node_t *node = path[curpi];
    client_t *client = NULL;
    if (!node)
        NULL;
    client = node->data;
    temp_win.id = xcb_generate_id(conn);
    xcb_create_window(conn,
            /* depth */
            XCB_COPY_FROM_PARENT,
            /* window Id */
            temp_win.id,
            /* parent window */
            screen->root,
            /* x, y */
            client->x,
            client->y,
            /* width, height */
            client->width,
            client->height,
            /* border width */
            borders[3],
            /* class */
            XCB_WINDOW_CLASS_INPUT_OUTPUT,
            /* visual */
            screen->root_visual,
            XCB_CW_BORDER_PIXEL,
            values
    );

    if (conf.enable_compton)
    {
        values[0] = 1;
        xcb_change_window_attributes(conn, temp_win.id,XCB_BACK_PIXMAP_PARENT_RELATIVE, values);
    }
    else
    {
        values[0] = conf.unfocuscol;
        xcb_change_window_attributes(conn, temp_win.id,XCB_CW_BACK_PIXEL, values);
    }

    temp_win.x              = client->x;
    temp_win.y              = client->y;
    temp_win.width          = client->width;
    temp_win.height         = client->height;
    temp_win.width_inc      = client->width_inc;
    temp_win.height_inc     = client->height_inc;
    temp_win.base_width     = client->base_width;
    temp_win.base_height    = client->base_height;
    temp_win.monitor        = client->monitor;
    temp_win.min_height     = client->min_height;
    temp_win.min_width      = client->min_height;
    temp_win.ignore_borders = client->ignore_borders;
    temp_win.ignore_borders;
    return temp_win;
}
void
buttonpressrequest(xcb_generic_event_t *ev)
{
    debug2(__func__,"");
    xcb_button_press_event_t *e = (xcb_button_press_event_t *)ev;
    for (unsigned int i = 0; i < LENGTH(buttons); i++){
        if (!buttons[i].func || buttons[i].button != e->detail
            || CLEANMASK(buttons[i].mask) != CLEANMASK(e->state))
            continue;
        if (!path[curpi] && buttons[i].func == mousemotion)
            return;
        if (!buttons[i].root_only){
            buttons[i].func(&(buttons[i].arg));
            continue;
        }
        if (e->event == e->root && e->child == 0)
            buttons[i].func(&(buttons[i].arg));
    }
}
// mouse click, menu events.
void
clientmessage(xcb_generic_event_t *ev)
{
    debug2(__func__,"start");
    xcb_client_message_event_t *e= (xcb_client_message_event_t *)ev;
    client_t *client = NULL;
    if (e->type == ewmh->_NET_CURRENT_DESKTOP){
        debug2(__func__,"changews");
        changews_help(e->data.data32[0]);
        return;
    }
    if(!(client = findclient(winpi, &e->window))){
        debug2(__func__,"noclient.");
        return;
    }
    if ((e->type == ATOM[wm_change_state] && e->format==32
            && e->data.data32[0]==XCB_ICCCM_WM_STATE_ICONIC)
            || e->type == ewmh->_NET_ACTIVE_WINDOW)
    {
        debug2(__func__,"icopi");
        if (!next(client->winnode,icopi)){
            if (e->type == ewmh->_NET_ACTIVE_WINDOW)
                setfocuswin(client);
            else
                hidewin();
            return;
        }
        xcb_map_window(conn,client->id);
        setfocuswin(client);
        return;
    }
    if (e->type != ewmh->_NET_WM_STATE || e->format != 32){
        debug2(__func__,"no disired client.");
        return;
    }
    if(e->data.data32[1] != ewmh->_NET_WM_STATE_FULLSCREEN
        && e->data.data32[2] != ewmh->_NET_WM_STATE_FULLSCREEN){
        debug2(__func__,"new client.");
        return;
    }
    debug2(__func__,"switch");
    switch (e->data.data32[0])
    {
        case XCB_EWMH_WM_STATE_REMOVE:
            unmaxwin(client);
            break;
        case XCB_EWMH_WM_STATE_ADD:
            maxwin(client, false);
            break;
        case XCB_EWMH_WM_STATE_TOGGLE:
                if(client->maxed)
                    unmaxwin(client);
                else
                    maxwin(client, false);
            break;
        default: break;
    }
    debug2(__func__,"ended");
}
// only sub window closing will generate this.
void
destroynotify(xcb_generic_event_t *ev)
{
    debug2(__func__,"start");
    xcb_destroy_notify_event_t *e = (xcb_destroy_notify_event_t *) ev;
    client_t *cl,*client = NULL;
    // find client in win path for recycle.
    if((client = findclient(winpi,&e->window))){
        denode(client->winnode);
        if (client->id == top_win)
            top_win = 0;
        killclient(client);
        focusnextwin();
    }
    // find client in recycle path
    if((client = findclient(recpi,&e->window))){
        if (client->id == top_win)
            top_win = 0;
        killclient(client);
        focusnextwin();
    }
    debug2(__func__,"ended");
}
// generated by mouse cover any window e.g: centerpointer.
void
enternotify(xcb_generic_event_t *ev)
{
    xcb_enter_notify_event_t *e = (xcb_enter_notify_event_t *) ev;
    node_t *node = path[curpi];
    client_t *client = NULL;
    if (!node)
        return;
    if(!mousefocus)
        return;
    debug2(__func__,"start");
    /*
     * If this isn't a normal enter notify, don't bother. We also need
     * ungrab events, since these will be generated on button and key
     * grabs and if the user for some reason presses a button on the
     * root and then moves the pointer to our window and releases the
     * button, we get an Ungrab EnterNotify. The other cases means the
     * pointer is grabbed and that either means someone is using it for
     * menu selections or that we're moving or resizing. We don't want
     * to change focus in those cases.
     */

    if (e->mode != XCB_NOTIFY_MODE_NORMAL && e->mode != XCB_NOTIFY_MODE_UNGRAB)
        return;
    /* If we're entering the same window we focus now,
     * then don't bother focusing. */
    if ((client = node->data) && e->event == client->id)
        return;
    /* Otherwise, set focus to the window we just entered if we
     * can find it among the windows we know about.
     * If not, just keep focus in the old window. */
    if(!(client = findclient(winpi,&e->event)))
        return;
    setfocuswin(client);
    debug2(__func__,"ended");
}
void
unmapnotify(xcb_generic_event_t *ev)
{
    xcb_unmap_notify_event_t *e = (xcb_unmap_notify_event_t *)ev;
    client_t *client = NULL;
//    debug2(__func__,"start");
    /*
     * Find the window in our current workspace list, then forget about it.
     * Note that we might not know about the window we got the UnmapNotify
     * event for.
     * It might be a window we just unmapped on *another* workspace when
     * changing workspaces, for instance, or it might be a window with
     * override redirect set.
     * This is not an error.
     * XXX We might need to look in the global window list, after all.
     * Consider if a window is unmapped on our last workspace while
     * changing workspaces.
     * If we do this, we need to keep track of our own windows and
     * ignore UnmapNotify on them.
     */
//    if(!(client = findclient(winpi,&e->window)))
//        return;
//   debug2(__func__,"ended");
}

void
confignotify(xcb_generic_event_t *ev)
{
    xcb_configure_notify_event_t *e= (xcb_configure_notify_event_t *)ev;
    if(e->window != screen->root)
        return;
    /*
     * When using RANDR or Xinerama, the root can change geometry
     * when the user adds a new screen, tilts their screen 90
     * degrees or whatnot. We might need to rearrange windows to
     * be visible.
     * We might get notified for several reasons, not just if the
     * geometry changed.
     * If the geometry is unchanged we do nothing.
     */
    if (e->width==screen->width_in_pixels && e->height == screen->height_in_pixels)
        return;
    screen->width_in_pixels = e->width;
    screen->height_in_pixels = e->height;
    if (-1 == randrbase)
        arrangewindows();
}

void
run(void)
{
    sigcode = 0;
    while (0 == sigcode){
        /* the WM is running */
        xcb_flush(conn);
        if (xcb_connection_has_error(conn)){
            cleanup();
            abort();
        }
        if (!(ev = xcb_wait_for_event(conn)))
            continue;
        if(ev->response_type==randrbase + XCB_RANDR_SCREEN_CHANGE_NOTIFY)
            changerandr();
        if (events[ev->response_type & ~0x80])
            events[ev->response_type & ~0x80](ev);
        if(top_win!=0)
            raisewindow(top_win);
        delocate(ev);
    }
    if(sigcode != SIGHUP)
        return;
    sigcode = 0;
}

/* Get a defined atom from the X server. */
xcb_atom_t
getatom(const char *atom_name)
{
    xcb_intern_atom_cookie_t atom_cookie =
    xcb_intern_atom(conn, 0, strlen(atom_name), atom_name);
    xcb_intern_atom_reply_t *rep = xcb_intern_atom_reply(conn, atom_cookie,NULL);
    /* Note that we return 0 as an atom if anything goes wrong.
     * Might become interesting.*/
    if (NULL == rep)
        return 0;
    xcb_atom_t atom = rep->atom;
    delocate(rep);
    return atom;
}

/* set the given client to listen to button events (presses / releases) */
void
grabbuttons(client_t *c)
{
    unsigned int modifiers[] = {
        0,
        XCB_MOD_MASK_LOCK,
        numlockmask,
        numlockmask | XCB_MOD_MASK_LOCK
    };
    for (unsigned int b=0; b < LENGTH(buttons); b++){
        if (buttons[b].root_only)
            continue;
        for (unsigned int m=0; m < LENGTH(modifiers); m++){
            xcb_grab_button(conn, 1, c->id,
                    XCB_EVENT_MASK_BUTTON_PRESS,
                    XCB_GRAB_MODE_ASYNC,
                    XCB_GRAB_MODE_ASYNC,
                    screen->root, XCB_NONE,
                    buttons[b].button,
                    buttons[b].mask|modifiers[m]);
        }
    }
}

void
ewmh_init(void)
{
    alocate((void**)&ewmh,sizeof(xcb_ewmh_connection_t));
    xcb_intern_atom_cookie_t *cookie = xcb_ewmh_init_atoms(conn, ewmh);
    if(!xcb_ewmh_init_atoms_replies(ewmh, cookie, (void *)0)){
        fprintf(stderr,"%s\n","xcb_ewmh_init_atoms_replies:faild.");
        exit(1);
    }
}

bool
setup(int scrno)
{
    unsigned int i;
    uint32_t event_mask_pointer[] = { XCB_EVENT_MASK_POINTER_MOTION };

    unsigned int values[1] = {
        XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT
        | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY
        | XCB_EVENT_MASK_PROPERTY_CHANGE
        | XCB_EVENT_MASK_BUTTON_PRESS
    };
    if(!(screen = xcb_screen_of_display(conn, scrno)))
        return false;
    ewmh_init();
    xcb_ewmh_set_wm_pid(ewmh, screen->root, getpid());
    xcb_ewmh_set_wm_name(ewmh, screen->root, 4, "2bwm");

    xcb_atom_t net_atoms[] = {
        ewmh->_NET_SUPPORTED,              ewmh->_NET_WM_DESKTOP,
        ewmh->_NET_NUMBER_OF_DESKTOPS,     ewmh->_NET_CURRENT_DESKTOP,
        ewmh->_NET_ACTIVE_WINDOW,          ewmh->_NET_WM_ICON,
        ewmh->_NET_WM_STATE,               ewmh->_NET_WM_NAME,
        ewmh->_NET_SUPPORTING_WM_CHECK ,   ewmh->_NET_WM_STATE_HIDDEN,
        ewmh->_NET_WM_ICON_NAME,           ewmh->_NET_WM_WINDOW_TYPE,
        ewmh->_NET_WM_WINDOW_TYPE_DOCK,    ewmh->_NET_WM_WINDOW_TYPE_DESKTOP,
        ewmh->_NET_WM_WINDOW_TYPE_TOOLBAR, ewmh->_NET_WM_PID,
        ewmh->_NET_CLIENT_LIST,            ewmh->_NET_CLIENT_LIST_STACKING,
        ewmh->WM_PROTOCOLS,                ewmh->_NET_WM_STATE,
        ewmh->_NET_WM_STATE_DEMANDS_ATTENTION,
        ewmh->_NET_WM_STATE_FULLSCREEN
    };

    xcb_ewmh_set_supported(ewmh, scrno, LENGTH(net_atoms), net_atoms);

    xcb_xrm_database_t* db = xcb_xrm_database_from_default(conn);
    // Load the default config anyway.
    conf.borderwidth            = borders[1];
    conf.outer_border         = borders[0];
    conf.focuscol                 = getcolor(colors[0]);
    conf.unfocuscol             = getcolor(colors[1]);
    conf.fixedcol                 = getcolor(colors[2]);
    conf.unkillcol                = getcolor(colors[3]);
    conf.outer_border_col = getcolor(colors[5]);
    conf.fixed_unkil_col    = getcolor(colors[4]);
    conf.empty_col                = getcolor(colors[6]);
    conf.inverted_colors    = inverted_colors;
    conf.enable_compton     = false;
    conf.selected_col = getcolor(colors[7]);
    
    if(db)
    {
        char* value;
        if (xcb_xrm_resource_get_string(db,
        "twobwm.border_width", NULL, &value) >= 0)
            conf.borderwidth = atoi(value);

        if (xcb_xrm_resource_get_string(db,
        "twobwm.outer_border", NULL, &value) >= 0)
            conf.outer_border = atoi(value);

        if (xcb_xrm_resource_get_string(db,
        "twobwm.focus_color", NULL, &value) >= 0)
            conf.focuscol = getcolor(value);

        if (xcb_xrm_resource_get_string(db, 
        "twobwm.selected_color", NULL, &value) >= 0)
            conf.selected_col = getcolor(value);

        if (xcb_xrm_resource_get_string(db,
        "twobwm.unfocus_color", NULL, &value) >= 0)
            conf.unfocuscol = getcolor(value);

        if (xcb_xrm_resource_get_string(db,
        "twobwm.fixed_color", NULL, &value) >= 0)
            conf.fixedcol = getcolor(value);

        if (xcb_xrm_resource_get_string(db,
        "twobwm.unkill_color", NULL, &value) >= 0)
            conf.unkillcol = getcolor(value);

        if (xcb_xrm_resource_get_string(db,
        "twobwm.outer_border_color", NULL, &value) >= 0)
            conf.outer_border_col = getcolor(value);

        if (xcb_xrm_resource_get_string(db,
        "twobwm.fixed_unkill_color", NULL, &value) >= 0)
            conf.fixed_unkil_col = getcolor(value);

        if (xcb_xrm_resource_get_string(db,
        "twobwm.inverted_colors", NULL, &value) >= 0)
            conf.inverted_colors = strcmp(value, "true") == 0;

        if (xcb_xrm_resource_get_string(db,
        "twobwm.enable_compton", NULL, &value) >= 0)
            conf.enable_compton = strcmp(value, "true") == 0;
    }

    xcb_xrm_database_free(db);

    for (i=0; i<NB_ATOMS; i++)
        ATOM[i] = getatom(atomnames[i][0]);

    randrbase = setuprandr();
    if (!setupscreen())
        return false;
    changews_help(0);
    if (!setup_keyboard())
        return false;

    xcb_generic_error_t *error = xcb_request_check(conn,xcb_change_window_attributes_checked(conn, screen->root,XCB_CW_EVENT_MASK, values));
    xcb_flush(conn);

    if (error){
        fprintf(stderr,"%s\n","xcb_request_check:faild.");
        delocate(error);
        return false;
    }

    xcb_ewmh_set_current_desktop(ewmh, scrno, curwsi);
    xcb_ewmh_set_number_of_desktops(ewmh, scrno, WORKSPACES);

    grabkeys();
    /* set events */
    for (i=0; i<XCB_NO_OPERATION; i++)
        events[i] = NULL;

    events[XCB_CONFIGURE_REQUEST]   = configurerequest;
    events[XCB_DESTROY_NOTIFY]      = destroynotify;
    events[XCB_ENTER_NOTIFY]        = enternotify;
    events[XCB_KEY_PRESS]           = handle_keypressrequest;
    events[XCB_MAP_REQUEST]         = newwinrequest;
    events[XCB_UNMAP_NOTIFY]        = unmapnotify;
    events[XCB_CONFIGURE_NOTIFY]    = confignotify;
    events[XCB_CIRCULATE_REQUEST]   = circulaterequest;
    events[XCB_BUTTON_PRESS]        = buttonpressrequest;
    events[XCB_CLIENT_MESSAGE]      = clientmessage;

    return true;
}

void
twobwm_exit()
{
    exit(EXIT_SUCCESS);
}
void
sigcatch(const int sig)
{
    sigcode = sig;
}
void
install_sig_handlers(void)
{
    struct sigaction sa;
    struct sigaction sa_chld;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_NOCLDSTOP;
    //could not initialize signal handler
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
        exit(-1);
    sa.sa_handler = sigcatch;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; /* Restart if interrupted by handler */
    if ( sigaction(SIGINT, &sa, NULL) == -1
        || sigaction(SIGHUP, &sa, NULL) == -1
        || sigaction(SIGTERM, &sa, NULL) == -1)
        exit(-1);
}
int
main(int argc, char **argv)
{
    int scrno = 0;
    if(!getenv("DISPLAY")){
        fprintf(stderr,"DISPLAY %s\n",getenv("DISPLAY"));
        exit(1);
    }
    atexit(cleanup);
    install_sig_handlers();
    cube_init();
    initallpath();
    if (!xcb_connection_has_error(conn = xcb_connect(NULL, &scrno)))
        if (setup(scrno));
            run();
    exit(sigcode);
}
