#include <gtk/gtk.h>
#include <gst/gst.h>
#include <glib.h>
#include <gst/interfaces/xoverlay.h>
#include <gdk/gdkx.h>
#include <gdk/gdkkeysyms.h>

#include "player-xml.h"



void
seek_cb (GtkRange *range,
         GtkScrollType scroll,
         gdouble value,
         gpointer data)
{
    gint64 to_seek;

    if (!DURATION_IS_VALID (duration))
        duration = gst_query_duration ();

    if (!DURATION_IS_VALID (duration))
        return;

    to_seek = (value / 100) * duration;


   gst_seek_absolute (to_seek);
}


void 
on_window_destroy (GtkObject *object, gpointer user_data)
{
    gtk_main_quit ();
}

void
on_open_activate ( GtkMenuItem *item,gpointer data) {
	GtkWidget               *chooser;           
	
    chooser = gtk_file_chooser_dialog_new ("Open File...",
                                               GTK_WINDOW (window),
                                               GTK_FILE_CHOOSER_ACTION_OPEN,
                                               GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                               GTK_STOCK_OPEN, GTK_RESPONSE_OK,
                                               NULL);
                                               
        if (gtk_dialog_run (GTK_DIALOG (chooser)) == GTK_RESPONSE_OK)
        {
                filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
        }
        else
			return;
        g_print("%s",filename);
        gtk_widget_destroy (chooser);
        //filename = g_strdup_printf ("file://%s", filename);
        gst_play_main();
}

void
toggle_paused ()
{
    static gboolean paused = FALSE;
    if (paused)
    {
		gst_element_set_state(pipeline,GST_STATE_PAUSED);	
     //   gtk_button_set_label (GTK_BUTTON (pause_button), "Pause");
        paused = FALSE;
    }
    else
    {
        gst_element_set_state(pipeline,GST_STATE_PLAYING);
      //  gtk_button_set_label (GTK_BUTTON (pause_button), "Resume");
        paused = TRUE;
    }
}

void
change_volume(GtkScaleButton *button,gdouble value,gpointer data)
{
	FILE *cmd;
	gint exit_status;
	gdouble val = gtk_scale_button_get_value(button);
	gchar *command = "/usr/bin/amixer sset Master ";
	command = g_strdup_printf ("%s %d\%", command,(int)(val*100));
	cmd = popen(command,"r");
    exit_status = pclose (cmd);
}	
void 
setCurrentVol(GtkWidget *widget,gpointer data) {
	FILE *cmd;
	char path[4];
	
	gchar *command = "amixer get Master | egrep -o -e \"[0-9][0-9][0-9]%|[0-9][0-9]%|[0-9]%\" | cut -d '%' -f1";
	cmd = popen(command,"r");
	fgets(path, 10, cmd);
    int val = atoi(path);
    g_print("%d",val);
    gtk_scale_button_set_value(GTK_SCALE_BUTTON(volumeButton),val);
	cmd = popen(command,"r");
	fgets(path, 10, cmd);
    val = atoi(path);
    g_print("%d",val);

}

gboolean
key_press (GtkWidget *widget,
           GdkEventKey *event,
           gpointer data)
{
	
	gchar *name = gdk_keyval_name(event->keyval);
	g_print("%s",name);
	
    switch (event->keyval)
    {
        case GDK_P:
        case GDK_p:
        case GDK_space:
            toggle_paused ();
            break;
        case GDK_F:
        case GDK_f:
      //      toggle_fullscreen ();
            break;
        case GDK_R:
        case GDK_r:
            gst_seek(0);
            break;
        case GDK_Right:
            gst_seek (10);
            break;
        case GDK_Left:
            gst_seek (-10);
            break;
        case GDK_Q:
        case GDK_q:
            gtk_main_quit ();
            break;
        default:
            break;
    }

    return TRUE;
}
gboolean
changeScroll (gpointer data)
{
    gint64 pos;
	if(pipeline) {
    pos = gst_query_position ();
    if (!DURATION_IS_VALID (duration))
        duration = gst_query_duration ();

    if (!DURATION_IS_VALID (duration))
        return TRUE;
	
    /** @todo use events for seeking instead of checking for bad positions. */
    if (pos != 0)
    {
        double value;
        value = (pos * (((double) 100) / duration));
        gtk_range_set_value (GTK_RANGE (hScale), value);
    }
	}
    return TRUE;
}

void
gst_stop (GtkWidget *widget,gpointer data)
{
	if(pipeline)
	{
		gst_element_set_state (pipeline, GST_STATE_NULL);
		gtk_range_set_value (GTK_RANGE (hScale), (gint64)0);
	}
}

void
gst_pause (GtkWidget *widget,gpointer data)
{

    if (paused1)
    {
		gst_element_set_state(pipeline,GST_STATE_PAUSED);	
     //   gtk_button_set_label (GTK_BUTTON (pause_button), "Pause");
        paused1 = FALSE;
    }
    else
    {
        gst_element_set_state(pipeline,GST_STATE_PLAYING);
      //  gtk_button_set_label (GTK_BUTTON (pause_button), "Resume");
        paused1 = TRUE;
    }
}

void
gst_play (GtkWidget *widget,gpointer data)
{
    //static gboolean paused1 = FALSE;
    if (!paused1)
    {
		gst_element_set_state(pipeline,GST_STATE_PLAYING);	
     //   gtk_button_set_label (GTK_BUTTON (pause_button), "Pause");
        paused1 = TRUE;
    }
}
