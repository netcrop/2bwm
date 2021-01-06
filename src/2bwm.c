#ifndef STIMY_H
#include <stimy.h>
#endif
#include "2bwm.h"
void
fixwin()
{
    stimy_pre();
     stimy_echo(fixwindow,fixwindow(path[curpi]));
    stimy_post();
}
void
unkillablewin()
{
    stimy_pre();
     stimy_echo(unkillablewindow,unkillablewindow(path[curpi]));
    stimy_post();
}
void
delmonitor(monitor_t *mon)
{
    stimy_pre();
     stimy_echo(denode,denode(mon->node));
    stimy_post();
}
void
raise_current_window(void)
{
    stimy_pre();
    if(!path[curpi])
        stimy_post(return;);
    client_t *client = path[curpi]->data;
     stimy_echo(raisewindow,raisewindow(client->id));
    stimy_post();
}
inline void
delfromworkspace(node_t *node, uint32_t ws)
{
    stimy_pre();
     stimy_echo(printf,printf("%s nodeid: %d ws: %d wspi(ws): %d\n",__func__,node->id,ws,wspi(ws)));
    for(int i =  stimy_echo(wspi,wspi(ws)); i <  stimy_echo(wspi,wspi(ws)) + maxloci;i++){
         stimy_echo(unlinknode,unlinknode(i,node));
    }
    stimy_post();
}
void
changeworkspace(const Arg *arg)
{
    stimy_pre();
     stimy_echo(changews_help,changews_help(arg->i));
    stimy_post();
}
void
nextworkspace()
{
    stimy_pre();
    curwsi == WORKSPACES - 1 ?  stimy_echo(changews_help,changews_help(0))
        :changews_help(curwsi+1);
    stimy_post();
}
void
prevworkspace()
{
    stimy_pre();
    curwsi > 0 ?  stimy_echo(changews_help,changews_help(curwsi - 1))
        :  stimy_echo(changews_help,changews_help(WORKSPACES-1));
    stimy_post();
}
void
saveorigsize(client_t *client)
{
    stimy_pre();
    client->origsize.x      = client->x;
    client->origsize.y      = client->y;
    client->origsize.width  = client->width;
    client->origsize.height = client->height;
    stimy_post();
}
void
centerpointer(xcb_drawable_t win, client_t *cl)
{
    stimy_pre();
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
     stimy_echo(xcb_warp_pointer,xcb_warp_pointer(conn, XCB_NONE, win, 0, 0, 0, 0, cur_x, cur_y));
    stimy_post();
}
void
showclientlist(int headpi)
{
    stimy_pre();
     stimy_echo(debug2,debug2(__func__,""));
    uint32_t len = 0;
    xcb_window_t *children = NULL;
    client_t *client = NULL;
    xcb_query_tree_reply_t *reply =
     stimy_echo(xcb_query_tree_reply,xcb_query_tree_reply(conn,  stimy_echo(xcb_query_tree,xcb_query_tree(conn, screen->root)), 0));
    len =  stimy_echo(xcb_query_tree_children_length,xcb_query_tree_children_length(reply));
    children =  stimy_echo(xcb_query_tree_children,xcb_query_tree_children(reply));
    for (int i=0; i < len; i ++) {
         stimy_echo(findclient,findclient(headpi,&children[i]));
    }
     stimy_echo(delocate,delocate(reply));
    stimy_post();
}
void
updateclientlist(void)
{
    stimy_pre();
     stimy_echo(debug2,debug2(__func__,""));
    uint32_t len = 0;
    xcb_window_t *children = NULL;
    client_t *cl = NULL;
    xcb_query_tree_reply_t *reply =
     stimy_echo(xcb_query_tree_reply,xcb_query_tree_reply(conn,  stimy_echo(xcb_query_tree,xcb_query_tree(conn, screen->root)), 0));
     stimy_echo(xcb_delete_property,xcb_delete_property(conn, screen->root, ewmh->_NET_CLIENT_LIST));
     stimy_echo(xcb_delete_property,xcb_delete_property(conn, screen->root, ewmh->_NET_CLIENT_LIST_STACKING));
    if (!reply) {
         stimy_echo(addtoclientlist,addtoclientlist(0));
        stimy_post(return;);
    }
    len =  stimy_echo(xcb_query_tree_children_length,xcb_query_tree_children_length(reply));
    children =  stimy_echo(xcb_query_tree_children,xcb_query_tree_children(reply));
    for (int i=0; i < len; i ++) {
        if((cl =  stimy_echo(findclient,findclient(winpi,&children[i]))))
             stimy_echo(addtoclientlist,addtoclientlist(cl->id));
    }
     stimy_echo(delocate,delocate(reply));
    stimy_post();
}
xcb_screen_t *
xcb_screen_of_display(xcb_connection_t *con, int screen)
{
    stimy_pre();
    xcb_screen_iterator_t iter;
    iter =  stimy_echo(xcb_setup_roots_iterator,xcb_setup_roots_iterator(xcb_get_setup(con)));
    for (; iter.rem; --screen,  stimy_echo(xcb_screen_next,xcb_screen_next(&iter)))
        if (screen == 0)
            stimy_post(return iter.data;);
    stimy_post(return NULL;);
    stimy_post();
}
void
movepointerback(const int16_t startx, const int16_t starty,
        const client_t *client)
{
    stimy_pre();
    if (startx > (0 - conf.borderwidth - 1) && startx < (client->width
                + conf.borderwidth + 1) && starty > (0
                    - conf.borderwidth - 1) && starty
            < (client->height + conf.borderwidth + 1))
         stimy_echo(xcb_warp_pointer,xcb_warp_pointer(conn, XCB_NONE, client->id,0,0,0,0, startx,
                starty));
    stimy_post();
}
void
cleanup(void)
{
    stimy_pre();
     stimy_echo(delocate,delocate(ev));
     stimy_echo(delall,delall());
    if (ewmh){
         stimy_echo(xcb_ewmh_connection_wipe,xcb_ewmh_connection_wipe(ewmh));
         stimy_echo(delocate,delocate(ewmh));
    }
    if(!conn)
        stimy_post(return;);
     stimy_echo(xcb_set_input_focus,xcb_set_input_focus(conn, XCB_NONE,XCB_INPUT_FOCUS_POINTER_ROOT,
            XCB_CURRENT_TIME));
     stimy_echo(xcb_flush,xcb_flush(conn));
     stimy_echo(xcb_disconnect,xcb_disconnect(conn));
    stimy_post();
}
void
arrangewindows(void)
{
    stimy_pre();
    client_t *client;
    node_t *node = path[winpi];
    if(!node)
        stimy_post(return;);
    do{
         stimy_echo(fitonscreenwin,fitonscreenwin(node->data));
    }while((node =  stimy_echo(next,next(node,winpi))) && node != path[winpi]);
    stimy_post();
}
uint32_t getwmdesktop(xcb_drawable_t win)
{
    stimy_pre();
    xcb_get_property_cookie_t cookie =  stimy_echo(xcb_get_property,xcb_get_property(conn,
        false, win, ewmh->_NET_WM_DESKTOP,
        XCB_GET_PROPERTY_TYPE_ANY, 0, sizeof(uint32_t)));
    xcb_get_property_reply_t *reply =  stimy_echo(xcb_get_property_reply,xcb_get_property_reply(conn, cookie, NULL));
    if (NULL==reply || 0 ==  stimy_echo(xcb_get_property_value_length,xcb_get_property_value_length(reply)))
    {
        if(NULL!=reply)  stimy_echo(delocate,delocate(reply));
        stimy_post(return TWOBWM_NOWS;);
    }
    uint32_t wsp = *(uint32_t *)xcb_get_property_value(reply);
    if(NULL!=reply)
         stimy_echo(delocate,delocate(reply));
    stimy_post(return wsp;);
    stimy_post();
}
bool
get_unkil_state(xcb_drawable_t win)
{
    stimy_pre();
    xcb_get_property_cookie_t cookie;
    xcb_get_property_reply_t *reply;
    uint8_t wsp;
    cookie =  stimy_echo(xcb_get_property,xcb_get_property(conn, false, win, ewmh->_NET_WM_STATE_DEMANDS_ATTENTION,
            XCB_GET_PROPERTY_TYPE_ANY, 0,sizeof(uint8_t)));
    reply  =  stimy_echo(xcb_get_property_reply,xcb_get_property_reply(conn, cookie, NULL));
    if (reply == NULL ||  stimy_echo(xcb_get_property_value_length,xcb_get_property_value_length(reply)) == 0) {
        if(reply != NULL )
             stimy_echo(delocate,delocate(reply));
        stimy_post(return false;);
    }
    wsp = *(uint8_t *)  stimy_echo(xcb_get_property_value,xcb_get_property_value(reply));
    if (reply != NULL)
         stimy_echo(delocate,delocate(reply));
    if (wsp == 1)
        stimy_post(return true;);
    else
        stimy_post(return false;);
    stimy_post();
}
void
check_namewin(client_t *client)
{
    stimy_pre();
    xcb_get_property_reply_t *reply ;
    unsigned int reply_len;
    char *wm_name_window;
    unsigned int i;
    uint32_t values[1] = { 0 };
    if (NULL == client)
        stimy_post(return;);
    reply =  stimy_echo(xcb_get_property_reply,xcb_get_property_reply(conn,  stimy_echo(xcb_get_property,xcb_get_property(conn, false,
                client->id,  stimy_echo(getatom,getatom(LOOK_INTO)) ,
                XCB_GET_PROPERTY_TYPE_ANY, 0,60)), NULL
    ));
    if (reply == NULL ||  stimy_echo(xcb_get_property_value_length,xcb_get_property_value_length(reply)) == 0) {
        if (NULL!=reply)
             stimy_echo(delocate,delocate(reply));
        stimy_post(return;);
    }
    reply_len =  stimy_echo(xcb_get_property_value_length,xcb_get_property_value_length(reply));
     stimy_echo(alocate,alocate((void**)&wm_name_window,sizeof(char*) * (reply_len+1)));
     stimy_echo(memcpy,memcpy(wm_name_window,  stimy_echo(xcb_get_property_value,xcb_get_property_value(reply)), reply_len));
    wm_name_window[reply_len] = '\0';
    if(NULL != reply)
         stimy_echo(delocate,delocate(reply));
    for(i=0;i<sizeof(ignore_names)/sizeof(__typeof__(*ignore_names));i++)
        if (strstr(wm_name_window, ignore_names[i]) !=NULL) {
            client->ignore_borders = true;
             stimy_echo(xcb_configure_window,xcb_configure_window(conn, client->id,
                    XCB_CONFIG_WINDOW_BORDER_WIDTH, values));
            break;
        }
     stimy_echo(delocate,delocate(wm_name_window));
    stimy_post();
}
inline void
addtoworkspace(client_t *client, uint32_t ws)
{
    stimy_pre();
    if (!client)
        stimy_post(return;);
     stimy_echo(linknode,linknode(pathindex(curmoni,ws,incloci),client->winnode));
     stimy_echo(debug,debug(client->winnode->headpi,client->winnode,__func__,""));
    stimy_post();
}
static void
addtoclientlist(const xcb_drawable_t id)
{
    stimy_pre();
     stimy_echo(xcb_change_property,xcb_change_property(conn, XCB_PROP_MODE_APPEND , screen->root, ewmh->_NET_CLIENT_LIST, XCB_ATOM_WINDOW, 32, 1,&id));
     stimy_echo(xcb_change_property,xcb_change_property(conn, XCB_PROP_MODE_APPEND , screen->root, ewmh->_NET_CLIENT_LIST_STACKING, XCB_ATOM_WINDOW, 32, 1,&id));
    stimy_post();
}
void
changews_help(const uint32_t ws)
{
    stimy_pre();
    xcb_query_pointer_reply_t *pointer;
    client_t *client = NULL;
    node_t *tail,*node = NULL;
    if (ws == curwsi)
        stimy_post(return;);
     stimy_echo(debug2,debug2(__func__,"start"));
     stimy_echo(xcb_ewmh_set_current_desktop,xcb_ewmh_set_current_desktop(ewmh, 0, ws));
    if((node = path[fixpi])){
        do{
             stimy_echo(unlinknode,unlinknode(node->dimpi,node));
             stimy_echo(linknode,linknode(pathindex(curmoni,ws,loci(node->dimpi)),node));
        }while((node =  stimy_echo(next,next(node,fixpi))) && node != path[fixpi]);
    }
    for(int i =  stimy_echo(wspi,wspi(curwsi)); i <  stimy_echo(wspi,wspi(curwsi)) + maxloci;i++){
        if(!(node = path[i]))
            continue;
        do{
            client = node->data;
             stimy_echo(setborderswin,setborderswin(client,false));
             stimy_echo(xcb_unmap_window,xcb_unmap_window(conn,client->id));
             stimy_echo(debug,debug(i,node,__func__,"unmap"));
        }while((node =  stimy_echo(next,next(node,i))) && node != path[i]);
    }
    client = NULL;
    for(int i =  stimy_echo(wspi,wspi(ws)); i <  stimy_echo(wspi,wspi(ws)) + maxloci;i++){
        if(!(node = path[i]))
            continue;
        do{
            client = node->data;
             stimy_echo(xcb_map_window,xcb_map_window(conn,client->id));
             stimy_echo(debug,debug(i,node,__func__,"map"));
        }while((node =  stimy_echo(next,next(node,i))) && node != path[i]);
    }
    curwsi = ws;
    if(path[wsfocpi] && !next(path[wsfocpi],recpi)){
         stimy_echo(debug,debug(curpi,path[wsfocpi],__func__,"wsfocpi"));
         stimy_echo(setfocuswin,setfocuswin(path[wsfocpi]->data));
    }else{
         stimy_echo(setfocuswin,setfocuswin(client));
    }
     stimy_echo(debug2,debug2(__func__,"ended"));
    stimy_post();
}
void
always_on_topwin()
{
    stimy_pre();
     stimy_echo(debug2,debug2(__func__,""));
    client_t *cl = NULL;
    node_t *node = path[curpi];
    if(!node)
        stimy_post(return;);
    cl = node->data;
    if(cl->id == top_win){
        top_win = 0;
         stimy_echo(setborderswin,setborderswin(cl,true));
        stimy_post(return;);
    }
    if (top_win != 0 && (cl =  stimy_echo(findclient,findclient(curwspi,&top_win))))
         stimy_echo(setborderswin,setborderswin(cl, false));
    cl = node->data;
    top_win = cl->id;
     stimy_echo(raisewindow,raisewindow(top_win));
     stimy_echo(setborderswin,setborderswin(cl,true));
    stimy_post();
}
void
fixwindow(node_t *node)
{
    stimy_pre();
    client_t *client = NULL;
    if(!node)
        stimy_post(return;);
    if(next(node,fixpi)){
         stimy_echo(unlinknode,unlinknode(fixpi,node));
         stimy_echo(setborderswin,setborderswin(node->data,true));
        stimy_post(return;);
    }
    client = node->data;
     stimy_echo(raisewindow,raisewindow(client->id));
     stimy_echo(linknode,linknode(fixpi,node));
     stimy_echo(setborderswin,setborderswin(node->data,true));
    stimy_post();
}
void
unkillablewindow(node_t* node)
{
    stimy_pre();
    client_t *client = NULL;
    if(!node)
        stimy_post(return;);
    client = node->data;
    if (next(node,unkpi)){
         stimy_echo(unlinknode,unlinknode(unkpi,node));
         stimy_echo(xcb_delete_property,xcb_delete_property(conn, client->id, ewmh->_NET_WM_STATE_DEMANDS_ATTENTION));
         stimy_echo(setborderswin,setborderswin(client,true));
        stimy_post(return;);
    }
     stimy_echo(linknode,linknode(unkpi,node));
     stimy_echo(raisewindow,raisewindow(client->id));
     stimy_echo(xcb_change_property,xcb_change_property(conn, XCB_PROP_MODE_REPLACE, client->id,
            ewmh->_NET_WM_STATE_DEMANDS_ATTENTION, XCB_ATOM_CARDINAL, 8, 1,
             stimy_echo(next,next(node,unkpi))));
     stimy_echo(setborderswin,setborderswin(client,true));
    stimy_post();
}
void
posttoworkspace(const Arg *arg)
{
    stimy_pre();
     stimy_echo(sendtoworkspace,sendtoworkspace(arg));
     stimy_echo(changeworkspace,changeworkspace(arg));
     stimy_echo(selectallwindows,selectallwindows(arg));
    Arg arg1 = { .i=TWOBWM_TELEPORT_CENTER_X };
     stimy_echo(teleportwin,teleportwin(&arg1));
    stimy_post();
}
void
sendtoworkspace(const Arg *arg)
{
    stimy_pre();
    client_t *client = NULL;
    node_t *tail,*node = path[curpi];
    if(!node)
        stimy_post(return;);
    if(!next(node,selpi))
         stimy_echo(linknode,linknode(selpi,node));
    for(node = path[selpi];node;
        tail = node,node =  stimy_echo(next,next(node,selpi)),unlinknode(selpi,tail)){
        if(wsi(node->dimpi) == arg->i ||  stimy_echo(next,next(node,fixpi)))
            continue;
        client = node->data;
         stimy_echo(unlinknode,unlinknode(node->dimpi,node));
         stimy_echo(linknode,linknode(pathindex(curmoni,arg->i,loci(node->dimpi)),node));
         stimy_echo(xcb_unmap_window,xcb_unmap_window(conn,client->id));
         stimy_echo(setborderswin,setborderswin(client,false));
    }
     stimy_echo(focusnextwin,focusnextwin());
    stimy_post();
}
void
sendtonextworkspace(const Arg *arg)
{
    stimy_pre();
    Arg arg2 = { .i=0 };
    Arg arg3 = { .i=curwsi + 1 };
    curwsi == WORKSPACES - 1 ?  stimy_echo(sendtoworkspace,sendtoworkspace(&arg2)):sendtoworkspace(&arg3);
    stimy_post();
}
void
sendtoprevworkspace(const Arg *arg)
{
    stimy_pre();
    Arg arg2 = {.i=curwsi-1};
    Arg arg3 = {.i=WORKSPACES-1};
    curwsi > 0 ?  stimy_echo(sendtoworkspace,sendtoworkspace(&arg2)) :  stimy_echo(sendtoworkspace,sendtoworkspace(&arg3));
    stimy_post();
}
uint32_t
getcolor(const char *hex)
{
    stimy_pre();
    uint32_t rgb48;
    char strgroups[7] = {
        hex[1], hex[2], hex[3], hex[4], hex[5], hex[6], '\0'
    };
    rgb48 =  stimy_echo(strtol,strtol(strgroups, NULL, 16));
    stimy_post(return rgb48 | 0xff000000;);
    stimy_post();
}
void
getmonsize(bool with_offsets, int16_t *mon_x, int16_t *mon_y,
        uint16_t *mon_width, uint16_t *mon_height,
        const client_t *client)
{
    stimy_pre();
    if (!client || !client->monitor){
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
    stimy_post();
}
void
noborderwin(int16_t *temp,client_t *client, bool set_unset)
{
    stimy_pre();
    if (!client->ignore_borders)
        stimy_post(return;);
    if (set_unset){
        *temp = conf.borderwidth;
        conf.borderwidth = 0;
        stimy_post(return;);
    }
    conf.borderwidth = *temp;
    stimy_post();
}
void
maximizewin_helper(client_t *client,uint16_t mon_x, uint16_t mon_y,
        uint16_t mon_width, uint16_t mon_height)
{
    stimy_pre();
    uint32_t values[4];
    values[0] = 0;
     stimy_echo(saveorigsize,saveorigsize(client));
     stimy_echo(xcb_configure_window,xcb_configure_window(conn, client->id, XCB_CONFIG_WINDOW_BORDER_WIDTH,
            values));
    client->x = mon_x;
    client->y = mon_y;
    client->width = mon_width;
    client->height = mon_height;
     stimy_echo(moveresizewin,moveresizewin(client->id, client->x, client->y, client->width,
            client->height));
    client->maxed = true;
    stimy_post();
}
void
fitonscreenwin(client_t *client)
{
    stimy_pre();
    int16_t mon_x, mon_y,temp=0;
    uint16_t mon_width, mon_height;
    bool willmove,willresize;
    willmove = willresize = client->vertmaxed = client->hormaxed = false;
     stimy_echo(getmonsize,getmonsize(true, &mon_x, &mon_y, &mon_width, &mon_height,client));
    if (client->maxed){
        client->maxed = false;
         stimy_echo(setborderswin,setborderswin(client,false));
    } else {
        if (client->width==mon_width && client->height==mon_height)
            willresize = true;
        else {
             stimy_echo(getmonsize,getmonsize(false, &mon_x, &mon_y, &mon_width, &mon_height,
                    client));
            if (client->width == mon_width && client->height
                    == mon_height)
                willresize = true;
        }
        if (willresize) {
            client->x = mon_x;
            client->y = mon_y;
            client->width -= conf.borderwidth * 2;
            client->height-= conf.borderwidth * 2;
             stimy_echo(maximizewin_helper,maximizewin_helper(client, mon_x, mon_y, mon_width,
                    mon_height));
            stimy_post(return;);
        } else {
             stimy_echo(getmonsize,getmonsize(true, &mon_x, &mon_y, &mon_width,
                    &mon_height,client));
        }
    }
    if (client->x > mon_x + mon_width || client->y > mon_y + mon_height
            || client->x < mon_x||client->y < mon_y) {
        willmove = true;
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
    if (0 != client->min_height && client->height < client->min_height) {
        client->height = client->min_height;
        willresize = true;
    }
    if (0 != client->min_width && client->width < client->min_width) {
        client->width = client->min_width;
        willresize = true;
    }
     stimy_echo(noborderwin,noborderwin(&temp, client, true));
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
         stimy_echo(movewindow,movewindow(client->id, client->x, client->y));
    if (willresize)
         stimy_echo(resizewin,resizewin(client->id, client->width, client->height));
     stimy_echo(noborderwin,noborderwin( &temp, client, false));
    stimy_post();
}
void
newwinrequest(xcb_generic_event_t *ev)
{
    stimy_pre();
     stimy_echo(debug2,debug2(__func__,"start"));
    xcb_map_request_event_t *e = (xcb_map_request_event_t *) ev;
    client_t *client = NULL;
    long data[] = { XCB_ICCCM_WM_STATE_NORMAL, XCB_NONE };
    if(findclient(winpi,&e->window))
        stimy_post(return;);
    if (!(client =  stimy_echo(setupwin,setupwin(e->window))))
        stimy_post(return;);
     stimy_echo(addtoworkspace,addtoworkspace(client, curwsi));
     stimy_echo(subwindowposition,subwindowposition(client));
     stimy_echo(windowposition,windowposition(client));
    if (-1 != randrbase){
        client->monitor = path[monpi]->data;
         stimy_echo(fitonscreenwin,fitonscreenwin(client));
        if (!client->monitor && path[monpi])
            client->monitor = path[monpi]->data;
    }
     stimy_echo(fitonscreenwin,fitonscreenwin(client));
     stimy_echo(xcb_map_window,xcb_map_window(conn,client->id));
     stimy_echo(xcb_change_property,xcb_change_property(conn, XCB_PROP_MODE_REPLACE, client->id,
            ewmh->_NET_WM_STATE, ewmh->_NET_WM_STATE, 32, 2, data));
     stimy_echo(raisewindow,raisewindow(client->id));
     stimy_echo(setfocuswin,setfocuswin(client));
     stimy_echo(debug2,debug2(__func__,"ended"));
    stimy_post();
}
client_t *
setupwin(xcb_window_t win)
{
    stimy_pre();
     stimy_echo(debug2,debug2(__func__,"start"));
    unsigned int i;
    uint32_t values[2],ws;
    xcb_atom_t a;
    xcb_size_hints_t hints;
    xcb_ewmh_get_atoms_reply_t win_type;
    node_t *node = NULL;
    client_t *client = NULL;
    if(xcb_ewmh_get_wm_window_type_reply(ewmh,
         stimy_echo(xcb_ewmh_get_wm_window_type,xcb_ewmh_get_wm_window_type(ewmh, win)), &win_type, NULL) == 1)
    {
        for (i = 0; i < win_type.atoms_len; i++) {
            a = win_type.atoms[i];
            if (a == ewmh->_NET_WM_WINDOW_TYPE_TOOLBAR || a
                    == ewmh->_NET_WM_WINDOW_TYPE_DOCK || a
                    == ewmh->_NET_WM_WINDOW_TYPE_DESKTOP ) {
                 stimy_echo(xcb_map_window,xcb_map_window(conn,win));
                stimy_post(return NULL;);
            }
        }
         stimy_echo(xcb_ewmh_get_atoms_reply_wipe,xcb_ewmh_get_atoms_reply_wipe(&win_type));
    }
    values[0] = XCB_EVENT_MASK_ENTER_WINDOW;
     stimy_echo(xcb_change_window_attributes,xcb_change_window_attributes(conn, win, XCB_CW_BACK_PIXEL,&conf.empty_col));
     stimy_echo(xcb_change_window_attributes_checked,xcb_change_window_attributes_checked(conn, win, XCB_CW_EVENT_MASK, values));
     stimy_echo(xcb_change_save_set,xcb_change_save_set(conn, XCB_SET_MODE_INSERT, win));
    node =  stimy_echo(addnode,addnode(sizeof(client_t)));
    if (!node)
        stimy_post(return NULL;);
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
     stimy_echo(getgeom,getgeom(&client->id, &client->x, &client->y, &client->width,
            &client->height, &client->depth));
     stimy_echo(xcb_icccm_get_wm_normal_hints_reply,xcb_icccm_get_wm_normal_hints_reply(conn,
             stimy_echo(xcb_icccm_get_wm_normal_hints_unchecked,xcb_icccm_get_wm_normal_hints_unchecked(conn, win)),
            &hints, NULL
    ));
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
     stimy_echo(linknode,linknode(winpi,node));
     stimy_echo(check_namewin,check_namewin(client));
     stimy_echo(addtoclientlist,addtoclientlist(client->id));
     stimy_echo(debug2,debug2(__func__,"ended"));
    stimy_post(return client;);
    stimy_post();
}
xcb_keycode_t *
xcb_get_keycodes(xcb_keysym_t keysym)
{
    stimy_pre();
    xcb_key_symbols_t *keysyms;
    xcb_keycode_t *keycode;
    if (!(keysyms =  stimy_echo(xcb_key_symbols_alloc,xcb_key_symbols_alloc(conn))))
        stimy_post(return NULL;);
    keycode =  stimy_echo(xcb_key_symbols_get_keycode,xcb_key_symbols_get_keycode(keysyms, keysym));
     stimy_echo(xcb_key_symbols_free,xcb_key_symbols_free(keysyms));
    stimy_post(return keycode;);
    stimy_post();
}
void
grabkeys(void)
{
    stimy_pre();
    xcb_keycode_t *keycode;
    int i,k,m;
    unsigned int modifiers[] = {
        0,
        XCB_MOD_MASK_LOCK,
        numlockmask,
        numlockmask | XCB_MOD_MASK_LOCK
    };
     stimy_echo(xcb_ungrab_key,xcb_ungrab_key(conn, XCB_GRAB_ANY, screen->root, XCB_MOD_MASK_ANY));
    for (i=0; i<LENGTH(keys);i++,delocate(keycode)) {
        keycode =  stimy_echo(xcb_get_keycodes,xcb_get_keycodes(keys[i].keysym));
        for (k=0; keycode[k] != XCB_NO_SYMBOL; k++)
            for (m=0; m<LENGTH(modifiers); m++)
                 stimy_echo(xcb_grab_key,xcb_grab_key(conn, 1, screen->root, keys[i].mod
                        | modifiers[m], keycode[k],
                        XCB_GRAB_MODE_ASYNC,
                        XCB_GRAB_MODE_ASYNC
                        ));
    }
    stimy_post();
}
bool
setup_keyboard(void)
{
    stimy_pre();
    xcb_get_modifier_mapping_reply_t *reply;
    xcb_keycode_t *modmap, *numlock;
    unsigned int i,j,n;
    reply =  stimy_echo(xcb_get_modifier_mapping_reply,xcb_get_modifier_mapping_reply(conn,
                 stimy_echo(xcb_get_modifier_mapping_unchecked,xcb_get_modifier_mapping_unchecked(conn)), NULL));
    if (!reply)
        stimy_post(return false;);
    modmap =  stimy_echo(xcb_get_modifier_mapping_keycodes,xcb_get_modifier_mapping_keycodes(reply));
    if (!modmap)
        stimy_post(return false;);
    numlock =  stimy_echo(xcb_get_keycodes,xcb_get_keycodes(XK_Num_Lock));
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
     stimy_echo(delocate,delocate(reply));
     stimy_echo(delocate,delocate(numlock));
    stimy_post(return true;);
    stimy_post();
}
bool
setupscreen(void)
{
    stimy_pre();
    xcb_get_window_attributes_reply_t *attr = NULL;
    client_t *client = NULL;
    uint32_t ws = 0;
    uint32_t len = 0;
    xcb_window_t *children = NULL;
    xcb_query_tree_reply_t *reply =  stimy_echo(xcb_query_tree_reply,xcb_query_tree_reply(conn,  stimy_echo(xcb_query_tree,xcb_query_tree(conn, screen->root)), 0));
    if (!reply)
        stimy_post(return false;);
    len =  stimy_echo(xcb_query_tree_children_length,xcb_query_tree_children_length(reply));
    children =  stimy_echo(xcb_query_tree_children,xcb_query_tree_children(reply));
    for (int i=0; i < len; i++,delocate(attr)){
        if(!(attr =  stimy_echo(xcb_get_window_attributes_reply,xcb_get_window_attributes_reply(conn,
                 stimy_echo(xcb_get_window_attributes,xcb_get_window_attributes(conn, children[i])), NULL))))
            continue;
        if (attr->override_redirect || attr->map_state != XCB_MAP_STATE_VIEWABLE)
            continue;
        if(!(client =  stimy_echo(setupwin,setupwin(children[i]))))
            continue;
        if (-1 != randrbase)
            client->monitor =  stimy_echo(findmonbycoord,findmonbycoord(client->x, client->y));
         stimy_echo(fitonscreenwin,fitonscreenwin(client));
         stimy_echo(setborderswin,setborderswin(client, false));
        ws =  stimy_echo(getwmdesktop,getwmdesktop(children[i]));
        if (get_unkil_state(children[i]))
             stimy_echo(unkillablewindow,unkillablewindow(client->winnode));
        if (ws == NET_WM_FIXED){
             stimy_echo(fixwindow,fixwindow(client->winnode));
            continue;
        }
        if (TWOBWM_NOWS != ws && ws < WORKSPACES) {
             stimy_echo(addtoworkspace,addtoworkspace(client, ws));
            if (ws != curwsi)
                 stimy_echo(xcb_unmap_window,xcb_unmap_window(conn,client->id));
            continue;
        }
         stimy_echo(addtoworkspace,addtoworkspace(client, curwsi));
         stimy_echo(addtoclientlist,addtoclientlist(children[i]));
    }
     stimy_echo(delocate,delocate(reply));
    stimy_post(return true;);
    stimy_post();
}
int
setuprandr(void)
{
    stimy_pre();
    int base = 0;
    const xcb_query_extension_reply_t *extension
        =  stimy_echo(xcb_get_extension_data,xcb_get_extension_data(conn, &xcb_randr_id));
    if (!extension->present)
        stimy_post(return -1;);
     stimy_echo(getrandr,getrandr());
    base = extension->first_event;
     stimy_echo(xcb_randr_select_input,xcb_randr_select_input(conn, screen->root,
            XCB_RANDR_NOTIFY_MASK_SCREEN_CHANGE
            | XCB_RANDR_NOTIFY_MASK_OUTPUT_CHANGE
            | XCB_RANDR_NOTIFY_MASK_CRTC_CHANGE
            | XCB_RANDR_NOTIFY_MASK_OUTPUT_PROPERTY));
    stimy_post(return base;);
    stimy_post();
}
void
changerandr(void)
{
    stimy_pre();
     stimy_echo(debug2,debug2(__func__,"start"));
    int len = 0;
    xcb_randr_get_screen_resources_current_cookie_t rcookie
        =  stimy_echo(xcb_randr_get_screen_resources_current,xcb_randr_get_screen_resources_current(conn, screen->root));
    xcb_randr_get_screen_resources_current_reply_t *res
        =  stimy_echo(xcb_randr_get_screen_resources_current_reply,xcb_randr_get_screen_resources_current_reply(conn, rcookie,NULL));
    if (!res)
        stimy_post(return;);
    xcb_timestamp_t timestamp = res->config_timestamp;
    len =  stimy_echo(xcb_randr_get_screen_resources_current_outputs_length,xcb_randr_get_screen_resources_current_outputs_length(res));
    xcb_randr_output_t *outputs
        =  stimy_echo(xcb_randr_get_screen_resources_current_outputs,xcb_randr_get_screen_resources_current_outputs(res));
     stimy_echo(delocate,delocate(res));
     stimy_echo(changeoutputs,changeoutputs(outputs,len,timestamp));
     stimy_echo(debug2,debug2(__func__,"ended"));
    stimy_post();
}
void
changeoutputs(xcb_randr_output_t *outputs,const int len, xcb_timestamp_t timestamp)
{
    stimy_pre();
    xcb_randr_get_crtc_info_cookie_t icookie;
    xcb_randr_get_crtc_info_reply_t *crtc = NULL;
    xcb_randr_get_output_info_reply_t *output = NULL;
    monitor_t *mon = NULL;
    node_t *node = NULL;
    client_t *client = NULL;
    xcb_randr_get_output_info_cookie_t ocookie[len];
    xcb_window_t win;
    for (int i=0; i < len; i++,delocate(output)){
        ocookie[i] =  stimy_echo(xcb_randr_get_output_info,xcb_randr_get_output_info(conn, outputs[i],timestamp));
        if (!(output =  stimy_echo(xcb_randr_get_output_info_reply,xcb_randr_get_output_info_reply(conn, ocookie[i], NULL))))
            continue;
        mon =  stimy_echo(findmonitor,findmonitor(outputs[i]));
         stimy_echo(printf,printf("%s crtc: %d mon: %p\n",__func__,output->crtc,mon));
        if (!output->crtc){
             if (!mon || !(node = path[winpi]))
                continue;
             stimy_echo(printf,printf("%s inside\n",__func__));
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
                  stimy_echo(fitonscreenwin,fitonscreenwin(client));
             }while((node =  stimy_echo(next,next(node,winpi))) && node != path[winpi]);
              stimy_echo(delmonitor,delmonitor(mon));
             continue;
        }
        icookie =  stimy_echo(xcb_randr_get_crtc_info,xcb_randr_get_crtc_info(conn, output->crtc,timestamp));
        if(!(crtc =  stimy_echo(xcb_randr_get_crtc_info_reply,xcb_randr_get_crtc_info_reply(conn, icookie, NULL))))
            continue;
        if (!mon){
             stimy_echo(addmonitor,addmonitor(outputs[i],crtc));
             stimy_echo(delocate,delocate(crtc));
            continue;
        }
        mon->x = crtc->x;
        mon->y = crtc->y;
        mon->width = crtc->width;
        mon->height = crtc->height;
         stimy_echo(arrbymon,arrbymon(mon));
         stimy_echo(delocate,delocate(crtc));
    }
    stimy_post();
}
void
getrandr(void)
{
    stimy_pre();
     stimy_echo(debug2,debug2(__func__,"start"));
    xcb_randr_get_screen_resources_current_cookie_t rcookie
        =  stimy_echo(xcb_randr_get_screen_resources_current,xcb_randr_get_screen_resources_current(conn, screen->root));
    xcb_randr_get_screen_resources_current_reply_t *res
        =  stimy_echo(xcb_randr_get_screen_resources_current_reply,xcb_randr_get_screen_resources_current_reply(conn, rcookie,NULL));
    if (!res)
        stimy_post(return;);
    xcb_timestamp_t timestamp = res->config_timestamp;
    int len =  stimy_echo(xcb_randr_get_screen_resources_current_outputs_length,xcb_randr_get_screen_resources_current_outputs_length(res));
    xcb_randr_output_t *outputs
        =  stimy_echo(xcb_randr_get_screen_resources_current_outputs,xcb_randr_get_screen_resources_current_outputs(res));
    xcb_randr_get_crtc_info_cookie_t icookie;
    xcb_randr_get_crtc_info_reply_t *crtc = NULL;
    xcb_randr_get_output_info_reply_t *output = NULL;
    monitor_t *mon = NULL;
    node_t *node = NULL;
    client_t *client = NULL;
    xcb_randr_get_output_info_cookie_t ocookie[len];
    xcb_window_t win;
    for (int i=0; i < len; i++,delocate(output)){
        ocookie[i] =  stimy_echo(xcb_randr_get_output_info,xcb_randr_get_output_info(conn, outputs[i],timestamp));
        if (!(output =  stimy_echo(xcb_randr_get_output_info_reply,xcb_randr_get_output_info_reply(conn, ocookie[i], NULL))))
            continue;
         stimy_echo(printf,printf("%s crtc: %d monid: %d\n",__func__,output->crtc,outputs[i]));
        icookie =  stimy_echo(xcb_randr_get_crtc_info,xcb_randr_get_crtc_info(conn, output->crtc,timestamp));
        if(!(crtc =  stimy_echo(xcb_randr_get_crtc_info_reply,xcb_randr_get_crtc_info_reply(conn, icookie, NULL))))
            continue;
         stimy_echo(printf,printf("%s x: %d y: %d width: %d height: %d\n",__func__,
            crtc->x, crtc->y, crtc->width,crtc->height));
         stimy_echo(addmonitor,addmonitor(outputs[i],crtc));
         stimy_echo(delocate,delocate(crtc));
    }
     stimy_echo(delocate,delocate(res));
    stimy_echo(debug2,debug2(__func__,"ended"));
    stimy_post();
}
void
arrbymon(monitor_t *monitor)
{
    stimy_pre();
    client_t *client;
    node_t *node = NULL;
    if((node = path[winpi]))
        stimy_post(return;);
    do{
        if ((client = node->data) && client->monitor == monitor)
             stimy_echo(fitonscreenwin,fitonscreenwin(client));
    }while((node =  stimy_echo(next,next(node,winpi))) && node != path[winpi]);
    stimy_post();
}
monitor_t *
findmonitor(xcb_randr_output_t id)
{
    stimy_pre();
     stimy_echo(debug2,debug2(__func__,"start"));
    monitor_t *mon = NULL;
    node_t *node = NULL;
    if(!(node = path[monpi]))
        stimy_post(return NULL;);
    do{
        if ((mon = node->data) && mon->id == id)
            stimy_post(return mon;);
    }while((node =  stimy_echo(next,next(node,monpi))) && node != path[monpi]);
     stimy_echo(debug2,debug2(__func__,"ended"));
    stimy_post(return NULL;);
    stimy_post();
}
monitor_t *
findmonclones(xcb_randr_output_t id, const int16_t x, const int16_t y)
{
    stimy_pre();
    monitor_t *clonemon = NULL;
    node_t *node = NULL;
   if(!(node = path[monpi]))
        stimy_post(return NULL;);
    do{
        if ((clonemon = node->data) && id != clonemon->id &&
            clonemon->x == x && clonemon->y == y)
            stimy_post(return clonemon;);
    }while((node =  stimy_echo(next,next(node,monpi))) && node != path[monpi]);
    stimy_post(return NULL;);
    stimy_post();
}
monitor_t *
findmonbycoord(const int16_t x, const int16_t y)
{
    stimy_pre();
    monitor_t *mon = NULL;
    node_t *node = NULL;
    if(!(node = path[monpi]))
        stimy_post(return NULL;);
    do{
        if(!(mon = node->data))
            continue;
        if ( x >= mon->x
            && x <= mon->x + mon->width
            && y >= mon->y
            && y <= mon->y+mon->height){
            stimy_post(return mon;);
        }
    }while((node =  stimy_echo(next,next(node,monpi))) && node != path[monpi]);
    stimy_post(return NULL;);
    stimy_post();
}
monitor_t *
addmonitor(xcb_randr_output_t id,xcb_randr_get_crtc_info_reply_t *crtc)
{
    stimy_pre();
    node_t *node = NULL;
    monitor_t *mon = NULL;
    if(!(node =  stimy_echo(addnode,addnode(sizeof(monitor_t)))))
        stimy_post(return NULL;);
    mon = node->data;
    mon->node = node;
    mon->id = id;
    mon->name = NULL;
    mon->x = crtc->x;
    mon->y = crtc->y;
    mon->width  = crtc->width;
    mon->height = crtc->height;
    incmoni;
     stimy_echo(linknode,linknode(monpi,node));
    stimy_post(return mon;);
    stimy_post();
}
void
raisewindow(xcb_drawable_t win)
{
    stimy_pre();
    uint32_t values[] = { XCB_STACK_MODE_ABOVE };
    if (screen->root == win || 0 == win)
        stimy_post(return;);
     stimy_echo(xcb_configure_window,xcb_configure_window(conn, win, XCB_CONFIG_WINDOW_STACK_MODE, values));
     stimy_echo(xcb_flush,xcb_flush(conn));
    stimy_post();
}
void
raiseorlowerwin()
{
    stimy_pre();
    uint32_t values[] = { XCB_STACK_MODE_OPPOSITE };
    client_t *client = NULL;
    if (!path[curpi])
        stimy_post(return;);
    client = path[curpi]->data;
     stimy_echo(xcb_configure_window,xcb_configure_window(conn, client->id,XCB_CONFIG_WINDOW_STACK_MODE,
            values));
     stimy_echo(xcb_flush,xcb_flush(conn));
    stimy_post();
}
void
movelimwin(client_t *client)
{
    stimy_pre();
    int16_t mon_y, mon_x,temp=0;
    uint16_t mon_height, mon_width;
     stimy_echo(getmonsize,getmonsize(true, &mon_x, &mon_y, &mon_width, &mon_height, client));
     stimy_echo(noborderwin,noborderwin(&temp, client, true));
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
     stimy_echo(movewindow,movewindow(client->id, client->x, client->y));
     stimy_echo(noborderwin,noborderwin(&temp, client, false));
    stimy_post();
}
void
movewindow(xcb_drawable_t win, const int16_t x, const int16_t y)
{
    stimy_pre();
    uint32_t values[2] = {x, y};
    if (screen->root == win || 0 == win)
        stimy_post(return;);
     stimy_echo(xcb_configure_window,xcb_configure_window(conn, win, XCB_CONFIG_WINDOW_X
            | XCB_CONFIG_WINDOW_Y, values));
     stimy_echo(xcb_flush,xcb_flush(conn));
    stimy_post();
}
void
selectwindows(const Arg *arg)
{
    stimy_pre();
    node_t *node = path[curpi];
    if(!node)
        stimy_post(return;);
    if(next(node,selpi)){
         stimy_echo(unlinknode,unlinknode(selpi,node));
         stimy_echo(setborderswin,setborderswin(node->data,true));
         stimy_echo(debug2,debug2(__func__,"unselected"));
        stimy_post(return;);
    }
     stimy_echo(linknode,linknode(selpi,node));
     stimy_echo(setborderswin,setborderswin(node->data,true));
     stimy_echo(debug2,debug2(__func__,"selected"));
    stimy_post(return;);
    stimy_post();
}
void
selectallwindows(const Arg *arg)
{
    stimy_pre();
       client_t *cl = NULL;
    node_t *node = NULL;
    for(int i =  stimy_echo(wspi,wspi(curwsi)); i <  stimy_echo(wspi,wspi(curwsi)) + maxloci;i++){
        if(!(node = path[i]))
            continue;
        do{
            if(next(node,selpi)){
                 stimy_echo(unlinknode,unlinknode(selpi,node));
                 stimy_echo(setborderswin,setborderswin(node->data,true));
                continue;
            }
             stimy_echo(linknode,linknode(selpi,node));
             stimy_echo(setborderswin,setborderswin(node->data,true));
        }while((node =  stimy_echo(next,next(node,i))) && node != path[i]);
    }
    stimy_post();
}
void
focusnextwin()
{
    stimy_pre();
    client_t *cl = NULL;
    node_t *node = NULL;
    for(int i =  stimy_echo(wspi,wspi(curwsi)); i <  stimy_echo(wspi,wspi(curwsi)) + maxloci; i++){
        if(i == curpi || !(node = path[i]))
            continue;
        cl = node->data;
         stimy_echo(raisewindow,raisewindow(cl->id));
         stimy_echo(setfocuswin,setfocuswin(cl));
        stimy_post(return;);
    }
    if(!(node = path[winpi]))
        stimy_post(return;);
    do{
        if(wsi(node->dimpi) != curwsi ||  stimy_echo(next,next(node,icopi)))
            continue;
        cl = node->data;
         stimy_echo(raisewindow,raisewindow(cl->id));
         stimy_echo(setfocuswin,setfocuswin(cl));
        stimy_post(return;);
    }while((node =  stimy_echo(next,next(node,winpi))) && node != path[winpi]);
    stimy_post();
}
void
focuslocationwin(const Arg *arg)
{
    stimy_pre();
    client_t *cl = NULL;
    node_t *node = path[curpi];
    if(!node || node ==  stimy_echo(next,next(node,curpi)) || !(node =  stimy_echo(next,next(node,curpi))))
        stimy_post(return;);
    cl = node->data;
     stimy_echo(raisewindow,raisewindow(cl->id));
     stimy_echo(setfocuswin,setfocuswin(cl));
    stimy_post();
}
void
setfocuswin(client_t *client)
{
    stimy_pre();
    long data[] = { XCB_ICCCM_WM_STATE_NORMAL, XCB_NONE };
    if (!client) {
         stimy_echo(debug2,debug2(__func__,"focus on root win"));
         stimy_echo(xcb_set_input_focus,xcb_set_input_focus(conn, XCB_NONE,
                XCB_INPUT_FOCUS_POINTER_ROOT, XCB_CURRENT_TIME));
        xcb_window_t no_win = 0;
         stimy_echo(xcb_change_property,xcb_change_property(conn, XCB_PROP_MODE_REPLACE, screen->root,
                ewmh->_NET_ACTIVE_WINDOW, XCB_ATOM_WINDOW, 32, 1,
                &no_win));
         stimy_echo(xcb_flush,xcb_flush(conn));
        stimy_post(return;);
    }
    if (client->id == screen->root)
        stimy_post(return;);
     stimy_echo(xcb_change_property,xcb_change_property(conn, XCB_PROP_MODE_REPLACE, client->id,
            ewmh->_NET_WM_STATE, ewmh->_NET_WM_STATE, 32, 2, data));
     stimy_echo(xcb_set_input_focus,xcb_set_input_focus(conn, XCB_INPUT_FOCUS_POINTER_ROOT, client->id,
            XCB_CURRENT_TIME));
     stimy_echo(xcb_change_property,xcb_change_property(conn, XCB_PROP_MODE_REPLACE, screen->root,
            ewmh->_NET_ACTIVE_WINDOW, XCB_ATOM_WINDOW, 32, 1,&client->id));
     stimy_echo(grabbuttons,grabbuttons(client));
     stimy_echo(setborderswin,setborderswin(client,true));
    if(path[wsfocpi] && path[wsfocpi]->data != client)
         stimy_echo(setborderswin,setborderswin(path[wsfocpi]->data,false));
    if(wsi(client->winnode->dimpi) == curwsi && !next(client->winnode,icopi)){
        if(loci(client->winnode->dimpi) != curloci)
            incloci;
        path[wsfocpi] = path[curpi] = client->winnode;
     stimy_echo(printf,printf("%s rechead:%p curpi:%d wsofcpi%d\n",__func__,(void*)path[recpi],curpi,wsfocpi));
    }
     stimy_echo(debug,debug(curpi,client->winnode,__func__,""));
    stimy_post();
}
client_t *
findclient(int headpi,const xcb_drawable_t *win)
{
    stimy_pre();
    client_t *client = NULL;
    node_t *node = NULL;
    int pi = 0;
    if(!(node = path[headpi]))
        stimy_post(return NULL;);
    if(propi(headpi)){
        do{
            if(!(client = node->data) || client->id != *win)
                continue;
             stimy_echo(printf,printf("%s id: %d\n",__func__,*win));
             stimy_echo(debug,debug(headpi,node,__func__,""));
            stimy_post(return client;);
        }while((node =  stimy_echo(next,next(node,headpi))) && node != path[headpi]);
        stimy_post(return NULL;);
    }
    for(int i =  stimy_echo(wspi,wspi(headpi)); i <  stimy_echo(wspi,wspi(headpi)) + maxloci;i++){
        if(!(node = path[i]))
            continue;
        do{
            if(!(client = node->data) || client->id != *win)
                continue;
             stimy_echo(printf,printf("%s id: %d\n",__func__,*win));
             stimy_echo(debug,debug(dimpi,node,__func__,""));
            stimy_post(return client;);
        }while((node =  stimy_echo(next,next(node,i))) && node != path[i]);
    }
    stimy_post(return NULL;);
    stimy_post();
}
void
start(const Arg *arg)
{
    stimy_pre();
    if (fork())
        stimy_post(return;);
    if (conn)
         stimy_echo(close,close(screen->root));
     stimy_echo(setsid,setsid());
     stimy_echo(execvp,execvp((char*)arg->com[0], (char**)arg->com));
    stimy_post();
}
void
resizelimwin(client_t *client)
{
    stimy_pre();
    int16_t mon_x, mon_y,temp=0;
    uint16_t mon_width, mon_height;
     stimy_echo(getmonsize,getmonsize(true,  &mon_x, &mon_y, &mon_width, &mon_height, client));
     stimy_echo(noborderwin,noborderwin(&temp, client, true));
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
     stimy_echo(resizewin,resizewin(client->id, client->width, client->height));
     stimy_echo(noborderwin,noborderwin(&temp, client, false));
    stimy_post();
}
void
moveresizewin(xcb_drawable_t win, const uint16_t x, const uint16_t y,
        const uint16_t width, const uint16_t height)
{
    stimy_pre();
    uint32_t values[4] = { x, y, width, height };
    if (screen->root == win || 0 == win)
        stimy_post(return;);
     stimy_echo(xcb_configure_window,xcb_configure_window(conn, win, XCB_CONFIG_WINDOW_X
            | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH
            | XCB_CONFIG_WINDOW_HEIGHT, values));
     stimy_echo(xcb_flush,xcb_flush(conn));
    stimy_post();
}
void
resizewin(xcb_drawable_t win, const uint16_t width, const uint16_t height)
{
    stimy_pre();
    uint32_t values[2] = { width , height };
    if (screen->root == win || 0 == win)
        stimy_post(return;);
     stimy_echo(xcb_configure_window,xcb_configure_window(conn, win, XCB_CONFIG_WINDOW_WIDTH
            | XCB_CONFIG_WINDOW_HEIGHT, values));
     stimy_echo(xcb_flush,xcb_flush(conn));
    stimy_post();
}
void
resizestepwin(const Arg *arg)
{
    stimy_pre();
    uint8_t stepx,stepy,cases = arg->i % 4;
    node_t *node = path[curpi];
    client_t *client = NULL;
    if (!node || !(client = node->data) || client->maxed)
        stimy_post(return;);
    arg->i < 4 ? (stepx = stepy = movements[1])
        : (stepx = stepy = movements[0]);
    if (client->width_inc > 7 && client->height_inc > 7) {
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
     stimy_echo(resizelimwin,resizelimwin(client));
    stimy_post();
}
void
resizestepwin_aspect(const Arg *arg)
{
    stimy_pre();
    node_t *node = path[curpi];
    client_t *client = NULL;
    if (!node || !(client = node->data) || client->maxed)
        stimy_post(return;);
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
     stimy_echo(resizelimwin,resizelimwin(client));
    stimy_post();
}
static void
snapwindow(client_t *client)
{
    stimy_pre();
    node_t *node = NULL;
    client_t *win = NULL;
    int16_t mon_x, mon_y;
    uint16_t mon_width, mon_height;
    if(!(node = path[curpi]))
        stimy_post(return;);
     stimy_echo(getmonsize,getmonsize(true, &mon_x,&mon_y,&mon_width,&mon_height,node->data));
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
    }while((node =  stimy_echo(next,next(node,curpi))) && node != path[curpi]);
    stimy_post();
}
void
mousemove(const int16_t rel_x, const int16_t rel_y)
{
    stimy_pre();
    node_t *node = path[curpi];
    client_t *client = NULL;
    if (!node)
        stimy_post(return;);
    client = node->data;
    client->x = rel_x;
    client->y = rel_y;
    if (borders[2] > 0)
         stimy_echo(snapwindow,snapwindow(client));
     stimy_echo(movelimwin,movelimwin(client));
    stimy_post();
}
static void
mousemotion(const Arg *arg)
{
    stimy_pre();
     stimy_echo(debug2,debug2(__func__,""));
    int16_t mx=0, my=0, winx=0, winy=0, winw=0, winh=0;
    xcb_query_pointer_reply_t *pointer = NULL;
    xcb_grab_pointer_reply_t  *grab_reply = NULL;
    xcb_motion_notify_event_t *ev = NULL;
    xcb_generic_event_t *e  = NULL;
    bool ungrab = false;
    node_t *node = path[curpi];
    client_t *client = NULL;
    if (!node)
        stimy_post(return;);
    client = node->data;
    pointer=xcb_query_pointer_reply(conn,xcb_query_pointer(conn,screen->root),0);
    if (!pointer || client->maxed) {
         stimy_echo(delocate,delocate(pointer));
        stimy_post(return;);
    }
    mx   = pointer->root_x;
    my   = pointer->root_y;
    winx = client->x;
    winy = client->y;
    winw = client->width;
    winh = client->height;
    xcb_cursor_t cursor;
    client_t example;
     stimy_echo(raisewindow,raisewindow(client->id));
    if(arg->i == TWOBWM_MOVE)
        cursor =  stimy_echo(create_font_cursor,create_font_cursor (conn, 52 ));
    else {
        cursor  =  stimy_echo(create_font_cursor,create_font_cursor (conn, 120));
        example =  stimy_echo(create_back_win,create_back_win());
         stimy_echo(xcb_map_window,xcb_map_window(conn,example.id));
    }
    grab_reply =  stimy_echo(xcb_grab_pointer_reply,xcb_grab_pointer_reply(conn,  stimy_echo(xcb_grab_pointer,xcb_grab_pointer(conn, 0,
            screen->root, BUTTONMASK | XCB_EVENT_MASK_BUTTON_MOTION
            | XCB_EVENT_MASK_POINTER_MOTION, XCB_GRAB_MODE_ASYNC,
            XCB_GRAB_MODE_ASYNC, XCB_NONE, cursor, XCB_CURRENT_TIME))
            , NULL
    ));
    if (grab_reply->status != XCB_GRAB_STATUS_SUCCESS) {
         stimy_echo(delocate,delocate(grab_reply));
        if (arg->i == TWOBWM_RESIZE)
             stimy_echo(xcb_unmap_window,xcb_unmap_window(conn,example.id));
        stimy_post(return;);
    }
     stimy_echo(delocate,delocate(grab_reply));
    ungrab = false;
    do {
        if (NULL!=e)
             stimy_echo(delocate,delocate(e));
        while(!(e =  stimy_echo(xcb_wait_for_event,xcb_wait_for_event(conn))))
             stimy_echo(xcb_flush,xcb_flush(conn));
        switch (e->response_type & ~0x80)
        {
            case XCB_CONFIGURE_REQUEST:
            case XCB_MAP_REQUEST:
                events[e->response_type & ~0x80](e);
                break;
            case XCB_MOTION_NOTIFY:
                ev = (xcb_motion_notify_event_t*)e;
                if (arg->i == TWOBWM_MOVE)
                     stimy_echo(mousemove,mousemove(winx + ev->root_x - mx,
                            winy + ev->root_y-my));
                else
                     stimy_echo(mouseresize,mouseresize(&example, winw + ev->root_x - mx,
                            winh + ev->root_y - my));
                 stimy_echo(xcb_flush,xcb_flush(conn));
                break;
            case XCB_KEY_PRESS:
            case XCB_KEY_RELEASE:
            case XCB_BUTTON_PRESS:
            case XCB_BUTTON_RELEASE:
                if (arg->i==TWOBWM_RESIZE) {
                    ev = (xcb_motion_notify_event_t*)e;
                     stimy_echo(mouseresize,mouseresize(client, winw + ev->root_x - mx,
                            winh + ev->root_y - my));
                     stimy_echo(setborderswin,setborderswin(client,true));
                }
                ungrab = true;
                break;
        }
    } while (!ungrab && client != NULL);
     stimy_echo(delocate,delocate(pointer));
     stimy_echo(delocate,delocate(e));
     stimy_echo(xcb_free_cursor,xcb_free_cursor(conn,cursor));
     stimy_echo(xcb_ungrab_pointer,xcb_ungrab_pointer(conn, XCB_CURRENT_TIME));
    if (arg->i == TWOBWM_RESIZE){
         stimy_echo(xcb_unmap_window,xcb_unmap_window(conn,example.id));
    }
     stimy_echo(xcb_flush,xcb_flush(conn));
    stimy_post();
}
void
mouseresize(client_t *client, const int16_t rel_x, const int16_t rel_y)
{
    stimy_pre();
    node_t *node = path[curpi];
    client_t *focuswin = NULL;
    if (!node)
        stimy_post(return;);
    focuswin = node->data;
    if(focuswin->id == screen->root || focuswin->maxed)
        stimy_post(return;);
    client->width  =  stimy_echo(abs,abs(rel_x));
    client->height =  stimy_echo(abs,abs(rel_y));
    if (resize_by_line) {
        client->width -= (client->width - client->base_width)
            % client->width_inc;
        client->height -= (client->height - client->base_height)
            % client->height_inc;
    }
     stimy_echo(resizelimwin,resizelimwin(client));
    client->vertmaxed = false;
    client->hormaxed  = false;
    stimy_post();
}
void
movestepwin(const Arg *arg)
{
    stimy_pre();
    int16_t start_x, start_y;
    uint8_t step, cases=arg->i;
    node_t *node = path[curpi];
    client_t *client = NULL;
    if (!node || !(client = node->data) || client->maxed)
        stimy_post(return;);
    if (!getpointer(&client->id, &start_x, &start_y))
        stimy_post(return;);
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
     stimy_echo(raisewindow,raisewindow(client->id));
     stimy_echo(movelimwin,movelimwin(client));
     stimy_echo(movepointerback,movepointerback(start_x,start_y,client));
     stimy_echo(xcb_flush,xcb_flush(conn));
    stimy_post();
}
inline void
setborderswin(client_t *client,const bool isitfocused)
{
    stimy_pre();
    uint32_t values[1];
    uint16_t half = 0;
    bool inv = conf.inverted_colors;
    if (client->maxed || client->ignore_borders)
        stimy_post(return;);
    values[0] = conf.borderwidth;
     stimy_echo(xcb_configure_window,xcb_configure_window(conn, client->id,XCB_CONFIG_WINDOW_BORDER_WIDTH, values));
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
    xcb_pixmap_t pmap =  stimy_echo(xcb_generate_id,xcb_generate_id(conn));
     stimy_echo(xcb_create_pixmap,xcb_create_pixmap(conn, client->depth, pmap, client->id,
            client->width + (conf.borderwidth * 2),
            client->height + (conf.borderwidth * 2)
    ));
    xcb_gcontext_t gc =  stimy_echo(xcb_generate_id,xcb_generate_id(conn));
     stimy_echo(xcb_create_gc,xcb_create_gc(conn, gc, pmap, 0, NULL));
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
     stimy_echo(xcb_change_gc,xcb_change_gc(conn, gc, XCB_GC_FOREGROUND, &values[0]));
     stimy_echo(xcb_poly_fill_rectangle,xcb_poly_fill_rectangle(conn, pmap, gc, 5, rect_outer));
    values[0]   = conf.focuscol;
    if (!isitfocused)
        values[0] = conf.unfocuscol;
     stimy_echo(xcb_change_gc,xcb_change_gc(conn, gc, XCB_GC_FOREGROUND, &values[0]));
     stimy_echo(xcb_poly_fill_rectangle,xcb_poly_fill_rectangle(conn, pmap, gc, 5, rect_inner));
    values[0] = pmap;
     stimy_echo(xcb_change_window_attributes,xcb_change_window_attributes(conn,client->id, XCB_CW_BORDER_PIXMAP,&values[0]));
     stimy_echo(xcb_free_pixmap,xcb_free_pixmap(conn,pmap));
     stimy_echo(xcb_free_gc,xcb_free_gc(conn,gc));
     stimy_echo(xcb_flush,xcb_flush(conn));
    stimy_post();
}
void
unmaxwin_helper(client_t *client)
{
    stimy_pre();
    uint32_t values[5], mask = 0;
    if (NULL == client)
        stimy_post(return;);
    client->x = client->origsize.x;
    client->y = client->origsize.y;
    client->width = client->origsize.width;
    client->height = client->origsize.height;
    values[0] = client->x;
    values[1] = client->y;
    values[2] = client->width;
    values[3] = client->height;
    client->maxed = client->hormaxed = 0;
     stimy_echo(moveresizewin,moveresizewin(client->id, client->x, client->y,
            client->width, client->height));
     stimy_echo(centerpointer,centerpointer(client->id,client));
     stimy_echo(setborderswin,setborderswin(client,true));
    stimy_post();
}
void
maximizewin(const Arg *arg)
{
    stimy_pre();
    if(!path[curpi])
        stimy_post(return;);
     stimy_echo(maxwin,maxwin(path[curpi]->data, true));
    stimy_post();
}
void
fullscreenwin(const Arg *arg)
{
    stimy_pre();
    if(!path[curpi])
        stimy_post(return;);
     stimy_echo(maxwin,maxwin(path[curpi]->data, false));
    stimy_post();
}
void
unmaxwin(client_t *client){
    stimy_pre();
     stimy_echo(unmaxwin_helper,unmaxwin_helper(client));
    client->maxed = false;
     stimy_echo(setborderswin,setborderswin(client,true));
     stimy_echo(xcb_change_property,xcb_change_property(conn, XCB_PROP_MODE_REPLACE, client->id,
            ewmh->_NET_WM_STATE, XCB_ATOM_ATOM, 32, 0, NULL));
    stimy_post();
}
void
maxwin(client_t *client, bool with_offsets)
{
    stimy_pre();
    uint32_t values[4];
    int16_t mon_x, mon_y;
    int16_t mon_width, mon_height;
    if (!client)
        stimy_post(return;);
    if (client->maxed) {
         stimy_echo(unmaxwin,unmaxwin(client));
        stimy_post(return;);
    }
     stimy_echo(getmonsize,getmonsize(with_offsets, &mon_x, &mon_y, &mon_width, &mon_height, client));
     stimy_echo(maximizewin_helper,maximizewin_helper(client, mon_x, mon_y, mon_width, mon_height));
     stimy_echo(raise_current_window,raise_current_window());
    if (!with_offsets) {
         stimy_echo(xcb_change_property,xcb_change_property(conn, XCB_PROP_MODE_REPLACE, client->id,
            ewmh->_NET_WM_STATE, XCB_ATOM_ATOM, 32, 1,
            &ewmh->_NET_WM_STATE_FULLSCREEN));
    }
     stimy_echo(xcb_flush,xcb_flush(conn));
    stimy_post();
}
void
maxvertwin_hor(const Arg *arg)
{
    stimy_pre();
    uint32_t values[2];
    int16_t mon_y, mon_x, temp = 0;
    uint16_t mon_height, mon_width;
    node_t *node = path[curpi];
    client_t *client = NULL;
    if (!node)
        stimy_post(return;);
    client = node->data;
    if (client->vertmaxed || client->hormaxed) {
         stimy_echo(unmaxwin,unmaxwin(client));
        client->vertmaxed = client->hormaxed = false;
         stimy_echo(fitonscreenwin,fitonscreenwin(client));
         stimy_echo(setborderswin,setborderswin(client, true));
        stimy_post(return;);
    }
     stimy_echo(getmonsize,getmonsize(true, &mon_x,&mon_y,&mon_width,&mon_height,client));
     stimy_echo(saveorigsize,saveorigsize(client));
     stimy_echo(noborderwin,noborderwin(&temp, client,true));
    if (arg->i==TWOBWM_MAXIMIZE_VERTICALLY) {
        client->y = mon_y;
        client->height = mon_height - (conf.borderwidth * 2);
        values[0] = client->y;
        values[1] = client->height;
         stimy_echo(xcb_configure_window,xcb_configure_window(conn, client->id, XCB_CONFIG_WINDOW_Y
                | XCB_CONFIG_WINDOW_HEIGHT, values));
        client->vertmaxed = true;
    } else if (arg->i==TWOBWM_MAXIMIZE_HORIZONTALLY) {
        client->x = mon_x;
        client->width = mon_width - (conf.borderwidth * 2);
        values[0] = client->x;
        values[1] = client->width;
         stimy_echo(xcb_configure_window,xcb_configure_window(conn, client->id, XCB_CONFIG_WINDOW_X
                | XCB_CONFIG_WINDOW_WIDTH, values));
        client->hormaxed = true;
    }
     stimy_echo(noborderwin,noborderwin(&temp, client,false));
     stimy_echo(raise_current_window,raise_current_window());
     stimy_echo(centerpointer,centerpointer(client->id,client));
     stimy_echo(setborderswin,setborderswin(client,true));
    stimy_post();
}
void
maxhalfwin(const Arg *arg)
{
    stimy_pre();
    uint32_t values[4];
    int16_t mon_x, mon_y, temp=0;
    uint16_t mon_width, mon_height;
    node_t *node = path[curpi];
    client_t *client = NULL;
    if (!node || !(client = node->data) || client->maxed)
        stimy_post(return;);
     stimy_echo(getmonsize,getmonsize(true, &mon_x,&mon_y,&mon_width,&mon_height,client));
     stimy_echo(noborderwin,noborderwin(&temp, client, true));
    if (arg->i>4) {
        if (arg->i>6) {
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
            if (arg->i == TWOBWM_MAXHALF_FOLD_HORIZONTAL)
                client->width = client->width / 2
                    - conf.borderwidth;
            else
                client->width = client->width * 2
                    + (2 * conf.borderwidth);
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
     stimy_echo(moveresizewin,moveresizewin(client->id, client->x, client->y,
            client->width, client->height));
    client->verthor = true;
     stimy_echo(noborderwin,noborderwin(&temp, client, false));
     stimy_echo(raise_current_window,raise_current_window());
     stimy_echo(fitonscreenwin,fitonscreenwin(client));
     stimy_echo(centerpointer,centerpointer(client->id, client));
     stimy_echo(setborderswin,setborderswin(client, true));
    stimy_post();
}
void
unhidewin(void)
{
    stimy_pre();
    node_t *tail,*node = NULL;
    client_t *client = NULL;
    if(!(node = path[icopi]))
        stimy_post(return;);
     stimy_echo(debug2,debug2(__func__,"start"));
    for(int i = length[icopath]; i >= 0; i--){
        tail = node;
        client = tail->data;
        node =  stimy_echo(next,next(node,icopi));
        if(wsi(tail->dimpi) != curwsi)
            continue;
         stimy_echo(addtoworkspace,addtoworkspace(client,curwsi));
         stimy_echo(xcb_map_window,xcb_map_window(conn,client->id));
         stimy_echo(unhidewin_helper,unhidewin_helper(client));
         stimy_echo(unlinknode,unlinknode(icopi,tail));
    }
     stimy_echo(setfocuswin,setfocuswin(client));
     stimy_echo(debug2,debug2(__func__,"ended"));
    stimy_post();
}
void
unhidewin_helper(client_t *client)
{
    stimy_pre();
    long data[] = {
        XCB_ICCCM_WM_STATE_NORMAL,
        XCB_NONE,
    };
     stimy_echo(xcb_change_property,xcb_change_property(conn, XCB_PROP_MODE_REPLACE, client->id,
        ewmh->_NET_WM_STATE, ewmh->_NET_WM_STATE, 32, 3, data));
     stimy_echo(xcb_flush,xcb_flush(conn));
    stimy_post();
}
void
hidewin(void)
{
    stimy_pre();
    node_t *tail,*node = path[curpi];
    client_t *client = NULL;
    if(!node)
        stimy_post(return;);
     stimy_echo(debug2,debug2(__func__,"start"));
    if(!next(node,icopi))
         stimy_echo(linknode,linknode(selpi,node));
    for(node = path[selpi];node;
        tail = node,node =  stimy_echo(next,next(node,selpi)),unlinknode(selpi,tail)){
        client = node->data;
         stimy_echo(linknode,linknode(icopi,node));
         stimy_echo(unlinknode,unlinknode(node->dimpi,node));
         stimy_echo(xcb_unmap_window,xcb_unmap_window(conn,client->id));
         stimy_echo(hidewin_helper,hidewin_helper(client));
    }
     stimy_echo(focusnextwin,focusnextwin());
     stimy_echo(debug2,debug2(__func__,"ended"));
    stimy_post();
}
void
hidewin_helper(client_t *client)
{
    stimy_pre();
    long data[] = {
        XCB_ICCCM_WM_STATE_ICONIC,
        ewmh->_NET_WM_STATE_HIDDEN,
        XCB_NONE
    };
     stimy_echo(xcb_change_property,xcb_change_property(conn, XCB_PROP_MODE_REPLACE, client->id,
        ewmh->_NET_WM_STATE, ewmh->_NET_WM_STATE, 32, 3, data));
     stimy_echo(xcb_flush,xcb_flush(conn));
    stimy_post();
}
bool
getpointer(const xcb_drawable_t *win, int16_t *x, int16_t *y)
{
    stimy_pre();
    xcb_query_pointer_reply_t *pointer =  stimy_echo(xcb_query_pointer_reply,xcb_query_pointer_reply(conn,xcb_query_pointer(conn, *win), 0));
    if (!pointer)
        stimy_post(return false;);
    *x = pointer->win_x;
    *y = pointer->win_y;
     stimy_echo(delocate,delocate(pointer));
    stimy_post(return true;);
    stimy_post();
}
bool
getgeom(const xcb_drawable_t *win, int16_t *x, int16_t *y, uint16_t *width,
        uint16_t *height, uint8_t *depth)
{
    stimy_pre();
    xcb_get_geometry_reply_t *geom =
     stimy_echo(xcb_get_geometry_reply,xcb_get_geometry_reply(conn,  stimy_echo(xcb_get_geometry,xcb_get_geometry(conn, *win)), NULL));
    if (!geom)
        stimy_post(return false;);
    *x = geom->x;
    *y = geom->y;
    *width = geom->width;
    *height = geom->height;
    *depth = geom->depth;
     stimy_echo(delocate,delocate(geom));
    stimy_post(return true;);
    stimy_post();
}
void
subwindowposition(client_t *client)
{
    stimy_pre();
     stimy_echo(debug2,debug2(__func__,"start"));
    if(client->usercoord)
        stimy_post(return;);
    xcb_window_t prop;
    xcb_get_property_cookie_t cookie;
    client_t *parent = NULL;
    cookie =  stimy_echo(xcb_icccm_get_wm_transient_for_unchecked,xcb_icccm_get_wm_transient_for_unchecked(conn,client->id));
    if(!cookie.sequence)
        stimy_post(return;);
    if(!xcb_icccm_get_wm_transient_for_reply(conn,cookie,&prop,NULL))
        stimy_post(return;);
    if(!(parent =  stimy_echo(findclient,findclient(curwspi,&prop))))
        stimy_post(return;);
    client->x = parent->x+(parent->width/2.0) - (client->width/2.0);
    client->y = parent->y+(parent->height/2.0) - (client->height/2.0);
    client->usercoord = true;
     stimy_echo(debug2,debug2(__func__,"ended"));
    stimy_post();
}
void
windowposition(client_t *client)
{
    stimy_pre();
     stimy_echo(debug2,debug2(__func__,"start"));
    if(client->usercoord)
        stimy_post(return;);
    int16_t pointx, pointy, mon_x, mon_y, temp = 0;
    uint16_t mon_width, mon_height;
     stimy_echo(getmonsize,getmonsize(true, &mon_x, &mon_y, &mon_width, &mon_height,client));
    client->x = mon_x + mon_x + mon_width - (client->width + conf.borderwidth * 2);
    client->y = mon_y + mon_height - (client->height + conf.borderwidth * 2);
    client->x /= 2;
    client->y /= 2;
     stimy_echo(movelimwin,movelimwin(client));
     stimy_echo(setborderswin,setborderswin(client,true));
     stimy_echo(debug2,debug2(__func__,"ended"));
    stimy_post();
}
void
windowsize(const Arg *arg)
{
    stimy_pre();
    node_t *tail,*node = path[curpi];
    client_t *cl = NULL;
    Arg arg1 = { .i=TWOBWM_TELEPORT_TOP};
    Arg arg2 = { .i=TWOBWM_TELEPORT_CENTER_X};
    Arg arg3 = { .i=TWOBWM_TELEPORT_CENTER_Y};
    int16_t pointx, pointy, mon_x, mon_y, temp = 0;
    uint16_t mon_width, mon_height;
    if(node && !next(node,selpi))
         stimy_echo(linknode,linknode(selpi,node));
    for(node = path[selpi];node;
        tail = node,node =  stimy_echo(next,next(node,selpi)),unlinknode(selpi,tail)){
        cl = node->data;
        if (cl->maxed)
            continue;
         stimy_echo(teleportwin_helper,teleportwin_helper(&arg1,cl));
         stimy_echo(getmonsize,getmonsize(true, &mon_x, &mon_y, &mon_width, &mon_height,cl));
        cl->width = windowwidth[arg->i];
        cl->height = windowheight[arg->i];
         stimy_echo(resizelimwin,resizelimwin(cl));
         stimy_echo(teleportwin_helper,teleportwin_helper(&arg2,cl));
         stimy_echo(teleportwin_helper,teleportwin_helper(&arg3,cl));
         stimy_echo(setborderswin,setborderswin(cl,true));
    }
    if(!(node = path[curpi]))
        stimy_post(return;);
    cl = node->data;
     stimy_echo(raisewindow,raisewindow(cl->id));
     stimy_echo(setfocuswin,setfocuswin(cl));
    stimy_post();
}
void
swapwin()
{
    stimy_pre();
    client_t *cl = NULL;
    node_t *node = path[curpi];
     stimy_echo(debug2,debug2(__func__,"start"));
    if(!node)
        stimy_post(return;);
     stimy_echo(unlinknode,unlinknode(curpi,node));
     stimy_echo(linknode,linknode(locpi(stepup(curloci,maxloci)),node));
     stimy_echo(swapwin_helper,swapwin_helper(node->data));
    cl = node->data;
     stimy_echo(raisewindow,raisewindow(cl->id));
     stimy_echo(setfocuswin,setfocuswin(cl));
     stimy_echo(debug2,debug2(__func__,"ended"));
    stimy_post();
}
void
swapwin_helper(client_t *cl)
{
    stimy_pre();
    if(cl->position == CENTER)
        stimy_post(return;);
    node_t *node = cl->winnode;
    int16_t pointx, pointy, mon_x, mon_y, temp = 0;
    uint16_t mon_width, mon_height, center, a4width, gap = 5;
     stimy_echo(getmonsize,getmonsize(true, &mon_x, &mon_y, &mon_width, &mon_height,cl));
     stimy_echo(noborderwin,noborderwin(&temp, cl,true));
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
     stimy_echo(movewindow,movewindow(cl->id, cl->x, cl->y));
     stimy_echo(noborderwin,noborderwin(&temp, cl, false));
    stimy_post();
}
void
sidebysidewin(const Arg *arg)
{
    stimy_pre();
    node_t *node = NULL;
    client_t *cl = NULL;
    for(int i =  stimy_echo(wspi,wspi(curwsi)); i <  stimy_echo(wspi,wspi(curwsi)) + maxloci;i++){
        if(!(node = path[i]))
            continue;
        do{
            if (!(cl = node->data) || cl->maxed)
                continue;
             stimy_echo(sidebysidewin_helper,sidebysidewin_helper(cl));
        }while((node =  stimy_echo(next,next(node,i))) && node != path[i]);
    }
    if(!(node = path[curpi]))
        stimy_post(return;);
    cl = node->data;
     stimy_echo(raisewindow,raisewindow(cl->id));
     stimy_echo(setfocuswin,setfocuswin(cl));
    stimy_post();
}
inline void
sidebysidewin_helper(client_t *cl)
{
    stimy_pre();
    node_t *node = cl->winnode;
    int16_t pointx, pointy, mon_x, mon_y, temp = 0;
    uint16_t mon_width, mon_height, center, a4width, gap = 5;
     stimy_echo(getmonsize,getmonsize(true, &mon_x, &mon_y, &mon_width, &mon_height,cl));
     stimy_echo(noborderwin,noborderwin(&temp, cl,true));
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
     stimy_echo(movewindow,movewindow(cl->id, cl->x, cl->y));
     stimy_echo(movelimwin,movelimwin(cl));
     stimy_echo(noborderwin,noborderwin(&temp, cl, false));
    stimy_post();
}
void
teleportwin(const Arg *arg)
{
    stimy_pre();
    node_t *tail,*node = path[curpi];
    client_t *cl = NULL;
    if(!node)
        stimy_post(return;);
    if(!next(node,selpi))
         stimy_echo(linknode,linknode(selpi,node));
    for(node = path[selpi];node;
        tail = node,node =  stimy_echo(next,next(node,selpi)),unlinknode(selpi,tail)){
         stimy_echo(teleportwin_helper,teleportwin_helper(arg,node->data));
    }
    if(!(node = path[curpi]))
        stimy_post(return;);
     stimy_echo(setborderswin,setborderswin(node->data,true));
    stimy_post();
}
inline void
teleportwin_helper(const Arg *arg,client_t *client)
{
    stimy_pre();
    int16_t pointx, pointy, mon_x, mon_y, temp = 0;
    uint16_t mon_width, mon_height;
    if (!client || client->maxed)
        stimy_post(return;);
     stimy_echo(noborderwin,noborderwin(&temp, client,true));
     stimy_echo(getmonsize,getmonsize(true, &mon_x, &mon_y, &mon_width, &mon_height,client));
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
     stimy_echo(movewindow,movewindow(client->id, client->x, client->y));
     stimy_echo(noborderwin,noborderwin(&temp, client, false));
    stimy_post();
}
inline void
killclient(client_t *client)
{
    stimy_pre();
     stimy_echo(debug,debug(curpi,client->winnode,__func__,""));
     stimy_echo(xcb_kill_client,xcb_kill_client(conn,client->id));
    stimy_post();
}
void
deletewin(void)
{
    stimy_pre();
     stimy_echo(debug2,debug2(__func__,"start"));
    client_t *client = NULL;
    int len = 0;
    node_t *tail,*node = path[curpi];
    if(!node)
        stimy_post(return;);
    if(!next(node,selpi))
         stimy_echo(linknode,linknode(selpi,node));
    if(!(node = path[selpi]))
        stimy_post(return;);
    for(int i = length[selpath]; i >= 0 ; i--){
        tail = node;
        node =  stimy_echo(next,next(node,selpi));
        if(next(tail,unkpi)){
             stimy_echo(unlinknode,unlinknode(selpi,tail));
            continue;
        }
         stimy_echo(deletewin_help,deletewin_help(tail->data));
         stimy_echo(denode,denode(tail));
    }
     stimy_echo(focusnextwin,focusnextwin());
     stimy_echo(debug2,debug2(__func__,"ended"));
    stimy_post();
}
inline void
deletewin_help(client_t *client)
{
    stimy_pre();
    xcb_icccm_get_wm_protocols_reply_t protocols;
    xcb_get_property_cookie_t cookie;
    cookie =  stimy_echo(xcb_icccm_get_wm_protocols_unchecked,xcb_icccm_get_wm_protocols_unchecked(conn, client->id,
            ewmh->WM_PROTOCOLS));
    if (client->id == top_win)
        top_win = 0;
    if (!xcb_icccm_get_wm_protocols_reply(conn, cookie, &protocols, NULL)){
         stimy_echo(killclient,killclient(client));
        stimy_post(return;);
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
         stimy_echo(xcb_send_event,xcb_send_event(conn, false, client->id,
            XCB_EVENT_MASK_NO_EVENT, (char *)&ev));
         stimy_echo(xcb_icccm_get_wm_protocols_reply_wipe,xcb_icccm_get_wm_protocols_reply_wipe(&protocols));
         stimy_echo(debug2,debug2(__func__,"event send"));
        stimy_post(return;);
    }
    stimy_post();
}
void
changescreenwin(const Arg *arg)
{
    stimy_pre();
    float xpercentage, ypercentage;
    node_t *node = path[curpi];
    client_t *client = NULL;
    if (!node)
        stimy_post(return;);
    client = node->data;
    if (!client->monitor)
        stimy_post(return;);
    if (arg->i == TWOBWM_NEXT_SCREEN)
        node =  stimy_echo(next,next(client->monitor->node,monpi));
    else
        node =  stimy_echo(prev,prev(client->monitor->node,monpi));
    xpercentage  = (float)((client->x - client->monitor->x)/(client->monitor->width));
    ypercentage  = (float)((client->y-client->monitor->y)/(client->monitor->height));
    client->monitor = node->data;
    client->x = client->monitor->width * xpercentage + client->monitor->x + 0.5;
    client->y = client->monitor->height * ypercentage + client->monitor->y+0.5;
     stimy_echo(raisewindow,raisewindow(client->id));
     stimy_echo(fitonscreenwin,fitonscreenwin(client));
     stimy_echo(movelimwin,movelimwin(client));
     stimy_echo(setborderswin,setborderswin(client, true));
     stimy_echo(centerpointer,centerpointer(client->id, client));
     stimy_echo(debug,debug(monpi,node,__func__,""));
    stimy_post();
}
void
cursor_move(const Arg *arg)
{
    stimy_pre();
    uint16_t speed = 0;
    arg->i < 4 ? (speed = movements[3]) : (speed = movements[2]);
    switch(arg->i%4){
        case TWOBWM_CURSOR_UP:
         stimy_echo(xcb_warp_pointer,xcb_warp_pointer(conn, XCB_NONE, XCB_NONE,0, 0, 0, 0, 0, -speed));
        break;
        case TWOBWM_CURSOR_DOWN:
         stimy_echo(xcb_warp_pointer,xcb_warp_pointer(conn, XCB_NONE, XCB_NONE,0, 0, 0, 0, 0, speed));
        break;
        case TWOBWM_CURSOR_RIGHT:
         stimy_echo(xcb_warp_pointer,xcb_warp_pointer(conn, XCB_NONE, XCB_NONE,0, 0, 0, 0, speed, 0));
        break;
        case TWOBWM_CURSOR_LEFT:
         stimy_echo(xcb_warp_pointer,xcb_warp_pointer(conn, XCB_NONE, XCB_NONE,0, 0, 0, 0, -speed, 0));
        break;
    }
     stimy_echo(xcb_flush,xcb_flush(conn));
    stimy_post();
}
static xcb_keysym_t
xcb_get_keysym(xcb_keycode_t keycode)
{
    stimy_pre();
    xcb_key_symbols_t *keysyms = NULL;
    if (!(keysyms =  stimy_echo(xcb_key_symbols_alloc,xcb_key_symbols_alloc(conn))))
        stimy_post(return 0;);
    xcb_keysym_t keysym =  stimy_echo(xcb_key_symbols_get_keysym,xcb_key_symbols_get_keysym(keysyms, keycode, 0));
     stimy_echo(xcb_key_symbols_free,xcb_key_symbols_free(keysyms));
    stimy_post(return keysym;);
    stimy_post();
}
void
circulaterequest(xcb_generic_event_t *ev)
{
    stimy_pre();
     stimy_echo(debug2,debug2(__func__,"start"));
    xcb_circulate_request_event_t *e = (xcb_circulate_request_event_t *)ev;
     stimy_echo(xcb_circulate_window,xcb_circulate_window(conn, e->window, e->place));
    stimy_post();
}
void
handle_keypressrequest(xcb_generic_event_t *e)
{
    stimy_pre();
    xcb_key_press_event_t *ev = (xcb_key_press_event_t *)e;
    xcb_keysym_t keysym =  stimy_echo(xcb_get_keysym,xcb_get_keysym(ev->detail));
    for (unsigned int i = 0; i < LENGTH(keys); i++){
        if (keysym != keys[i].keysym || CLEANMASK(keys[i].mod)
            != CLEANMASK(ev->state) || !keys[i].func)
            continue;
        keys[i].func(&keys[i].arg);
        stimy_post(return;);
    }
    stimy_post();
}
void
configwin(xcb_window_t win, uint16_t mask, const struct winconf *wc)
{
    stimy_pre();
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
        stimy_post(return;);
     stimy_echo(xcb_configure_window,xcb_configure_window(conn, win, mask, values));
     stimy_echo(xcb_flush,xcb_flush(conn));
    stimy_post();
}
void
configurerequest(xcb_generic_event_t *ev)
{
    stimy_pre();
     stimy_echo(debug2,debug2(__func__,"start"));
    xcb_configure_request_event_t *e = (xcb_configure_request_event_t *)ev;
    client_t *client = NULL;
    struct winconf wc;
    int16_t mon_x, mon_y;
    uint16_t mon_width, mon_height;
    uint32_t values[1];
    if (!(client =  stimy_echo(findclient,findclient(curwspi,&e->window)))){
        wc.x = e->x;
        wc.y = e->y;
        wc.width = e->width;
        wc.height = e->height;
        wc.sibling = e->sibling;
        wc.stackmode = e->stack_mode;
         stimy_echo(configwin,configwin(e->window,e->value_mask,&wc));
         stimy_echo(debug2,debug2(__func__,"unmap ended"));
        stimy_post(return;);
    }
     stimy_echo(getmonsize,getmonsize(true, &mon_x, &mon_y, &mon_width, &mon_height, client));
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
    if (e->value_mask & XCB_CONFIG_WINDOW_SIBLING) {
        values[0] = e->sibling;
         stimy_echo(xcb_configure_window,xcb_configure_window(conn, e->window,
                XCB_CONFIG_WINDOW_SIBLING,values));
    }
    if (e->value_mask & XCB_CONFIG_WINDOW_STACK_MODE) {
        values[0] = e->stack_mode;
         stimy_echo(xcb_configure_window,xcb_configure_window(conn, e->window,
                XCB_CONFIG_WINDOW_STACK_MODE,values));
    }
    if (!client->maxed) {
         stimy_echo(resizelimwin,resizelimwin(client));
         stimy_echo(movelimwin,movelimwin(client));
         stimy_echo(fitonscreenwin,fitonscreenwin(client));
    }
     stimy_echo(setborderswin,setborderswin(client,true));
     stimy_echo(debug2,debug2(__func__,"ended"));
    stimy_post();
}
xcb_cursor_t
create_font_cursor(xcb_connection_t *conn, uint16_t glyph)
{
    stimy_pre();
    static xcb_font_t cursor_font;
    cursor_font =  stimy_echo(xcb_generate_id,xcb_generate_id (conn));
     stimy_echo(xcb_open_font,xcb_open_font(conn, cursor_font,  stimy_echo(strlen,strlen("cursor")), "cursor"));
    xcb_cursor_t cursor =  stimy_echo(xcb_generate_id,xcb_generate_id (conn));
     stimy_echo(xcb_create_glyph_cursor,xcb_create_glyph_cursor(conn, cursor, cursor_font, cursor_font, glyph,
            glyph + 1,0x3232, 0x3232, 0x3232, 0xeeee, 0xeeee, 0xeeec
    ));
    stimy_post(return cursor;);
    stimy_post();
}
client_t
create_back_win(void)
{
    stimy_pre();
    client_t temp_win;
    uint32_t values[1] = { conf.focuscol };
    node_t *node = path[curpi];
    client_t *client = NULL;
    if (!node)
        NULL;
    client = node->data;
    temp_win.id =  stimy_echo(xcb_generate_id,xcb_generate_id(conn));
     stimy_echo(xcb_create_window,xcb_create_window(conn,
            XCB_COPY_FROM_PARENT,
            temp_win.id,
            screen->root,
            client->x,
            client->y,
            client->width,
            client->height,
            borders[3],
            XCB_WINDOW_CLASS_INPUT_OUTPUT,
            screen->root_visual,
            XCB_CW_BORDER_PIXEL,
            values
    ));
    if (conf.enable_compton)
    {
        values[0] = 1;
         stimy_echo(xcb_change_window_attributes,xcb_change_window_attributes(conn, temp_win.id,XCB_BACK_PIXMAP_PARENT_RELATIVE, values));
    }
    else
    {
        values[0] = conf.unfocuscol;
         stimy_echo(xcb_change_window_attributes,xcb_change_window_attributes(conn, temp_win.id,XCB_CW_BACK_PIXEL, values));
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
    stimy_post(return temp_win;);
    stimy_post();
}
void
buttonpressrequest(xcb_generic_event_t *ev)
{
    stimy_pre();
     stimy_echo(debug2,debug2(__func__,""));
    xcb_button_press_event_t *e = (xcb_button_press_event_t *)ev;
    for (unsigned int i = 0; i < LENGTH(buttons); i++){
        if (!buttons[i].func || buttons[i].button != e->detail
            || CLEANMASK(buttons[i].mask) != CLEANMASK(e->state))
            continue;
        if (!path[curpi] && buttons[i].func == mousemotion)
            stimy_post(return;);
        if (!buttons[i].root_only){
            buttons[i].func(&(buttons[i].arg));
            continue;
        }
        if (e->event == e->root && e->child == 0)
            buttons[i].func(&(buttons[i].arg));
    }
    stimy_post();
}
void
clientmessage(xcb_generic_event_t *ev)
{
    stimy_pre();
     stimy_echo(debug2,debug2(__func__,"start"));
    xcb_client_message_event_t *e= (xcb_client_message_event_t *)ev;
    client_t *client = NULL;
    if (e->type == ewmh->_NET_CURRENT_DESKTOP){
         stimy_echo(debug2,debug2(__func__,"changews"));
         stimy_echo(changews_help,changews_help(e->data.data32[0]));
        stimy_post(return;);
    }
    if(!(client =  stimy_echo(findclient,findclient(winpi, &e->window)))){
         stimy_echo(debug2,debug2(__func__,"noclient."));
        stimy_post(return;);
    }
    if ((e->type == ATOM[wm_change_state] && e->format==32
            && e->data.data32[0]==XCB_ICCCM_WM_STATE_ICONIC)
            || e->type == ewmh->_NET_ACTIVE_WINDOW)
    {
         stimy_echo(debug2,debug2(__func__,"icopi"));
        if (!next(client->winnode,icopi)){
            if (e->type == ewmh->_NET_ACTIVE_WINDOW)
                 stimy_echo(setfocuswin,setfocuswin(client));
            else
                 stimy_echo(hidewin,hidewin());
            stimy_post(return;);
        }
         stimy_echo(xcb_map_window,xcb_map_window(conn,client->id));
         stimy_echo(setfocuswin,setfocuswin(client));
        stimy_post(return;);
    }
    if (e->type != ewmh->_NET_WM_STATE || e->format != 32){
         stimy_echo(debug2,debug2(__func__,"no disired client."));
        stimy_post(return;);
    }
    if(e->data.data32[1] != ewmh->_NET_WM_STATE_FULLSCREEN
        && e->data.data32[2] != ewmh->_NET_WM_STATE_FULLSCREEN){
         stimy_echo(debug2,debug2(__func__,"new client."));
        stimy_post(return;);
    }
     stimy_echo(debug2,debug2(__func__,"switch"));
    switch (e->data.data32[0])
    {
        case XCB_EWMH_WM_STATE_REMOVE:
             stimy_echo(unmaxwin,unmaxwin(client));
            break;
        case XCB_EWMH_WM_STATE_ADD:
             stimy_echo(maxwin,maxwin(client, false));
            break;
        case XCB_EWMH_WM_STATE_TOGGLE:
                if(client->maxed)
                     stimy_echo(unmaxwin,unmaxwin(client));
                else
                     stimy_echo(maxwin,maxwin(client, false));
            break;
        default: break;
    }
     stimy_echo(debug2,debug2(__func__,"ended"));
    stimy_post();
}
void
destroynotify(xcb_generic_event_t *ev)
{
    stimy_pre();
     stimy_echo(debug2,debug2(__func__,"start"));
    xcb_destroy_notify_event_t *e = (xcb_destroy_notify_event_t *) ev;
    client_t *cl,*client = NULL;
    if((client =  stimy_echo(findclient,findclient(winpi,&e->window)))){
         stimy_echo(denode,denode(client->winnode));
        if (client->id == top_win)
            top_win = 0;
         stimy_echo(killclient,killclient(client));
         stimy_echo(focusnextwin,focusnextwin());
    }
    if((client =  stimy_echo(findclient,findclient(recpi,&e->window)))){
        if (client->id == top_win)
            top_win = 0;
         stimy_echo(killclient,killclient(client));
         stimy_echo(focusnextwin,focusnextwin());
    }
     stimy_echo(debug2,debug2(__func__,"ended"));
    stimy_post();
}
void
enternotify(xcb_generic_event_t *ev)
{
    stimy_pre();
    xcb_enter_notify_event_t *e = (xcb_enter_notify_event_t *) ev;
    node_t *node = path[curpi];
    client_t *client = NULL;
    if (!node)
        stimy_post(return;);
    if(!mousefocus)
        stimy_post(return;);
     stimy_echo(debug2,debug2(__func__,"start"));
    if (e->mode != XCB_NOTIFY_MODE_NORMAL && e->mode != XCB_NOTIFY_MODE_UNGRAB)
        stimy_post(return;);
    if ((client = node->data) && e->event == client->id)
        stimy_post(return;);
    if(!(client =  stimy_echo(findclient,findclient(winpi,&e->event))))
        stimy_post(return;);
     stimy_echo(setfocuswin,setfocuswin(client));
     stimy_echo(debug2,debug2(__func__,"ended"));
    stimy_post();
}
void
mapnotify(xcb_generic_event_t *ev)
{
    stimy_pre();
    xcb_mapping_notify_event_t *e = (xcb_mapping_notify_event_t *)ev;
    xcb_key_symbols_t *keysyms;
    if (!(keysyms =  stimy_echo(xcb_key_symbols_alloc,xcb_key_symbols_alloc(conn))))
        stimy_post(return;);
     stimy_echo(xcb_refresh_keyboard_mapping,xcb_refresh_keyboard_mapping(keysyms, e));
     stimy_echo(xcb_key_symbols_free,xcb_key_symbols_free(keysyms));
     stimy_echo(setup_keyboard,setup_keyboard());
     stimy_echo(grabkeys,grabkeys());
    stimy_post();
}
void
unmapnotify(xcb_generic_event_t *ev)
{
    stimy_pre();
    xcb_unmap_notify_event_t *e = (xcb_unmap_notify_event_t *)ev;
    client_t *client = NULL;
    stimy_post();
}
void
confignotify(xcb_generic_event_t *ev)
{
    stimy_pre();
    xcb_configure_notify_event_t *e= (xcb_configure_notify_event_t *)ev;
    if(e->window != screen->root)
        stimy_post(return;);
    if (e->width==screen->width_in_pixels && e->height == screen->height_in_pixels)
        stimy_post(return;);
    screen->width_in_pixels = e->width;
    screen->height_in_pixels = e->height;
    if (-1 == randrbase)
         stimy_echo(arrangewindows,arrangewindows());
    stimy_post();
}
void
run(void)
{
    stimy_pre();
    sigcode = 0;
    while (0 == sigcode){
         stimy_echo(xcb_flush,xcb_flush(conn));
        if (xcb_connection_has_error(conn)){
             stimy_echo(cleanup,cleanup());
             stimy_echo(abort,abort());
        }
        if (!(ev =  stimy_echo(xcb_wait_for_event,xcb_wait_for_event(conn))))
            continue;
        if (events[ev->response_type & ~0x80])
            events[ev->response_type & ~0x80](ev);
        if(top_win!=0)
             stimy_echo(raisewindow,raisewindow(top_win));
         stimy_echo(delocate,delocate(ev));
    }
    if(sigcode != SIGHUP)
        stimy_post(return;);
    sigcode = 0;
    stimy_post();
}
xcb_atom_t
getatom(const char *atom_name)
{
    stimy_pre();
    xcb_intern_atom_cookie_t atom_cookie =
     stimy_echo(xcb_intern_atom,xcb_intern_atom(conn, 0,  stimy_echo(strlen,strlen(atom_name)), atom_name));
    xcb_intern_atom_reply_t *rep =  stimy_echo(xcb_intern_atom_reply,xcb_intern_atom_reply(conn, atom_cookie,NULL));
    if (NULL == rep)
        stimy_post(return 0;);
    xcb_atom_t atom = rep->atom;
     stimy_echo(delocate,delocate(rep));
    stimy_post(return atom;);
    stimy_post();
}
void
grabbuttons(client_t *c)
{
    stimy_pre();
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
             stimy_echo(xcb_grab_button,xcb_grab_button(conn, 1, c->id,
                    XCB_EVENT_MASK_BUTTON_PRESS,
                    XCB_GRAB_MODE_ASYNC,
                    XCB_GRAB_MODE_ASYNC,
                    screen->root, XCB_NONE,
                    buttons[b].button,
                    buttons[b].mask|modifiers[m]));
        }
    }
    stimy_post();
}
void
ewmh_init(void)
{
    stimy_pre();
     stimy_echo(alocate,alocate((void**)&ewmh,sizeof(xcb_ewmh_connection_t)));
    xcb_intern_atom_cookie_t *cookie =  stimy_echo(xcb_ewmh_init_atoms,xcb_ewmh_init_atoms(conn, ewmh));
    if(!xcb_ewmh_init_atoms_replies(ewmh, cookie, (void *)0)){
         stimy_echo(fprintf,fprintf(stderr,"%s\n","xcb_ewmh_init_atoms_replies:faild."));
         stimy_echo(exit,exit(1));
    }
    stimy_post();
}
bool
setup(int scrno)
{
    stimy_pre();
    unsigned int i;
    uint32_t event_mask_pointer[] = { XCB_EVENT_MASK_POINTER_MOTION };
    unsigned int values[1] = {
        XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT
        | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY
        | XCB_EVENT_MASK_PROPERTY_CHANGE
        | XCB_EVENT_MASK_BUTTON_PRESS
    };
    if(!(screen =  stimy_echo(xcb_screen_of_display,xcb_screen_of_display(conn, scrno))))
        stimy_post(return false;);
     stimy_echo(ewmh_init,ewmh_init());
     stimy_echo(xcb_ewmh_set_wm_pid,xcb_ewmh_set_wm_pid(ewmh, screen->root,  stimy_echo(getpid,getpid())));
     stimy_echo(xcb_ewmh_set_wm_name,xcb_ewmh_set_wm_name(ewmh, screen->root, 4, "2bwm"));
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
     stimy_echo(xcb_ewmh_set_supported,xcb_ewmh_set_supported(ewmh, scrno, LENGTH(net_atoms), net_atoms));
    xcb_xrm_database_t* db =  stimy_echo(xcb_xrm_database_from_default,xcb_xrm_database_from_default(conn));
    conf.borderwidth            = borders[1];
    conf.outer_border         = borders[0];
    conf.focuscol                 =  stimy_echo(getcolor,getcolor(colors[0]));
    conf.unfocuscol             =  stimy_echo(getcolor,getcolor(colors[1]));
    conf.fixedcol                 =  stimy_echo(getcolor,getcolor(colors[2]));
    conf.unkillcol                =  stimy_echo(getcolor,getcolor(colors[3]));
    conf.outer_border_col =  stimy_echo(getcolor,getcolor(colors[5]));
    conf.fixed_unkil_col    =  stimy_echo(getcolor,getcolor(colors[4]));
    conf.empty_col                =  stimy_echo(getcolor,getcolor(colors[6]));
    conf.inverted_colors    = inverted_colors;
    conf.enable_compton     = false;
    conf.selected_col =  stimy_echo(getcolor,getcolor(colors[7]));
    if(db)
    {
        char* value;
        if (xcb_xrm_resource_get_string(db,
        "twobwm.border_width", NULL, &value) >= 0)
            conf.borderwidth =  stimy_echo(atoi,atoi(value));
        if (xcb_xrm_resource_get_string(db,
        "twobwm.outer_border", NULL, &value) >= 0)
            conf.outer_border =  stimy_echo(atoi,atoi(value));
        if (xcb_xrm_resource_get_string(db,
        "twobwm.focus_color", NULL, &value) >= 0)
            conf.focuscol =  stimy_echo(getcolor,getcolor(value));
        if (xcb_xrm_resource_get_string(db,
        "twobwm.selected_color", NULL, &value) >= 0)
            conf.selected_col =  stimy_echo(getcolor,getcolor(value));
        if (xcb_xrm_resource_get_string(db,
        "twobwm.unfocus_color", NULL, &value) >= 0)
            conf.unfocuscol =  stimy_echo(getcolor,getcolor(value));
        if (xcb_xrm_resource_get_string(db,
        "twobwm.fixed_color", NULL, &value) >= 0)
            conf.fixedcol =  stimy_echo(getcolor,getcolor(value));
        if (xcb_xrm_resource_get_string(db,
        "twobwm.unkill_color", NULL, &value) >= 0)
            conf.unkillcol =  stimy_echo(getcolor,getcolor(value));
        if (xcb_xrm_resource_get_string(db,
        "twobwm.outer_border_color", NULL, &value) >= 0)
            conf.outer_border_col =  stimy_echo(getcolor,getcolor(value));
        if (xcb_xrm_resource_get_string(db,
        "twobwm.fixed_unkill_color", NULL, &value) >= 0)
            conf.fixed_unkil_col =  stimy_echo(getcolor,getcolor(value));
        if (xcb_xrm_resource_get_string(db,
        "twobwm.inverted_colors", NULL, &value) >= 0)
            conf.inverted_colors =  stimy_echo(strcmp,strcmp(value, "true")) == 0;
        if (xcb_xrm_resource_get_string(db,
        "twobwm.enable_compton", NULL, &value) >= 0)
            conf.enable_compton =  stimy_echo(strcmp,strcmp(value, "true")) == 0;
    }
     stimy_echo(xcb_xrm_database_free,xcb_xrm_database_free(db));
    for (i=0; i<NB_ATOMS; i++)
        ATOM[i] =  stimy_echo(getatom,getatom(atomnames[i][0]));
    randrbase =  stimy_echo(setuprandr,setuprandr());
    if (!setupscreen())
        stimy_post(return false;);
     stimy_echo(changews_help,changews_help(0));
    if (!setup_keyboard())
        stimy_post(return false;);
    xcb_generic_error_t *error =  stimy_echo(xcb_request_check,xcb_request_check(conn,xcb_change_window_attributes_checked(conn, screen->root,XCB_CW_EVENT_MASK, values)));
     stimy_echo(xcb_flush,xcb_flush(conn));
    if (error){
         stimy_echo(fprintf,fprintf(stderr,"%s\n","xcb_request_check:faild."));
         stimy_echo(delocate,delocate(error));
        stimy_post(return false;);
    }
     stimy_echo(xcb_ewmh_set_current_desktop,xcb_ewmh_set_current_desktop(ewmh, scrno, curwsi));
     stimy_echo(xcb_ewmh_set_number_of_desktops,xcb_ewmh_set_number_of_desktops(ewmh, scrno, WORKSPACES));
     stimy_echo(grabkeys,grabkeys());
    for (i=0; i<XCB_NO_OPERATION; i++)
        events[i] = NULL;
    events[XCB_CONFIGURE_REQUEST]   = configurerequest;
    events[XCB_DESTROY_NOTIFY]      = destroynotify;
    events[XCB_ENTER_NOTIFY]        = enternotify;
    events[XCB_KEY_PRESS]           = handle_keypressrequest;
    events[XCB_MAP_REQUEST]         = newwinrequest;
    events[XCB_UNMAP_NOTIFY]        = unmapnotify;
    events[XCB_MAPPING_NOTIFY]      = mapnotify;
    events[XCB_CONFIGURE_NOTIFY]    = confignotify;
    events[XCB_CIRCULATE_REQUEST]   = circulaterequest;
    events[XCB_BUTTON_PRESS]        = buttonpressrequest;
    events[XCB_CLIENT_MESSAGE]      = clientmessage;
    stimy_post(return true;);
    stimy_post();
}
void
twobwm_exit()
{
    stimy_pre();
     stimy_echo(exit,exit(EXIT_SUCCESS));
    stimy_post();
}
void
sigcatch(const int sig)
{
    stimy_pre();
    sigcode = sig;
    stimy_post();
}
void
install_sig_handlers(void)
{
    stimy_pre();
    struct sigaction sa;
    struct sigaction sa_chld;
    sa.sa_handler = SIG_IGN;
     stimy_echo(sigemptyset,sigemptyset(&sa.sa_mask));
    sa.sa_flags = SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
         stimy_echo(exit,exit(-1));
    sa.sa_handler = sigcatch;
     stimy_echo(sigemptyset,sigemptyset(&sa.sa_mask));
    sa.sa_flags = SA_RESTART;
    if (  stimy_echo(sigaction,sigaction(SIGINT, &sa, NULL)) == -1
        ||  stimy_echo(sigaction,sigaction(SIGHUP, &sa, NULL)) == -1
        ||  stimy_echo(sigaction,sigaction(SIGTERM, &sa, NULL)) == -1)
         stimy_echo(exit,exit(-1));
    stimy_post();
}
int
main(int argc, char **argv)
{
    stimy_pre();
    static const char *termcmd[] = {"/usr/local/bin/urxvt",NULL};
    Arg arg1 = { .com=termcmd};
    int scrno = 0;
    if(!getenv("DISPLAY")){
         stimy_echo(fprintf,fprintf(stderr,"DISPLAY %s\n",getenv("DISPLAY")));
         stimy_echo(exit,exit(1));
    }
     stimy_echo(atexit,atexit(cleanup));
     stimy_echo(install_sig_handlers,install_sig_handlers());
     stimy_echo(cube_init,cube_init());
     stimy_echo(initallpath,initallpath());
    if (!xcb_connection_has_error(conn =  stimy_echo(xcb_connect,xcb_connect(NULL, &scrno))))
        if (setup(scrno)) {
             stimy_echo(start,start(&arg1));
             stimy_echo(run,run());
        }
     stimy_echo(exit,exit(sigcode));
    stimy_post();
}
