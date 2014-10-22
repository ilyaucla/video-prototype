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
      playbin_appsrc_data (const uint8_t* buffer, size_t bufferSize);

      static gboolean
      bus_call (GstBus * bus, GstMessage *msg, gpointer data)
      {
        GMainLoop *loop = (GMainLoop *) data;
    
        switch (GST_MESSAGE_TYPE (msg)) {
          case GST_MESSAGE_EOS:{
            g_print ("End-of-stream\n");
            g_main_loop_quit (loop);
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
            g_main_loop_quit (loop);
            break;
          }
          default:
            break;
        }
      return TRUE;
      }

    private:
      struct _App
      {
        GstElement *playbin;
        GstElement *appsrc;
      
        GMainLoop *loop;
        guint sourceid;
      
        guint8 *data;
        gsize length;
        guint64 offset;
        
        guint8 *nextData;
        gsize nextLength;
      };
      
      typedef struct _App App;
      App s_app;
      #define CHUNK_SIZE  4096
      
      static gboolean
      read_data (App * app)
      {
        GstBuffer *buffer;
        guint len;
        GstFlowReturn ret;
      
        buffer = gst_buffer_new ();
        if (app->data == NULL) 
        {
          return FALSE;
        } 
        if (app->offset >= app->length) {
          /* we are EOS, send end-of-stream and remove the source */
          g_signal_emit_by_name (app->appsrc, "end-of-stream", &ret);
          return FALSE;
        }
      
        /* read the next chunk */
        len = CHUNK_SIZE;
        if (app->offset + len > app->length)
          len = app->length - app->offset;
        buffer = gst_buffer_new_wrapped (app->data + app->offset, len);
        GST_DEBUG ("feed buffer: offset %" G_GUINT64_FORMAT "-%u", 
            app->offset, len);
        g_signal_emit_by_name (app->appsrc, "push-buffer", buffer, &ret);
      //  gst_buffer_unref (buffer);
        if (ret != GST_FLOW_OK) {
          /* some error, stop sending data */
          return FALSE;
        }
      
        app->offset += len;
      
        return TRUE;
      }
      
      /* This signal callback is called when appsrc needs data, we add an idle handler
       * to the mainloop to start pushing data into the appsrc */
      static void
      start_feed (GstElement * playbin, guint size, App * app)
      {
        if (app->sourceid == 0) {
          GST_DEBUG ("start feeding");
          app->sourceid = g_idle_add ((GSourceFunc) read_data, app);
        }
      }
      
      /* This callback is called when appsrc has enough data and we can stop sending.
       * We remove the idle handler from the mainloop */
      static void
      stop_feed (GstElement * playbin, App * app)
      {
        if (app->sourceid != 0) {
          GST_DEBUG ("stop feeding");
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
      
        GST_DEBUG ("got appsrc %p", app->appsrc);
      
        /* we can set the length in appsrc. This allows some elements to estimate the
         * total duration of the stream. It's a good idea to set the property when you
         * can but it's not required. */
        g_object_set (app->appsrc, "size", (gint64) app->length, NULL);
      
        /* configure the appsrc, we will push data into the appsrc from the
         * mainloop. */
        g_signal_connect (app->appsrc, "need-data", G_CALLBACK (start_feed), app);
        g_signal_connect (app->appsrc, "enough-data", G_CALLBACK (stop_feed), app);
      }
  };
} // namespace ndn
#endif //VIDEO_PLAYER_HPP
