/*
 * @author Lijing Wang <phdloli@ucla.edu>
 * 
 */

#include "playvideo.hpp"

namespace ndn {

  PlayVideo::PlayVideo()
  {
  }

  void
  PlayVideo::play_bin_uri (std::string filename)
  {
    GstElement *playbin;
    GMainLoop *loop;
    GstBus *bus;
    guint bus_watch_id;
    gchar *uri;

    gst_init (NULL, NULL);

    playbin = gst_element_factory_make ("playbin", NULL);
    if (!playbin) {
      g_print ("'playbin' gstreamer plugin missing\n");
    }

  /* take the commandline argument and ensure that it is a uri */
    if (gst_uri_is_valid (filename.c_str()))
      uri = g_strdup (filename.c_str());
    else
      uri = gst_filename_to_uri (filename.c_str(), NULL);
    g_object_set (playbin, "uri", uri, NULL);
    g_free (uri);

  /* create and event loop and feed gstreamer bus mesages to it */
    loop = g_main_loop_new (NULL, FALSE);

    bus = gst_element_get_bus (playbin);
    bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
    g_object_unref (bus);

  /* start play back and listed to events */
    gst_element_set_state (playbin, GST_STATE_PLAYING);
    g_main_loop_run (loop);

  /* cleanup */
    gst_element_set_state (playbin, GST_STATE_NULL);
    g_object_unref (playbin);
    g_source_remove (bus_watch_id);
    g_main_loop_unref (loop);

  }

} // namespace ndn
