
#ifndef PLAYER_XML_H
#define PLAYER_XML_H


GtkBuilder *builder; 
GtkWidget *window;
GtkWidget *button;
GtkWidget *button1;
GtkWidget *vBox;
GtkWidget *hBox;
GtkWidget *vBoxBottom;
GtkHScale *hScale;
GtkAdjustment *hScaleAdjustment;
GtkWidget *hBoxToolbar;
GtkToolbar *toolBar;
GtkToolButton *buttonPlay;
GtkToolButton *buttonPause;
GtkToolButton *buttonStop;
GtkVolumeButton *volumeButton;
GtkWidget *hBoxMain;

GtkMenuBar *menu;
GtkMenuItem *FileItem;
GtkMenu *MenuFile;
GtkImageMenuItem *openMenu;
GtkImageMenuItem *quitMenu;
static gboolean paused1 = FALSE;
GtkWidget *video_output;

char *filename;

//GstElement *pipeline,*videosink,*bin;
GstElement *src, *dec, *conv, *sink, *typefind,*typefind2,*audioqueue,*videoqueue,*volume,*level;
GstElement *convVid,*videosink,*videoRate,*capsFilter1,*videoScale,*capsFilter2;
GstBus *bus;
GstCaps *caps,*caps2;
guint64 duration;
GstElement *pipeline, *audio,*video;
static int mutex;



#define DURATION_IS_VALID(x) (x != 0 && x != (guint64) -1)
void seek_cb (GtkRange *range, GtkScrollType scroll,  gdouble value,  gpointer data);
void on_window_destroy (GtkObject *object, gpointer user_data);
void on_open_activate ( GtkMenuItem *item,gpointer data);
void toggle_paused ();
void change_volume(GtkScaleButton *button,gdouble value,gpointer data);	
gboolean key_press (GtkWidget *widget, GdkEventKey *event, gpointer data);
gboolean changeScroll (gpointer data);
void gst_stop (GtkWidget *widget,gpointer data);

void gst_pause (GtkWidget *widget,gpointer data);

void gst_play (GtkWidget *widget,gpointer data);

gboolean bus_call (GstBus     *bus,GstMessage *msg, gpointer    data);
void gst_play_main();
void gst_seek_absolute (guint64 value);
void gst_seek (gint value);
guint64 gst_query_duration (void);
guint64 gst_query_position (void);
static void cb_newpad (GstElement *decodebin,GstPad     *pad,gboolean    last, gpointer    data);
gboolean refresh_ui(gpointer data);
gulong seek_cb_signal;
gboolean slider_button_press_cb(GtkWidget * widget, GdkEventButton * event, gpointer user_data);
gboolean slider_button_release_cb(GtkWidget * widget, GdkEventButton * event, gpointer user_data);


#endif /* PLAYER_XML_H */
