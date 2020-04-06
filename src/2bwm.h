#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <xcb/randr.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/xcb_xrm.h>
#include <X11/keysym.h>
#include "definitions.h"
#include "path.h"
#include "types.h"

/* Global Variables */
static xcb_generic_event_t *ev = NULL;
static void (*events[XCB_NO_OPERATION])(xcb_generic_event_t *e);
static unsigned int numlockmask = 0;
static int sigcode = 0;
static xcb_connection_t *conn = NULL;
static xcb_ewmh_connection_t *ewmh = NULL;
static xcb_screen_t *screen = NULL;
static int randrbase = 0;
static xcb_drawable_t top_win=0;
static const char *atomnames[NB_ATOMS][1] = {
    {"WM_DELETE_WINDOW"},
    {"WM_CHANGE_STATE"}
};
static xcb_atom_t ATOM[NB_ATOMS];

/* Setup Teardown */
static bool setup(int);
static void start(const Arg *);
static void run(void);
static void twobwm_exit();
static void install_sig_handlers(void);
static void sigcatch(const int);
static bool setup_keyboard(void);
static bool setupscreen(void);
static int setuprandr(void);
static void getrandr(void);
static xcb_atom_t getatom(const char *);
static xcb_screen_t *xcb_screen_of_display(xcb_connection_t *, int);
static uint32_t getwmdesktop(xcb_drawable_t);
static uint32_t getcolor(const char *);
static void getoutputs(xcb_randr_output_t *,const int, xcb_timestamp_t);
static void changeoutputs(xcb_randr_output_t *,const int, xcb_timestamp_t);
static bool getgeom(const xcb_drawable_t*,int16_t*,int16_t*,uint16_t*,uint16_t*,uint8_t*);
static void ewmh_init(void);
static void cleanup(void);

/* Monitor Manipulation */
static void arrbymon(monitor_t *);
static monitor_t *findmonitor(xcb_randr_output_t);
static monitor_t *findmonclones(xcb_randr_output_t, const int16_t, const int16_t);
static monitor_t *findmonbycoord(const int16_t, const int16_t);
static void delmonitor(monitor_t *);
static monitor_t *addmonitor(xcb_randr_output_t, xcb_randr_get_crtc_info_reply_t*);
static void getmonsize(bool,int16_t*,int16_t*,uint16_t*,uint16_t*,const client_t*);

/* Window Manipulation */
static void selectwindows(const Arg *);
static void selectallwindows(const Arg *);
static inline void windowsize(const Arg *);
static inline void windowposition(client_t *);
static void arrangewindows(void);
static void raise_current_window(void);
static client_t *setupwin(xcb_window_t);
static void subwindowposition(client_t*);
static void fixwindow(node_t*);
static void unkillablewindow(node_t*);
static void raisewindow(xcb_drawable_t);
static void movewindow(xcb_drawable_t, const int16_t, const int16_t);
static void configwin(xcb_window_t, uint16_t,const struct winconf *);
static void snapwindow(client_t *);
static void maxwin(client_t *, bool);
static void unmaxwin(client_t *);
static void deletewin();
static inline void deletewin_help();
static client_t create_back_win(void);
static void focuslocationwin(const Arg *);
static void focusnextwin();
static void setfocuswin(client_t *);
static inline void swapwin();
static inline void swapwin_helper(client_t*);
static inline void sidebysidewin(const Arg *);
static inline void sidebysidewin_helper(client_t*);
static void teleportwin(const Arg *);
static inline void teleportwin_helper(const Arg *,client_t*);
static void maximizewin(const Arg *);
static void fullscreenwin(const Arg *);
static void maximizewin_helper(client_t *,uint16_t, uint16_t, uint16_t, uint16_t);
static void hidewin();
static void hidewin_helper(client_t*);
static void unhidewin();
static void unhidewin_helper(client_t*);
static void resizestepwin(const Arg *);
static void resizestepwin_aspect(const Arg *);
static void movestepwin(const Arg *);
static void maxvertwin_hor(const Arg *);
static void maxhalfwin(const Arg *);
static void raiseorlowerwin();
static void movelimwin(client_t *);
static void resizelimwin(client_t *);
static void resizewin(xcb_drawable_t, const uint16_t, const uint16_t);
static void moveresizewin(xcb_drawable_t,const uint16_t,const uint16_t,const uint16_t,const uint16_t);
static void fixwin();
static void always_on_topwin();
static void unmaxwin_helper(client_t *);
static inline void setborderswin(client_t *,const bool);
static void fitonscreenwin(client_t *);
static void unkillablewin();
static void noborderwin(int16_t *,client_t *, bool);
static void check_namewin(client_t *);
static void changescreenwin(const Arg *);

/* Workspace Manipulation */
static void sendtoworkspace(const Arg *);
static void sendtonextworkspace(const Arg *);
static void sendtoprevworkspace(const Arg *);
static void prevworkspace();
static void nextworkspace();
static void changeworkspace(const Arg *);
static void changews_help(const uint32_t);
static inline void delfromworkspace(node_t*, uint32_t);
static inline void addtoworkspace(client_t *, uint32_t);

/* Events Handler */
static void newwinrequest(xcb_generic_event_t *);
static void clientmessage(xcb_generic_event_t *);
static void configurerequest(xcb_generic_event_t *);
static void buttonpressrequest(xcb_generic_event_t *);
static void unmapnotify(xcb_generic_event_t *);
static void destroynotify(xcb_generic_event_t *);
static void circulaterequest(xcb_generic_event_t *);
static void handle_keypressrequest(xcb_generic_event_t *);

/* Client Manipulation */
static void addtoclientlist(const xcb_drawable_t);
static void showclientlist(int);
static inline void killclient(client_t*);
static client_t *findclient(int,const xcb_drawable_t *);

/* Mouse, Cursor, Keycodes */
static void mouseresize(client_t *,const int16_t,const int16_t);
static void mousemove(const int16_t,const int16_t);
static void mousemotion(const Arg *);
static void cursor_move(const Arg *);
static void grabkeys(void);
static void grabbuttons(client_t *);
static void centerpointer(xcb_drawable_t, client_t *);
static bool getpointer(const xcb_drawable_t *, int16_t *,int16_t *);
static void movepointerback(const int16_t, const int16_t, const client_t *);
static xcb_cursor_t create_font_cursor(xcb_connection_t *, uint16_t);
static xcb_keycode_t *xcb_get_keycodes(xcb_keysym_t);

#include "config.h"
