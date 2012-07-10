#include <gtk/gtk.h>
#include <gst/gst.h>
#include <glib.h>
#include <gst/interfaces/xoverlay.h>
#include <gdk/gdkx.h>
#include <gdk/gdkkeysyms.h>

#include "player-xml.h"

int
main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    gst_init(&argc, &argv);
	GMutex* lock;
	GList *list;

    builder = gtk_builder_new();
    if (!gtk_builder_add_from_file(builder, "UI.glade", NULL)) {
        g_print("Error opening file");
        return 0;
    }
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    vBox = GTK_WIDGET(gtk_builder_get_object(builder, "vbox"));
    vBoxBottom = GTK_WIDGET(gtk_builder_get_object(builder, "vboxBottom"));
    hBoxToolbar = GTK_WIDGET(gtk_builder_get_object(builder, "hboxToolbar"));
    hBoxMain = GTK_WIDGET(gtk_builder_get_object(builder, "hboxMain"));
    // hBox = GTK_WIDGET (gtk_builder_get_object (builder, "hbox"));
    menu = GTK_MENU_BAR(gtk_builder_get_object(builder, "menubar"));
    FileItem = GTK_MENU_ITEM(gtk_builder_get_object(builder, "menuitemFile"));
    MenuFile = GTK_MENU(gtk_builder_get_object(builder, "menuFile"));
    video_output = GTK_WIDGET(gtk_builder_get_object(builder, "drawingarea"));
    hScale = GTK_HSCALE(gtk_builder_get_object(builder, "hscale"));
    hScaleAdjustment = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "adjustment"));
    // button = GTK_WIDGET (gtk_builder_get_object (builder, "button" ));
    openMenu = GTK_IMAGE_MENU_ITEM(gtk_builder_get_object(builder, "imagemenuitemOpen"));
    quitMenu = GTK_IMAGE_MENU_ITEM(gtk_builder_get_object(builder, "imagemenuitemQuit"));
    toolBar = GTK_TOOLBAR(gtk_builder_get_object(builder, "toolbar"));
    buttonPlay = GTK_TOOL_BUTTON(gtk_builder_get_object(builder, "toolbuttonPlay"));
    buttonPause = GTK_TOOL_BUTTON(gtk_builder_get_object(builder, "toolbuttonPause"));
    buttonStop = GTK_TOOL_BUTTON(gtk_builder_get_object(builder, "toolbuttonStop"));
    volumeButton = GTK_VOLUME_BUTTON(gtk_builder_get_object(builder, "volumebutton"));

    gtk_builder_connect_signals(builder, NULL);

    g_object_unref(G_OBJECT(builder));

    gtk_widget_show_all(window);
    g_timeout_add(1000, changeScroll, NULL);
    gtk_main();

    return 0;
}

