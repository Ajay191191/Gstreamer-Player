#include <gtk/gtk.h>
#include <gst/gst.h>
#include <glib.h>
#include <gst/video/video.h>
#include <gst/video/videooverlay.h>
#include <gdk/gdkx.h>
#include <gdk/gdkkeysyms.h>

#include "player-xml.h"




void
seek_cb (GtkRange *range,
         GtkScrollType scroll,
         gdouble value,
         gpointer data)
{
    /*gint64 to_seek;

    if (!DURATION_IS_VALID (duration))
        duration = gst_query_duration ();

    if (!DURATION_IS_VALID (duration))
        return;

    to_seek = (value / 100) * duration;


   gst_seek_absolute (to_seek);*/
   value = gtk_range_get_value (GTK_RANGE (hScale));

   gst_element_seek_simple (pipeline, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT,(gint64)(value * GST_SECOND));
}


void 
on_window_destroy (GtkObject *object, gpointer user_data)
{
    gtk_main_quit ();
}

void
on_open_activate ( GtkMenuItem *item,gpointer data) {
	GtkWidget               *chooser;           
	
	printf("asd");
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

gboolean slider_button_press_cb(GtkWidget * widget, GdkEventButton * event, gpointer user_data)
{
	if (event->button == 1)
        event->button = 2;
	
	return FALSE;
}

gboolean slider_button_release_cb(GtkWidget * widget, GdkEventButton * event, gpointer user_data)
{
    if (event->button == 1)
        event->button = 2;
    gst_element_set_state(pipeline, GST_STATE_PAUSED);
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    return FALSE;
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
	gdouble val = gtk_scale_button_get_value(button);
	g_object_set(volume,"volume",(val)/10,NULL);
}	

gboolean
key_press (GtkWidget *widget,
           GdkEventKey *event,
           gpointer data)
{
    gchar *name = gdk_keyval_name(event->keyval);	
    switch (event->keyval)
    {
        case GDK_P:
        case GDK_p:
        case GDK_space:
            toggle_paused ();
            break;
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
	case GDK_f:
	case GDK_Alt_L + GDK_Return:
    		if ((gdk_window_get_state(GDK_WINDOW(widget->window)) == GDK_WINDOW_STATE_FULLSCREEN)) 
		{
			gtk_window_unfullscreen(GTK_WINDOW(widget));
			gtk_widget_set_visible(vBoxBottom,TRUE);
		}
         	else
		{
        		gtk_window_fullscreen(GTK_WINDOW(widget));
			gtk_widget_set_visible(vBoxBottom,FALSE);
		}
	break;
        default:
            break;
    }

    return TRUE;
}

/*void
refresh_ui() {

    gint64 duration;
    GstState state;
    GstFormat fmt = GST_FORMAT_TIME;
    if(pipeline) {
        gst_element_get_state(pipeline, &state, NULL, GST_CLOCK_TIME_NONE);
        if(state<GST_STATE_PAUSED){
            gst_element_query_duration (pipeline, &fmt, &duration);
            g_print("Duration = %d",duration);
	    if(duration>0)
            gtk_range_set_range (GTK_RANGE (hScale), 0, (gdouble)duration / GST_SECOND);
        }
    }
}*/

gboolean refresh_ui (gpointer data) {
  GstFormat fmt = GST_FORMAT_TIME;
  gint64 current = -1;
  
  GstState state;
  gst_element_get_state(pipeline, &state, NULL, GST_CLOCK_TIME_NONE);
  /* We do not want to update anything unless we are in the PAUSED or PLAYING states */
  if (state < GST_STATE_PAUSED)
    return TRUE;
  /* If we didn't know it yet, query the stream duration */
  if (!GST_CLOCK_TIME_IS_VALID (duration)) {
    if (!gst_element_query_duration (pipeline, fmt, &duration)) {
      g_printerr ("Could not query current duration.\n");
    } else {
      /* Set the range of the slider to the clip duration, in SECONDS */
      gtk_range_set_range (GTK_RANGE (hScale), 0, (gdouble)duration / GST_SECOND);
    }
  }
  
  if (gst_element_query_position (pipeline, fmt, &current)) {
    /* Block the "value-changed" signal, so the slider_cb function is not called
     * (which would trigger a seek the user has not requested) */
    g_signal_handler_block (hScale, seek_cb_signal);
    /* Set the position of the slider to the current pipeline positoin, in SECONDS */
    gtk_range_set_value (GTK_RANGE (hScale), (gdouble)current / GST_SECOND);
    /* Re-enable the signal */
    g_signal_handler_unblock (hScale, seek_cb_signal);
  }
  return TRUE;
}

/*gboolean
changeScroll (gpointer data)
{
    gint64 pos;
    if(pipeline) {
    pos = gst_query_position ();
    if (!DURATION_IS_VALID (duration))
        duration = gst_query_duration ();

    if (!DURATION_IS_VALID (duration))
        return TRUE;
	
   // ** @todo use events for seeking instead of checking for bad positions. *
    if (pos != 0)
    {
        double value;
        value = (pos * (((double) 100) / duration));
        gtk_range_set_value (GTK_RANGE (hScale), value);
    }
	}
    return TRUE;
}
*/
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
