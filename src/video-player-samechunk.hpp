/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Regents of the University of California.
 *
 * @author Lijing Wang <phdloli@ucla.edu>
 */

#ifndef VIDEO_PLAYER_HPP
#define VIDEO_PLAYER_HPP

#include <fstream>
#include <gst/gst.h>
#include <iostream>
#include <deque>
#include <pthread.h>

namespace ndn {

  class VideoPlayer
  {
    public:

      VideoPlayer();
      void
      playbin_uri (std::string filename);
      void
      playbin_appsrc_init ();
      void
      playbin_appsrc_data (const uint8_t *buffer, size_t bufferSize);

    private:
      struct _DataNode
      {
        gsize length;
        guint8 *data;
      };
      typedef struct _DataNode DataNode;

      struct _App
      {
        GstElement *playbin;
        GstElement *appsrc;
      
        GMainLoop *loop;
        guint sourceid;

        std::deque<DataNode> dataQue;
        
      };
      typedef struct _App App;
      App s_app;

      #define CHUNK_SIZE  1024*1024

      static gboolean
      read_data (App * app)
      {
        GstBuffer *buffer;
        GstFlowReturn ret;
        
        if((app -> dataQue).size() == 0)
        {
          return TRUE; 
        }
        
        DataNode tmpNode = (app -> dataQue).front();

        std::cout << "Read Data Chunk" << tmpNode.length <<std::endl;
        buffer = gst_buffer_new ();
        buffer = gst_buffer_new_wrapped (tmpNode.data , tmpNode.length);
        g_signal_emit_by_name (app->appsrc, "push-buffer", buffer, &ret);
      //  gst_buffer_unref (buffer);
        if (ret != GST_FLOW_OK) {
          /* some error, stop sending data */
          return FALSE;
        }
        (app -> dataQue).pop_front();
        return TRUE;
      }
      
      /* This signal callback is called when appsrc needs data, we add an idle handler
       * to the mainloop to start pushing data into the appsrc */
      static void
      start_feed (GstElement * playbin, guint size, App * app)
      {
        if (app->sourceid == 0) {
          app->sourceid = g_idle_add ((GSourceFunc) read_data, app);
        }
      }
      
      /* This callback is called when appsrc has enough data and we can stop sending.
       * We remove the idle handler from the mainloop */
      static void
      stop_feed (GstElement * playbin, App * app)
      {
        if (app->sourceid != 0) {
          g_source_remove (app->sourceid);
          app->sourceid = 0;
        }
      }
      
      /* this callback is called when playbin has constructed a source object to read
       * from. Since we provided the appsrc:// uri to playbin, this will be the
       * appsrc that we must handle. We set up some signals to start and stop pushing
       * data into appsrc */
      static void
      found_source (GObject * object, GObject * orig, GParamSpec * pspec, App * app)
      {
        /* get a handle to the appsrc */
        g_object_get (orig, pspec->name, &app->appsrc, NULL);
      
//        g_object_set (app->appsrc, "size", (gint64) app->length, NULL);
      
        /* configure the appsrc, we will push data into the appsrc from the
         * mainloop. */
        g_signal_connect (app->appsrc, "need-data", G_CALLBACK (start_feed), app);
        g_signal_connect (app->appsrc, "enough-data", G_CALLBACK (stop_feed), app);
      }

      static void
      *playbin_appsrc_thread (void * threadData)
      {
        App *app;
        app = (App *) threadData;

        GstBus *bus;
        gst_init (NULL, NULL);
        /* create a mainloop to get messages */
        app->loop = g_main_loop_new (NULL, FALSE);
        app->playbin = gst_element_factory_make ("playbin", NULL);
        g_assert (app->playbin);
        bus = gst_pipeline_get_bus (GST_PIPELINE (app->playbin));
        /* add watch for messages */
        gst_bus_add_watch (bus, (GstBusFunc)bus_call, app);
        g_signal_connect (bus, "message", G_CALLBACK (bus_call), app);
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
        gst_element_set_state (app->playbin, GST_STATE_NULL);
        /* free the file */
        gst_object_unref (bus);
        g_main_loop_unref (app->loop);

        pthread_exit(NULL);
      }

      static gboolean
      bus_call (GstBus * bus, GstMessage *msg, App *app)
      {
        switch (GST_MESSAGE_TYPE (msg)) {
          case GST_MESSAGE_BUFFERING: {
            gint percent = 0;
            gst_message_parse_buffering (msg, &percent);
            g_print ("Buffering (%3d%%)\r", percent);
            /* Wait until buffering is complete before start/resume playing */
            if (percent < 100)
              gst_element_set_state (app->playbin, GST_STATE_PAUSED);
            else
              gst_element_set_state (app->playbin, GST_STATE_PLAYING);
            break;
          }
          case GST_MESSAGE_EOS:{
            g_print ("End-of-stream\n");
            g_main_loop_quit (app->loop);
            break;
          }
          case GST_MESSAGE_ERROR:{
            gchar *debug;
            GError *err;
            gst_message_parse_error (msg, &err, &debug);
            g_printerr ("Debugging info: %s\n", (debug) ? debug : "none");
            g_free (debug);
            g_print ("Error: %s\n", err->message);
            g_error_free (err);
            g_main_loop_quit (app->loop);
            break;
          }
          case GST_MESSAGE_CLOCK_LOST:{
          /* Get a new clock */
            gst_element_set_state (app->playbin, GST_STATE_PAUSED);
            gst_element_set_state (app->playbin, GST_STATE_PLAYING);
            break;
          }
          default:
            break;
        }
      return TRUE;
      }
  };
} // namespace ndn
#endif //VIDEO_PLAYER_HPP
