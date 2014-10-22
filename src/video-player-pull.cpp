/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Regents of the University of California.
 *
 * @author Lijing Wang <phdloli@ucla.edu>
 */

#include "video-player.hpp"

namespace ndn {

  VideoPlayer::VideoPlayer()
  {
  }

  GST_DEBUG_CATEGORY (appsrc_playbin_debug);

  #define GST_CAT_DEFAULT appsrc_playbin_debug

/*
 * Store the whole video file into local path
 * then read from filename and playback
 */
  void
  VideoPlayer::playbin_uri (std::string filename)
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
  
  void
  VideoPlayer::playbin_appsrc_init (const uint8_t* buffer, size_t bufferSize)
  {
    App *app = &s_app;
    GstBus *bus;

    gst_init (NULL, NULL);

    GST_DEBUG_CATEGORY_INIT (appsrc_playbin_debug, "appsrc-playbin", 0,
        "appsrc playbin example");

    /* get some vitals, this will be used to read data from the mmapped file and
     * feed it to appsrc. */
    app->length = bufferSize;
    app->data = (guint8 *) buffer;
    app->offset = 0;

    /* create a mainloop to get messages */
    app->loop = g_main_loop_new (NULL, TRUE);

    app->playbin = gst_element_factory_make ("playbin", NULL);
    g_assert (app->playbin);

    bus = gst_pipeline_get_bus (GST_PIPELINE (app->playbin));

    /* add watch for messages */
   // gst_bus_add_watch (bus, (GstBusFunc) bus_message, app);
    gst_bus_add_watch (bus, bus_call, app->loop);

    /* set to read from appsrc */
    g_object_set (app->playbin, "uri", "appsrc://", NULL);

    /* get notification when the source is created so that we get a handle to it
     * and can configure it */
    g_signal_connect (app->playbin, "deep-notify::source",
        (GCallback) found_source, app);

    /* go to playing and wait in a mainloop. */
    gst_element_set_state (app->playbin, GST_STATE_PLAYING);

    /* this mainloop is stopped when we receive an error or EOS */
    g_main_loop_run (app->loop);

    GST_DEBUG ("stopping!");

    gst_element_set_state (app->playbin, GST_STATE_NULL);

    /* free the file */

    gst_object_unref (bus);
    g_main_loop_unref (app->loop);

  }

} // namespace ndn
