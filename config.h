/* See LICENSE file for copyright and license details. */

/* dwm-arp v2 based on dwm_6.1
 * patches & mods :
 * attachabove          http://dwm.suckless.org/patches/attachabove
 * bottomstack          http://dwm.suckless.org/patches/bottom_stack
 * fancybar             http://dwm.suckless.org/patches/fancybar
 * float_border_color   http://dwm.suckless.org/patches/float_border_color
 * push up/down         http://dwm.suckless.org/patches/push
 * pertag               http://dwm.suckless.org/patches/pertag
 * scratchpad           https://github.com/ok100/dwm/blob/master/03-dwm-6.0-scratchpad-stay.diff
 * cycle-layout         https://bbs.archlinux.org/viewtopic.php?id=103402
 * view_prev/next_tag   forum post
 * arpinux@2012 <http://arpinux.org>
 * 
 * tilemovemouse
 * fancybarclickable
 * closeonclick
*/

/* appearance */
static const char font[]                 = "-*-stlarch-medium-r-*-*-10-*-*-*-*-*-*-*" "," "-*-terminusmodx.icons-*-*-*--12-*-*-*-*-*-*-*";
static const char normbordercolor[]      = "#222222"; // dark grey
static const char normbgcolor[]          = "#222222";
static const char normfgcolor[]          = "#7D7D7D"; // clear grey
static const char selbordercolor[]       = "#16B600"; // green
static const char selbgcolor[]           = "#222222";
static const char selfgcolor[]           = "#C7C7C7"; // grey
static const char floatnormbordercolor[] = "#222222";
static const char floatselbordercolor[]  = "#FFA000"; // orange
static const unsigned int borderpx       = 1;         // border pixel of windows
static const unsigned int snap           = 8;        // snap pixel
static const Bool showbar                = True;      // False means no bar
static const Bool topbar                 = False;     // False means bottom bar
static const char scratchpadname[]       = "scratch"; // scratchpad window title

/* False means using the scroll wheel on a window will not change focus */
static const Bool focusonwheelscroll = False;

/* tagging */
static const char *tags[] = { "", "", "", "", "" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class          instance  title          tags mask    isfloating   monitor */
	{ "Claws-mail",    NULL,    NULL,               1<<0,   False,        0 }, // claws-mail on tag 1
	{ NULL,            NULL,    "weechat 0.3.2",    1<<2,   False,       -1 }, // weechat on tag 3
	{ NULL,            NULL,    "vim",              1<<3,   False,       -1 }, // vim on tag 4
	{ "Gimp",          NULL,    NULL,               1<<4,   True,        -1 }, // gimp free on tag 5
	{ "Filezilla",     NULL,    NULL,               1<<7,   False,       -1 }, // filezilla on tag 8
	{ "Transmission",  NULL,    NULL,               1<<8,   False,        0 }, // transmission on tag 9
	{ "XCalc",         NULL,    NULL,               0,      True,        -1 }, // xcalc free notag
	{ "Lpx2",          NULL,    NULL,               0,      True,        -1 }, // pix-viewer free notag
	{ NULL,            NULL,    "cal",			    0,      True,        -1 }, // calendar is floating
};

/* layout(s) */
static const float mfact      = 0.50;  // factor of master area size [0.05..0.95]
static const int nmaster      = 1;     // number of clients in master area
static const Bool resizehints = False; // True means respect size hints in tiled resizals

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[|]",      tile },    // [0] first entry is default
    { "[-]",      bstack },  // [1] bottomstack layout
    { "[O]",      monocle }, // [2] monocle aka maximize all client
	{ "[ ]",      NULL },    // [3] no layout function means floating behavior
    { NULL,       NULL },    // here for prev/next layout feature
};

/* key definitions */
#define MODKEY Mod1Mask
#define MODKEY2 Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

#define ICONS                   4
#define ICON_OFFSET             (ICONS*12)

/* commands */
static const char *dmenucmd[] = { "dmenu_run", "-b", "-fn", font, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, "-p", "exec:", NULL };
static const char *termcmd[]  = { "urxvtc", NULL };                                       // terminal
static const char *scratcmd[] = { "urxvtc", "-T", "scratch", "-geometry", "90x9", NULL }; // scratchpad
static const char *geanycmd[] = { "geany", NULL };                                        // gui editor

static const char *thingmenu_start[] = { "thingmenu", "-s", "-o", "-x", "-g", "+0-1", "--",  // launch thingmenu
                                         "h", "pcmanfm &",
                                         "e", "geany &",
                                         "i", "firefox &",
                                         "w", "urxvt -title wicd -e sh -c \"wicd-curses 2> /dev/null\"", NULL };

static const char *thingmenu_stop[] = { "killall",  "thingmenu", NULL };                     // kill thingmenu

static const char *clockmenu_start[] = {"clock-menu", NULL };
static const char *clockupdate_start[] = {"clock-update", NULL };

static const char *clockmenu_stop[] = {"killall", "clockmenu", "&&", "killall", "clock-update",  NULL };

/* menus */
static const char *deskmenu[] = { "compiz-deskmenu", NULL };
static const char *homecmd[]  = { "dmenu-home.sh", NULL };
static const char *googcmd[]  = { "dmenu-google.sh", NULL };

static Key keys[] = {
	/* modifier                     key           function        argument */
	/* thingmenu launchers */
    { 0,                            0,            spawn,          {.v = thingmenu_start } },
    { 0,                            0,            spawn,          {.v = thingmenu_stop  } },
    { 0,                            0,            spawn,          {.v = clockmenu_start } },
    { 0,                            0,            spawn,          {.v = clockmenu_stop  } },
    { 0,                            0,            spawn,          {.v = clockupdate_start } },
    /* menus */
    { MODKEY,                       XK_p,         spawn,          {.v = dmenucmd } },   // dmenu           Alt+p
    { MODKEY2,                      XK_d,         spawn,          {.v = dmenucmd } },   // dmenu           Super+d
    { MODKEY2,                      XK_g,         spawn,          {.v = googcmd } },    // dmenu-google    Super+g
    { MODKEY2,                      XK_h,         spawn,          {.v = homecmd } },    // dmenu-home      Super+h
    /* applications */
    { 0,                            XK_F12,       togglescratch,  {.v = scratcmd} },    // scratchpad      F12
    { ControlMask,                  XK_Return,    spawn,          {.v = termcmd } },    // terminal        Ctrl+Return
	{ MODKEY|ShiftMask,             XK_e,         spawn,          {.v = geanycmd } },   // geany           Alt+Shift+e
    /* navigation */
	{ MODKEY,                       XK_b,         togglebar,      {0} },                // toggle bar visibility          Alt+b
	{ MODKEY,                       XK_j,         focusstack,     {.i = +1 } },         // focus next client              Alt+j
	{ MODKEY,                       XK_Tab,       focusstack,     {.i = +1 } },         // focus next client              Alt+Tab
	{ MODKEY,                       XK_k,         focusstack,     {.i = -1 } },         // focus previous client          Alt+k
	{ MODKEY|ShiftMask,             XK_Tab,       focusstack,     {.i = -1 } },         // focus previous client          Alt+Shift+Tab
    { MODKEY|ControlMask,           XK_j,         pushdown,       {0} },                // move client down in clientlist Ctrl+Alt+j
    { MODKEY|ControlMask,           XK_k,         pushup,         {0} },                // move client up in clientlist   Ctrl+Alt+k
	{ MODKEY,                       XK_i,         incnmaster,     {.i = +1 } },         // increase client in master      Alt+i
	{ MODKEY,                       XK_d,         incnmaster,     {.i = -1 } },         // decrease clients in master     Alt+d
	{ MODKEY,                       XK_h,         setmfact,       {.f = -0.05} },       // decrease master area           Alt+h
	{ MODKEY,                       XK_l,         setmfact,       {.f = +0.05} },       // increase master area           Alt+l
	{ MODKEY|ShiftMask,             XK_h,         setmfact,       {.f = -0.001} },      // decrease master area by 1px    Shift+Alt+h
	{ MODKEY|ShiftMask,             XK_l,         setmfact,       {.f = +0.001} },      // increase master area by 1px    Shift+Alt+l
	{ MODKEY,                       XK_Return,    zoom,           {0} },                // put client in master           Alt+Return
	{ MODKEY,                       XK_q,         killclient,     {0} },                // kill client                    Alt+q
	{ MODKEY|ShiftMask,             XK_q,         killclients,    {0} },                // kill clients in workspace      Ctrl+Alt+q
	{ MODKEY,                       XK_t,         setlayout,      {.v = &layouts[0]} }, // set layout 0 tile              Alt+t
	{ MODKEY,                       XK_s,         setlayout,      {.v = &layouts[1]} }, // set layout 1 bstack            Alt+s
	{ MODKEY,                       XK_m,         setlayout,      {.v = &layouts[2]} }, // set layout 2 monocle           Alt+m
    { MODKEY,                       XK_f,         setlayout,      {.v = &layouts[3]} }, // set layout 3 free              Alt+f
	{ MODKEY,                       XK_space,     setlayout,      {0} },                // set prev layout                Alt+Space
    { MODKEY2,                      XK_space,     nextlayout,     {0} },                // next layout                    Super+Space
	{ MODKEY2|ShiftMask,            XK_space,     prevlayout,     {0} },                // prev layout                    Super+Shift+Space
	{ MODKEY|ShiftMask,             XK_space,     togglefloating, {0} },                // toggle client floating         Alt+Shift+Space
	{ MODKEY,                       XK_agrave,    view,           {.ui = ~0 } },        // view alltags                   Alt+à
	{ MODKEY|ShiftMask,             XK_agrave,    tag,            {.ui = ~0 } },        // tag alltags                    Alt+Shift+à
	{ MODKEY,                       XK_Down,      focusmon,       {.i = -1 } },         // focus previous screen          Alt+Down
	{ MODKEY,                       XK_Up,        focusmon,       {.i = +1 } },         // focus next screen              Alt+Up
	{ MODKEY|ShiftMask,             XK_Down,      tagmon,         {.i = -1 } },         // tag previous screen            Alt+Shift+Down
	{ MODKEY|ShiftMask,             XK_Up,        tagmon,         {.i = +1 } },         // tag next screen                Alt+Shift+Up
	{ MODKEY,                       XK_Left,      view_prev_tag,  {0} },                // previous tag                   Alt+Left
	{ MODKEY,                       XK_Right,     view_next_tag,  {0} },                // next tag                       Alt+Right
	TAGKEYS(                        XK_1,                         0)
	TAGKEYS(                        XK_2,                         1)                    // view tag [n]                   Alt+[n]
    TAGKEYS(                        XK_3,                         2)                    // tag [n] client                 Alt+Shift+[n]
	TAGKEYS(                        XK_4,                         3)                    // toggleview tag [n]             Ctrl+Alt+[n]
	TAGKEYS(                        XK_5,                         4)                    // toggletag tag [n]              Atrl+Shift+Alt+[n]
	TAGKEYS(                        XK_6,                         5)
	TAGKEYS(                        XK_7,                         6)
	TAGKEYS(                        XK_8,                         7)
	TAGKEYS(                        XK_9,                         8)
//	{ MODKEY|ControlMask,           XK_q,         quit,           {0} },                 // exit/reload dwm               Alt+Control+q
};

#include "tilemovemouse.c"

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click             event mask   button      function        argument */
    /* on layout symbol */
	{ ClkLtSymbol,       0,           Button1,    setlayout,      {.v = &layouts[0]} },  // set layout 0 tile             B1 on layout symbol
	{ ClkLtSymbol,       0,           Button2,    setlayout,      {.v = &layouts[1]} },  // set layout 1 bstack           B2 on layout symbol
	{ ClkLtSymbol,       0,           Button3,    setlayout,      {.v = &layouts[2]} },  // set layout 2 monocle          B3 on layout symbol
    /* on client title */
//	{ ClkWinTitle,       0,           Button1,    zoom,           {0} },                 // put client in master          B1 on client title
    { ClkWinTitle,       0,           Button1,    focusonclick,   {0} },
    { ClkWinTitle,       0,           Button3,    closeonclick,   {0} },
	/* on client window */
    { ClkClientWin,      MODKEY,      Button1,    tilemovemouse,  {0} },                 // move client in tiles          Alt+B1 on client
	{ ClkClientWin,      MODKEY,      Button2,    togglefloating, {0} },                 // toggle floating               Alt+B2 on client
	{ ClkClientWin,      MODKEY,      Button3,    resizemouse,    {0} },                 // resize client                 Alt+B3 on client
	/* on tagbar */
    { ClkTagBar,         0,           Button1,    view,           {0} },                 // view tag [n]                  B1 on tag [n]
	{ ClkTagBar,         0,           Button3,    toggleview,     {0} },                 // toggle viewtag [n]            B3 on tag [n]
	{ ClkTagBar,         MODKEY,      Button1,    tag,            {0} },                 // tag client with tag [n]       Alt+B1 on tag [n]
	{ ClkTagBar,         MODKEY,      Button3,    toggletag,      {0} },                 // toggle tag client with [n]    Alt+B3 on tag [n]
    /* on root window */
    { ClkRootWin,        0,           Button3,    spawn,          {.v = deskmenu } },    // open deskmenu                 B3 on root window
};

/* EOF */
