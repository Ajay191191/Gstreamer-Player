#include <gtk/gtk.h>
#include <gst/gst.h>
#include <glib.h>
#include <gst/video/video.h>
#include <gst/video/videooverlay.h>
#include <gdk/gdkx.h>
#include <gdk/gdkkeysyms.h>

#include "player-xml.h"

static void state_changed_cb (GstBus *bus, GstMessage *msg) {
  GstState old_state, new_state, pending_state;
  gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
  if (GST_MESSAGE_SRC (msg) == GST_OBJECT (dec)) {
   // data->state = new_state;
    g_print ("State set to %s\n", gst_element_state_get_name (new_state));
    if (old_state == GST_STATE_READY && new_state == GST_STATE_PAUSED) {
      /* For extra responsiveness, we refresh the GUI as soon as we reach the PAUSED state */
      refresh_ui (NULL);
	
    }
  }
}


gboolean bus_call(GstBus *bus,GstMessage *msg,gpointer data) {
	
    GMainLoop *loop = (GMainLoop *) data;

    switch (GST_MESSAGE_TYPE(msg)) {

        case GST_MESSAGE_EOS:
            g_print("End of stream\n");
            g_main_loop_quit(loop);
            break;

        case GST_MESSAGE_ERROR:
        {
            gchar *debug;
            GError *error;

            gst_message_parse_error(msg, &error, &debug);
            g_free(debug);

            g_printerr("Error: %s\n", error->message);
            g_error_free(error);

            g_main_loop_quit(loop);
            break;
        }
        default:
            break;
    }

    return TRUE;
}

static void cb_newpad(GstElement *decodebin, GstPad *pad, gboolean last, gpointer data) {
  GstPad *sinkpad;
  GstCaps *caps;
  GstStructure *str;
  gchar *tex;
  caps = gst_pad_get_current_caps(pad);
  str = gst_caps_get_structure(caps, 0);
  tex = gst_structure_get_name(str);
  mutex++;
  g_print("TEX:%s \nSTR:%s",tex,gst_structure_get_name (str));
  if (g_strrstr(gst_structure_get_name(str), "audio"))
  {
    sinkpad = gst_element_get_static_pad(audio, "sink");
    gst_pad_link(pad, sinkpad);
    gst_object_unref (sinkpad);
    gint curr_vol; 
  }
  else if (g_strrstr(gst_structure_get_name(str), "video"))
  {
    sinkpad = gst_element_get_static_pad(video, "sink");
    gst_pad_link(pad, sinkpad);
    gst_object_unref (sinkpad);
  }
}

static void eos_cb (GstBus *bus, GstMessage *msg, gpointer *data) {
  g_print ("End-Of-Stream reached.\n");
  gst_element_set_state (pipeline, GST_STATE_PAUSED);
}

void gst_play_main() {
    GstPad *audiopad, *videopad;
    //Stop before playing
    if (pipeline) {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(GST_OBJECT(pipeline));
        pipeline = NULL;
    }
    {
        mutex = 0;

        /* setup */
        pipeline = gst_pipeline_new("pipeline");

        bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
        gst_bus_add_signal_watch(bus);
        g_signal_connect (G_OBJECT (bus), "message::state-changed", (GCallback)state_changed_cb, NULL);
        g_signal_connect (G_OBJECT (bus), "message::eos", (GCallback)eos_cb, NULL);
        gst_object_unref(bus);

        src = gst_element_factory_make("filesrc", "source");
        g_object_set(G_OBJECT(src), "location", filename, NULL);
        typefind = gst_element_factory_make("typefind", "typefinder");
        //g_signal_connect(typefind,"have-type",G_CALLBACK(typefound_cb),loop);
        dec = gst_element_factory_make("decodebin", "decoder");
        g_signal_connect(dec, "pad-added", G_CALLBACK(cb_newpad), NULL);
        //myplugin = gst_element_factory_make("myplugin","MyPlugin");
        audioqueue = gst_element_factory_make("queue", "audioqueue");
        videoqueue = gst_element_factory_make("queue", "videoqueue");
        gst_bin_add_many(GST_BIN(pipeline), src, typefind, dec, NULL);
        gst_element_link_many(src, typefind, dec, NULL);

        /* create audio output */
        audio = gst_bin_new("audiobin");
        conv = gst_element_factory_make("audioconvert", "aconv");
        typefind2 = gst_element_factory_make("typefind", "typefinder2");
		volume = gst_element_factory_make("volume","volume");
		level = gst_element_factory_make("level","level");
        //g_signal_connect(typefind2,"have-type",G_CALLBACK(typefound_cb),loop);
        audiopad = gst_element_get_static_pad(audioqueue, "sink");
        sink = gst_element_factory_make("alsasink", "sink");
        gst_bin_add_many(GST_BIN(audio), audioqueue, conv, typefind2,volume,level, sink, NULL);
        gst_element_link_many(audioqueue, conv, typefind2,volume, level,sink, NULL);
        gst_element_add_pad(audio,
        gst_ghost_pad_new("sink", audiopad));
        gst_object_unref(audiopad);
        gst_bin_add(GST_BIN(pipeline), audio);

        /* create video output */
        video = gst_bin_new("videobin");
        caps2 =gst_caps_from_string("video/x-raw,framerate=25/1");
        caps =gst_caps_from_string("video/x-raw,width=1024,height=768");
        
        videoRate = gst_element_factory_make("videorate", "Video Rate");
        capsFilter1 = gst_element_factory_make("capsfilter", "Caps Filter");
        g_object_set(G_OBJECT(capsFilter1), "caps", caps2, NULL);
        videoScale = gst_element_factory_make("videoscale", "Video Scale");
        //g_object_set(G_OBJECT(videoScale),"add-borders","true",NULL);
        capsFilter2 = gst_element_factory_make("capsfilter", "Caps Filter2");
        g_object_set(G_OBJECT(capsFilter2), "caps", caps, NULL);
        
        //convVid = gst_element_factory_make("ffmpegcolorspace", "converter");
        convVid = gst_element_factory_make("videoscale", "converter");
        videopad = gst_element_get_static_pad(videoqueue, "sink");
        videosink = gst_element_factory_make("xvimagesink", "videosink");
        g_object_set(G_OBJECT(videosink),"pixel-aspect-ratio","3/4",NULL);
		g_object_set(G_OBJECT(videosink),"force-aspect-ratio",TRUE,NULL);
        gst_bin_add_many(GST_BIN(video), videoqueue,videoScale,capsFilter2 ,videoRate, capsFilter1, convVid, videosink, NULL);
        gst_element_link_many(videoqueue, videoScale,capsFilter2 ,videoRate, capsFilter1, convVid, videosink, NULL);
        gst_element_add_pad(video, gst_ghost_pad_new("sink", videopad));
        gst_object_unref(videopad);
        gst_bin_add(GST_BIN(pipeline), video);

	

        /* run */
        //gst_element_set_state (pipeline, GST_STATE_PLAYING);

        g_print("Now playing: %s\n", filename);
        if (GST_IS_VIDEO_OVERLAY (videosink))
        {
			gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY (videosink), GPOINTER_TO_UINT(GINT_TO_POINTER(GDK_WINDOW_XWINDOW(video_output->window))));
            //gst_x_overlay_set_window_handle(GST_X_OVERLAY(videosink), GPOINTER_TO_UINT(GINT_TO_POINTER(GDK_WINDOW_XWINDOW(video_output->window))));
           /* if(gst_x_overlay_set_render_rectangle(GST_X_OVERLAY(videosink),0,0,800,600))
            {
                gst_x_overlay_expose(GST_X_OVERLAY(videosink));    
                g_print("Redraw");
            }*/
        }
        gst_element_set_state(pipeline, GST_STATE_PLAYING);
	gdouble curr_vol; 
    	g_object_get(volume,"volume",&curr_vol,NULL);
    	gtk_scale_button_set_value(GTK_SCALE_BUTTON(volumeButton),(curr_vol)*10);
	g_timeout_add(1, refresh_ui, NULL);
    }
}

void
gst_seek_absolute(guint64 value) {
    gst_element_seek(pipeline, 1.0,
            GST_FORMAT_TIME,
            GST_SEEK_FLAG_ACCURATE | GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT,
            GST_SEEK_TYPE_SET, value,
            GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
}

void
gst_seek(gint value) {
    gst_element_seek(pipeline, 1.0,
            GST_FORMAT_TIME,
            GST_SEEK_FLAG_ACCURATE | GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT,
            GST_SEEK_TYPE_END, value * GST_SECOND,
            GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
}

guint64
gst_query_duration(void) {
    GstFormat format = GST_FORMAT_TIME;
    gint64 cur;

    gst_element_query_duration(pipeline, format, &cur);
    if (format != GST_FORMAT_TIME)
        return GST_CLOCK_TIME_NONE;

    return cur;
}

guint64
gst_query_position(void) {
    GstFormat format = GST_FORMAT_TIME;
    gint64 cur;

    gst_element_query_position(pipeline, format, &cur);
    if (format != GST_FORMAT_TIME)
        return GST_CLOCK_TIME_NONE;

    return cur;
}
