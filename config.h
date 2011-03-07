/* modifier 0 means no modifier */
static char   *useragent        = "Surf/"VERSION" (X11; U; Unix; en-US) AppleWebKit/531.2+ Compatible (Safari)";
static char   *progress         = "#000000";
static char   *progress_untrust = "#FF0000";
static char   *progress_trust   = "#00FF00";
static char   *stylefile        = ".surf/style.css";
static char   *scriptfile       = ".surf/script.js";
static char   *cookiefile       = ".surf/cookies.txt";
static char   *cafile           = "/etc/ssl/certs/ca-certificates.crt";
static char   *strictssl        = FALSE;
static time_t sessiontime       = 3600;
#define NOBACKGROUND 0
#define SURF_SIZE_X 1024
#define SURF_SIZE_Y 600

#define SETPROP(p, q)     { .v = (char *[]){ "/bin/sh", "-c", \
	"prop=\"`xprop -id $2 $0 | cut -d '\"' -f 2 | dmenu -fn '-monotype-andale mono-medium-r-normal--0-0-0-0-c-0-iso8859-1' |"\
    "sed -e 's!^ !shortcuts.haller.ws/?!' -e 's/ /+/g' `\" &&" \
	"xprop -id $2 -f $1 8s -set $1 \"$prop\"", \
	p, q, winid, NULL } }
#define DOWNLOAD(d) { \
	.v = (char *[]){ "/bin/sh", "-c", \
	"xterm -geometry 400x100 -e \"echo $PWD; echo wget --load-cookies ~/.surf/cookies.txt '$0'; /bin/bash\"", \
	d, NULL } }
#define MODKEY GDK_CONTROL_MASK

static void goodbye(Client *c, const Arg *arg ) {
	exit(0);
}
static void ttfn(Client *c, const Arg *arg ) {
	destroyclient(c);
}
static void n00b(Client *c, const Arg *arg ) {
	newclient();
}

static Key keys[] = {
    /* modifier               keyval         function   arg                                */
    {  MODKEY,                GDK_t,         n00b,      {0}                                },
    {  MODKEY|GDK_SHIFT_MASK, GDK_r,         reload,    {.b=TRUE}                          },
    {  MODKEY,                GDK_r,         reload,    {.b=FALSE}                         },
    {  MODKEY|GDK_SHIFT_MASK, GDK_p,         print,     {0}                                },
    {  MODKEY,                GDK_p,         clipboard, {.b=TRUE}                          },
    {  MODKEY,                GDK_q,         ttfn,      {0}                                },
    {  MODKEY|GDK_SHIFT_MASK, GDK_q,         goodbye,   {0}                                },
    {  MODKEY,                GDK_y,         clipboard, {.b=FALSE}                         },
    {  MODKEY|GDK_SHIFT_MASK, GDK_j,         zoom,      {.i=-1}                            },
    {  MODKEY|GDK_SHIFT_MASK, GDK_k,         zoom,      {.i=+1}                            },
    {  MODKEY|GDK_SHIFT_MASK, GDK_i,         zoom,      {.i=0}                             },
    {  MODKEY,                GDK_h,         navigate,  {.i=-1}                            },
    {  MODKEY,                GDK_BackSpace, navigate,  {.i=-1}                            },
    {  MODKEY,                GDK_l,         navigate,  {.i=+1}                            },
    {  MODKEY,                GDK_j,         scroll,    {.i=+1}                            },
    {  MODKEY,                GDK_k,         scroll,    {.i=-1}                            },
    {  0,                     GDK_Escape,    stop,      {0}                                },
    {  MODKEY,                GDK_o,         source,    {0}                                },
    {  MODKEY,                GDK_g,         spawn,     SETPROP("_SURF_URI","_SURF_GO")    },
    {  MODKEY,                GDK_slash,     spawn,     SETPROP("_SURF_FIND","_SURF_FIND") },
    {  MODKEY,                GDK_n,         find,      {.b=TRUE}                          },
    {  GDK_SHIFT_MASK,        GDK_n,         find,      {.b=FALSE}                         },
};
