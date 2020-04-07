///---User configurable stuff---///
///---Modifiers---///
// Super/Windows key  or check xmodmap(1) with -pm  defined in xproto.h //
#define MOD             XCB_MOD_MASK_4
/* winodw sizes */
enum{a4paper = 0,a8paper = 1,a3paper = 2, a5paper = 3};
static const uint16_t windowwidth[] = {1155,288,2310,674};
static const uint16_t windowheight[] = {1630,407,1630,760};

// enable/disable mouse focus.
static bool mousefocus = false;

/* moving or resizing with keyboard, unless the window has hints saying otherwise.
 *0)move step slow   1)move step fast
 *2)mouse slow       3)mouse fast
 */
static const uint16_t movements[] = {20,40,15,400};
/* resize by line like in mcwm -- jmbi */

static const bool resize_by_line = true;
/* the ratio used when resizing and keeping the aspect */
static const float resize_keep_aspect_ratio = 1.03;
///---Offsets---///
/*0)offsetx          1)offsety
 *2)maxwidth         3)maxheight */
static const uint8_t offsets[] = {0,0,0,0};
///---Colors---///
/*0)focuscol         1)unfocuscol
 *2)fixedcol         3)unkilcol
 *4)fixedunkilcol    5)outerbordercol
 *6)emptycol         7)selected_col 00c473
 */
static const char *colors[] = {
"#888a85",
"#333333",
"#7a8c5c",
"#ff6666",
"#cc9933",
"#0d131a",
"#000000",
"#de8787"
};
/* if this is set to true the inner border and outer borders colors will be swapped */
static const bool inverted_colors = true;
///---Cursor---///
/* default position of the cursor:
 * correct values are:
 * TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT, MIDDLE
 * All these are relative to the current window.
 */
#define CURSOR_POSITION MIDDLE
///---Borders---///
/*0) Outer border size. If you put this negative it will be a square.
 *1) Full borderwidth    2) Magnet border size
 *3) Resize border size
 */
static const uint8_t borders[] = {3,5,5,4};

/* Windows that won't have a border.
 * It uses substring comparison with what is found in the WM_NAME
 * attribute of the window. You can test this using `xprop WM_NAME`
 */
#define LOOK_INTO "WM_NAME"
static const char *ignore_names[] = {"bar", "xclock"};
///--Menus and Programs---//
static const char *menucmd[] = { "/usr/local/bin/menu", NULL };
static const char *testcmd[] = { "/usr/local/bin/2bwm.verify", NULL };
static const char *termcmd[] = { "/bin/urxvt","-b","40","+sb","-lsp","8","-g","90x40+1400+300", NULL };
///--Custom foo---///
static void halfandcentered(const Arg *arg)
{
	Arg arg2 = {.i=TWOBWM_MAXHALF_VERTICAL_LEFT};
	maxhalfwin(&arg2);
	Arg arg3 = {.i=TWOBWM_TELEPORT_CENTER};
	teleportwin(&arg3);
}

///---Shortcuts---///
/* Check /usr/include/X11/keysymdef.h for the list of all keys
 * 0x000000 is for no modkey
 * If you are having trouble finding the right keycode use the `xev` to get it
 * For example:
 * KeyRelease event, serial 40, synthetic NO, window 0x1e00001,
 *  root 0x98, subw 0x0, time 211120530, (128,73), root:(855,214),
 *  state 0x10, keycode 171 (keysym 0x1008ff17, XF86AudioNext), same_screen YES,
 *  XLookupString gives 0 bytes: 
 *  XFilterEvent returns: False
 *
 *  The keycode here is keysym 0x1008ff17, so use  0x1008ff17
 *
 *
 * For AZERTY keyboards XK_1...0 should be replaced by :
 *      DESKTOPCHANGE(     XK_ampersand,                     0)
 *      DESKTOPCHANGE(     XK_eacute,                        1)
 *      DESKTOPCHANGE(     XK_quotedbl,                      2)
 *      DESKTOPCHANGE(     XK_apostrophe,                    3)
 *      DESKTOPCHANGE(     XK_parenleft,                     4)
 *      DESKTOPCHANGE(     XK_minus,                         5)
 *      DESKTOPCHANGE(     XK_egrave,                        6)
 *      DESKTOPCHANGE(     XK_underscore,                    7)
 *      DESKTOPCHANGE(     XK_ccedilla,                      8)
 *      DESKTOPCHANGE(     XK_agrave,                        9)*
 */
#define DESKTOPCHANGE(K,N) \
{  MOD ,             K,              changeworkspace, {.i=N}}, \
{  MOD |SHIFT,       K,              sendtoworkspace, {.i=N}},
static key keys[] = {
/* modifier           key            function           argument */
// Focus to next/previous window
{  MOD ,              XK_Tab,        focusnextwin,        {}},
{  MOD |SHIFT,        XK_Tab,      focuslocationwin,        {.i=TWOBWM_FOCUS_LOCATION}},
{  MOD |CONTROL,      XK_Tab,      focuslocationwin,    {.i=TWOBWM_FOCUS_LOCATION}},
{  MOD |CONTROL,      XK_4,          windowsize,               {.i=a4paper}},
{  MOD |CONTROL,      XK_5,          windowsize,               {.i=a5paper}},
{  MOD |CONTROL,      XK_3,          windowsize,               {.i=a3paper}},
{  MOD |CONTROL,      XK_8,          windowsize,               {.i=a8paper}},

// Kill a window
{  MOD,              XK_p,            deletewin,         {}},
// Resize a window
{  MOD |SHIFT,        XK_k,          resizestepwin,        {.i=TWOBWM_RESIZE_UP}},
{  MOD |SHIFT,        XK_j,          resizestepwin,        {.i=TWOBWM_RESIZE_DOWN}},
{  MOD |SHIFT,        XK_l,          resizestepwin,        {.i=TWOBWM_RESIZE_RIGHT}},
{  MOD |SHIFT,        XK_h,          resizestepwin,        {.i=TWOBWM_RESIZE_LEFT}},
// Resize a window slower
{  MOD |SHIFT|CONTROL,XK_k,      resizestepwin,        {.i=TWOBWM_RESIZE_UP_SLOW}},
{  MOD |SHIFT|CONTROL,XK_j,      resizestepwin,        {.i=TWOBWM_RESIZE_DOWN_SLOW}},
{  MOD |SHIFT|CONTROL,XK_l,      resizestepwin,        {.i=TWOBWM_RESIZE_RIGHT_SLOW}},
{  MOD |SHIFT|CONTROL,XK_h,      resizestepwin,        {.i=TWOBWM_RESIZE_LEFT_SLOW}},
// Move a window
{  MOD ,              XK_k,          movestepwin,          {.i=TWOBWM_MOVE_UP}},
{  MOD ,              XK_j,          movestepwin,          {.i=TWOBWM_MOVE_DOWN}},
{  MOD ,              XK_l,          movestepwin,          {.i=TWOBWM_MOVE_RIGHT}},
{  MOD ,              XK_h,          movestepwin,          {.i=TWOBWM_MOVE_LEFT}},
// Move a window slower
{  MOD |CONTROL,      XK_k,          movestepwin,          {.i=TWOBWM_MOVE_UP_SLOW}},
{  MOD |CONTROL,      XK_j,          movestepwin,         {.i=TWOBWM_MOVE_DOWN_SLOW}},
{  MOD |CONTROL,      XK_l,          movestepwin,        {.i=TWOBWM_MOVE_RIGHT_SLOW}},
{  MOD |CONTROL,      XK_h,          movestepwin,        {.i=TWOBWM_MOVE_LEFT_SLOW}},
// Teleport the window to an area of the screen.
// Center:
{  MOD ,              XK_s,         sidebysidewin,   {.i=TWOBWM_TELEPORT_SIDEBYSIDE}},
{  MOD |SHIFT,        XK_s,         swapwin,           {}},
// Center y:
{  MOD,        XK_y,          teleportwin,      {.i=TWOBWM_TELEPORT_CENTER_Y}},
{  MOD,        XK_c,          teleportwin,      {.i=TWOBWM_TELEPORT_CENTER_Y}},
// Center x:
{  MOD ,     XK_space,      teleportwin,   {.i=TWOBWM_TELEPORT_CENTER_X}},
// left:
{  MOD ,             XK_g,          teleportwin,     {.i=TWOBWM_TELEPORT_TOP_LEFT}},
{  MOD |CONTROL,     XK_g,          teleportwin,     {.i=TWOBWM_TELEPORT_BOTTOM_LEFT}},
{  MOD ,              XK_u,          teleportwin,      {.i=TWOBWM_TELEPORT_TOP}},
{  MOD ,              XK_b,          teleportwin,      {.i=TWOBWM_TELEPORT_BOTTOM}},
// right:
{  MOD ,              XK_n,          teleportwin,   {.i=TWOBWM_TELEPORT_BOTTOM_RIGHT}},
{  MOD |CONTROL,      XK_n,          teleportwin,   {.i=TWOBWM_TELEPORT_TOP_RIGHT}},
// Resize while keeping the window aspect
{  MOD ,         XK_r,    resizestepwin_aspect, {.i=TWOBWM_RESIZE_KEEP_ASPECT_GROW}},
{  MOD |SHIFT,   XK_r,    resizestepwin_aspect, {.i=TWOBWM_RESIZE_KEEP_ASPECT_SHRINK}},
// Maximize (ignore offset and no EWMH atom)
{  MOD ,              XK_z,          maximizewin,          {}},
// Full screen (disregarding offsets and adding EWMH atom)
{  MOD |SHIFT ,       XK_z,          fullscreenwin,        {}},
// Maximize vertically
{  MOD ,              XK_d,          maxvertwin_hor,       {.i=TWOBWM_MAXIMIZE_VERTICALLY}},
// Maximize horizontally
{  MOD |SHIFT,        XK_d,          maxvertwin_hor,       {.i=TWOBWM_MAXIMIZE_HORIZONTALLY}},
// Maximize and move
// vertically left
{  MOD |SHIFT,        XK_y,          maxhalfwin,           {.i=TWOBWM_MAXHALF_VERTICAL_LEFT}},
// vertically right
{  MOD |SHIFT,        XK_u,          maxhalfwin,           {.i=TWOBWM_MAXHALF_VERTICAL_RIGHT}},
// horizontally left
{  MOD |SHIFT,        XK_b,          maxhalfwin,           {.i=TWOBWM_MAXHALF_HORIZONTAL_BOTTOM}},
// horizontally right
{  MOD |SHIFT,        XK_n,          maxhalfwin,           {.i=TWOBWM_MAXHALF_HORIZONTAL_TOP}},
//fold half vertically
{  MOD |SHIFT|CONTROL,XK_y,          maxhalfwin,           {.i=TWOBWM_MAXHALF_FOLD_VERTICAL}},
//fold half horizontally
{  MOD |SHIFT|CONTROL,XK_b,          maxhalfwin,           {.i=TWOBWM_MAXHALF_FOLD_HORIZONTAL}},
//unfold vertically
{  MOD |SHIFT|CONTROL,XK_u,          maxhalfwin,           {.i=TWOBWM_MAXHALF_UNFOLD_VERTICAL}},
//unfold horizontally
{  MOD |SHIFT|CONTROL,XK_n,          maxhalfwin,           {.i=TWOBWM_MAXHALF_UNFOLD_HORIZONTAL}},
// Next/Previous screen
{  MOD ,              XK_comma,      changescreenwin,      {.i=TWOBWM_NEXT_SCREEN}},
{  MOD ,              XK_period,     changescreenwin,      {.i=TWOBWM_PREVIOUS_SCREEN}},
// Next/Previous workspace
//    {  MOD ,              XK_v,          nextworkspace,     {}},
//    {  MOD ,              XK_c,          prevworkspace,     {}},
// Move to Next/Previous workspace
// {  MOD |SHIFT ,       XK_v,          sendtonextworkspace,{}},
// {  MOD |SHIFT ,       XK_c,          sendtoprevworkspace,{}},
// Iconify the window
// Make the window unkillable
{  MOD ,              XK_a,          unkillablewin,        {}},
// Make the window appear always on top
{  MOD,               XK_t,          always_on_topwin,     {}},
// Make the window stay on all workspaces
{  MOD ,              XK_f,          fixwin,               {}},
// Move the cursor
{  MOD ,              XK_Up,         cursor_move,       {.i=TWOBWM_CURSOR_UP_SLOW}},
{  MOD ,              XK_Down,       cursor_move,       {.i=TWOBWM_CURSOR_DOWN_SLOW}},
{  MOD ,              XK_Right,      cursor_move,       {.i=TWOBWM_CURSOR_RIGHT_SLOW}},
{  MOD ,              XK_Left,       cursor_move,       {.i=TWOBWM_CURSOR_LEFT_SLOW}},
// Move the cursor faster
{  MOD |SHIFT,        XK_Up,         cursor_move,       {.i=TWOBWM_CURSOR_UP}},
{  MOD |SHIFT,        XK_Down,       cursor_move,       {.i=TWOBWM_CURSOR_DOWN}},
{  MOD |SHIFT,        XK_Right,      cursor_move,       {.i=TWOBWM_CURSOR_RIGHT}},
{  MOD |SHIFT,        XK_Left,       cursor_move,       {.i=TWOBWM_CURSOR_LEFT}},
// Start programs
{  MOD ,              XK_Return,     start,             {.com = termcmd}},
{  MOD |SHIFT,        XK_v,          start,             {.com = testcmd}},
{  MOD ,              XK_w,          start,             {.com = termcmd}},
{  MOD ,              XK_m,          start,             {.com = menucmd}},
{  MOD ,              XK_o,          selectwindows,     {.i=TWOBWM_SELECT}},
{  MOD |SHIFT,        XK_o,          selectallwindows,     {.i=TWOBWM_SELECT_ALL}},
{  MOD ,              XK_i,          hidewin,				{}},
{  MOD |SHIFT,        XK_i,          unhidewin,			{}},
// Exit
{  MOD |CONTROL,      XK_p,          twobwm_exit,       {.i=0}},
// Change current workspace
   DESKTOPCHANGE(     XK_1,                             0)
   DESKTOPCHANGE(     XK_2,                             1)
   DESKTOPCHANGE(     XK_3,                             2)
   DESKTOPCHANGE(     XK_4,                             3)
   DESKTOPCHANGE(     XK_5,                             4)
   DESKTOPCHANGE(     XK_6,                             5)
   DESKTOPCHANGE(     XK_7,                             6)
   DESKTOPCHANGE(     XK_8,                             7)
   DESKTOPCHANGE(     XK_9,                             8)
   DESKTOPCHANGE(     XK_0,                             9)
};
// the last argument makes it a root window only event
static Button buttons[] = {
    {  MOD        ,XCB_BUTTON_INDEX_1,     mousemotion,   {.i=TWOBWM_MOVE}, false},
    {  MOD        ,XCB_BUTTON_INDEX_3,     mousemotion,   {.i=TWOBWM_RESIZE}, false},
    {  0          ,XCB_BUTTON_INDEX_3,     start,         {.com = menucmd}, true},
    {  MOD|SHIFT,  XCB_BUTTON_INDEX_1,     changeworkspace, {.i=0}, false},
    {  MOD|SHIFT,  XCB_BUTTON_INDEX_3,     changeworkspace, {.i=1}, false},
    {  MOD|ALT,    XCB_BUTTON_INDEX_1,     changescreenwin,    {.i=1}, false},
    {  MOD|ALT,    XCB_BUTTON_INDEX_3,     changescreenwin,    {.i=0}, false}
};
