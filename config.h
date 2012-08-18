/* modifier 0 means no modifier */
static char   *useragents[]     = {
	//"Surf/"VERSION" (X11; U; Unix; en-US) AppleWebKit/531.2+ Compatible (Safari)",
	"Mozilla/5.0 (X11; Linux x86_64; rv:2.0b9pre) Gecko/20110111 Firefox/4.0b9pre",
	"Mozilla/5.0 (compatible; Googlebot/2.1; +http://www.google.com/bot.html)",
};
static char   *useragent        =
	"Mozilla/5.0 (X11; Linux x86_64; rv:2.0b9pre) Gecko/20110111 Firefox/4.0b9pre";

static char   *download_dir     = "/home/phaller/tmp/downloads/";
static char   *progress         = "#8888FF";
static char   *progress_untrust = "#FF0000";
static char   *progress_trust   = "#00FF00";
static char   *stylefile        = ".surf/style.css";
static char   *scriptfile       = ".surf/script.js";
static char   *cookiefile       = ".surf/cookies.txt";
static char   *cafile           = "/etc/ssl/certs/ca-certificates.crt";
static char   *strictssl        = FALSE;
static time_t sessiontime       = 3600;
#define NOBACKGROUND 0
#define SURF_SIZE_X 1366
#define SURF_SIZE_Y 768

#define MARKS { .v = (char *[]){ "javascript:document.location.replace('http://haller.ws/marks/mark.cgi?' + document.location) " } }

#define SETPROP(p, q)     { .v = (char *[]){ "/bin/sh", "-c", \
	"prop=\"`xprop -id $2 $0 | cut -d '\"' -f 2 | dmenu.sh |"\
    "sed -e 's!^ !shortcuts.haller.ws/?!' -e 's/ /+/g' `\" &&" \
	"xprop -id $2 -f $1 8s -set $1 \"$prop\"", \
	p, q, winid, NULL } }
#define DOWNLOAD(d) { \
	.v = (char *[]){ "/bin/sh", "-c", \
	"xterm -geometry 400x100 -e \"echo $PWD; echo wget --load-cookies ~/.surf/cookies.txt '$0'; /bin/bash\"", \
	d, NULL } }
#define OPENFILE(d) { .v = (char *[]){ "/bin/bash", "-c", "[[ \"$0\" =~ pdf$ ]] && xpdf \"$0\" ", d, d, NULL } }
#define MODKEY GDK_CONTROL_MASK

static void js_eval(Client *c, const Arg *arg) {
	JSStringRef jsscript;
	JSValueRef exception = NULL;
	WebKitWebFrame *frame;

	frame = webkit_web_view_get_main_frame(c->view);
	JSContextRef js = webkit_web_frame_get_global_context(frame);
	jsscript = JSStringCreateWithUTF8CString( (((char **)arg->v)[0] ));
	JSEvaluateScript(js, jsscript, JSContextGetGlobalObject(js), NULL, 0, &exception);
}

static void goodbye(Client *c, const Arg *arg ) {
	exit(0);
}
static void ttfn(Client *c, const Arg *arg ) {
	destroyclient(c);
}
static void n00b(Client *c, const Arg *arg ) {
	newclient();
}
static void tricksy( Client *c, const Arg *arg) {
	WebKitWebSettings *settings;
	gboolean is_tricksy = *(gboolean *)arg;

	settings = webkit_web_view_get_settings(c->view);
	if (is_tricksy)
		useragent = useragents[1];
	else
		useragent = useragents[0];
	g_object_set(G_OBJECT(settings), "user-agent", useragent, NULL);
}

void
scrolli(GtkAdjustment *a, const Arg *arg) {
	gdouble v;

	v = gtk_adjustment_get_value(a);
	switch (arg->i){
		case +10000:
		case -10000:
			v += gtk_adjustment_get_page_increment(a) * (arg->i / 10000); break;
		case +20000:
		case -20000:
		default:
			v += gtk_adjustment_get_step_increment(a) * arg->i;
	}
	v = MAX(v, 0.0);
	v = MIN(v, gtk_adjustment_get_upper(a) - gtk_adjustment_get_page_size(a));
	gtk_adjustment_set_value(a, v);
}

void
scroll_h(Client *c, const Arg *arg) {
	scrolli(gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(c->scroll)), arg);
}

static Key keys[] = {
    /* modifier               keyval         function   arg                                */
    {  MODKEY|GDK_SHIFT_MASK, GDK_z,         tricksy,   {.b=TRUE}                          },
    {  MODKEY,                GDK_z,         tricksy,   {.b=FALSE}                         },
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
    {  MODKEY,                GDK_BackSpace, navigate,  {.i=-1}                            },
    {  MODKEY|GDK_SHIFT_MASK, GDK_BackSpace, navigate,  {.i=+1}                            },
    {  MODKEY,                GDK_h,         scroll_h,  {.i=-1}                            },
    {  MODKEY,                GDK_l,         scroll_h,  {.i=+1}                            },
    {  MODKEY,                GDK_j,         scroll,    {.i=+1}                            },
    {  MODKEY,                GDK_k,         scroll,    {.i=-1}                            },
    {  0,                     GDK_Escape,    stop,      {0}                                },
    {  MODKEY,                GDK_o,         source,    {0}                                },
    {  MODKEY,                GDK_g,         spawn,     SETPROP("_SURF_URI","_SURF_GO")    },
    {  MODKEY,                GDK_m,         js_eval,   MARKS                              },
    {  MODKEY,                GDK_slash,     spawn,     SETPROP("_SURF_FIND","_SURF_FIND") },
    {  MODKEY,                GDK_n,         find,      {.b=TRUE}                          },
    {  MODKEY|GDK_SHIFT_MASK, GDK_n,         find,      {.b=FALSE}                         },
};
