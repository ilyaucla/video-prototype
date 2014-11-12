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
#include <unistd.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>

namespace ndn {

  class VideoPlayer
  {
    public:

      VideoPlayer();
      void
      playbin_appsrc_init ();
      void
      playbin_appsrc_data (const uint8_t *buffer, size_t bufferSize);
      void 
      get_streaminfo(std::string streaminfo);

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
        std::string capstr;
      };
      typedef struct _App App;
      App s_app;

      #define CHUNK_SIZE  1024*1024
      
      static void
      cb_need_data (GstElement *appsrc,
                    guint       unused_size,
                    App    *app)
      {
        GstBuffer *buffer;
        GstFlowReturn ret;
        g_print ("HERE!!");
        while ((app -> dataQue).size() == 0)
        {
//          g_print ("CHOKED\t");
          //sleep(1);
        }
        
        DataNode tmpNode = (app -> dataQue).front();

        std::cout << "Read Data Chunk" << tmpNode.length <<std::endl;
        buffer = gst_buffer_new ();
        buffer = gst_buffer_new_wrapped (tmpNode.data , tmpNode.length);
        g_signal_emit_by_name (app->appsrc, "push-buffer", buffer, &ret);
      //  gst_buffer_unref (buffer);
        if (ret != GST_FLOW_OK) {
          /* some error, stop sending data */
          g_print ("ERROR!\t");
          return ;
        }
        (app -> dataQue).pop_front();
        g_print ("Read Over!\t");
      }
      
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

        std::cout << "Read Data Here appsrc work well! data length " << tmpNode.length <<std::endl;
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
        std::cout << app->capstr << std::endl;
        g_object_set (G_OBJECT (app->appsrc), "caps", gst_caps_from_string((app->capstr).c_str()), NULL);
      
//        g_object_set (app->appsrc, "size", (gint64) app->length, NULL);
      
        /* configure the appsrc, we will push data into the appsrc from the
         * mainloop. */
        g_signal_connect (app->appsrc, "need-data", G_CALLBACK (start_feed), app);
        g_signal_connect (app->appsrc, "enough-data", G_CALLBACK (stop_feed), app);
      }

      static void
      *h264_appsrc_thread (void * threadData)
      {
        App *app;
        app = (App *) threadData;

        GstBus *bus;
        GstElement *pipeline, *decoder, *videosink;
        GstCaps *caps;
        
//        gint width, height, num, denum;
//        const char * name, stream-format, alignment;

        /* init GStreamer */
        gst_init (NULL, NULL);
        app->loop = g_main_loop_new (NULL, FALSE);
        
        /* setup pipeline */
        pipeline = gst_pipeline_new ("pipeline");
        app->appsrc = gst_element_factory_make ("appsrc", "source");
        decoder = gst_element_factory_make ("avdec_h264", "decoder");
        videosink = gst_element_factory_make ("autovideosink", "videosink");
        
        bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
        gst_bus_add_watch (bus, (GstBusFunc)bus_call, app);
        gst_object_unref (bus);

        const gchar * streaminfo = app->capstr.c_str();
        std::cout << "HAHA " << streaminfo << std::endl;
        /* setup streaminfo */

        caps = gst_caps_from_string(streaminfo); 
        std::cout << "WHAT" <<gst_caps_to_string (caps) << std::endl;
        caps = gst_caps_make_writable(caps);
        GstStructure *str = gst_caps_get_structure (caps, 0);
//        GstStructure *str =  gst_structure_from_string((app->capstr).c_str());
        gst_structure_remove_fields (str,"level", "profile", "height", "width", "framerate", "pixel-aspect-ratio", "parsed", NULL);
////        gst_caps_append_structure(caps, str);
//        std::cout << gst_caps_to_string (caps) << std::endl;
//        str = gst_caps_get_structure (capstmp, 0);
//        if (!gst_structure_get_int (str, "width", &width) ||
//            !gst_structure_get_int (str, "height", &height)) { g_print ("No width/height available\n");
//          return;
//        }
//
//        caps = gst_caps_new_simple ("video/x-h264", "stream-format", G_TYPE_STRING, "avc",
//            "alignment", G_TYPE_STRING, "au", "codec_data", GST_TYPE_BUFFER, "0164001effe1001a6764001eacc8602a0c7e4c0440000003004000000ca3c58b678001000568e9bb2c8b", NULL);

//        caps = gst_caps_new_simple ("video/x-h264", "stream-format", G_TYPE_STRING, "avc",
//            "alignment", G_TYPE_STRING, "au", NULL);
        std::cout << "HERE!" << gst_caps_to_string (caps) << std::endl;
//        gst_app_src_set_caps(GST_APP_SRC(app->appsrc), caps);
        g_object_set (G_OBJECT (app->appsrc), "caps", caps, NULL);

        /*
        gboolean link_ok;
        link_ok = gst_element_link_filtered (app->appsrc, decoder, caps);

        if (!link_ok) {
          g_warning ("Failed to link appsrc and decoder");
        }
        */ 
        gst_bin_add_many (GST_BIN (pipeline), app->appsrc, decoder, videosink, NULL);
       // gst_element_link_many (app->appsrc, decoder, videosink, NULL);
        if(!gst_element_link(app->appsrc, decoder)){
          g_print("failed to link appsrc and decoder");
          pthread_exit(NULL);
        }
        if(!gst_element_link(decoder, videosink)){
          g_print("failed to link decoder and videosink");
          pthread_exit(NULL);
        }
        
        /* setup appsrc */
//        g_object_set (G_OBJECT (app->appsrc),
//                      "stream-type", 0,
//                      "format", GST_FORMAT_TIME, NULL);
        g_signal_connect (app->appsrc, "need-data", G_CALLBACK (start_feed), app);
        g_signal_connect (app->appsrc, "enough-data", G_CALLBACK (stop_feed), app);
        
        /* play */
        gst_element_set_state (pipeline, GST_STATE_PLAYING);
        g_main_loop_run (app->loop);
        
        /* clean up */
        gst_element_set_state (pipeline, GST_STATE_NULL);
        gst_object_unref (GST_OBJECT (pipeline));
        g_main_loop_unref (app->loop);

        pthread_exit(NULL);
      }

      static void
      *videosink_appsrc_thread (void * threadData)
      {
        App *app;
        app = (App *) threadData;

        GstElement *pipeline, *conv, *videosink;
        
        /* init GStreamer */
        gst_init (NULL, NULL);
        app->loop = g_main_loop_new (NULL, FALSE);
        
        /* setup pipeline */
        pipeline = gst_pipeline_new ("pipeline");
        app->appsrc = gst_element_factory_make ("appsrc", "source");
        conv = gst_element_factory_make ("videoconvert", "conv");
        videosink = gst_element_factory_make ("autovideosink", "videosink");
        
        /* setup */
        g_object_set (G_OBJECT (app->appsrc), "caps", gst_caps_from_string((app->capstr).c_str()), NULL);
          /* setup 
        g_object_set (G_OBJECT (app->appsrc), "caps",
          gst_caps_new_simple ("video/x-raw",
          "format", G_TYPE_STRING, "RGB16",
          "width", G_TYPE_INT, 384,
          "height", G_TYPE_INT, 288,
          "framerate", GST_TYPE_FRACTION, 0, 1,
          NULL), NULL);
          */
        gst_bin_add_many (GST_BIN (pipeline), app->appsrc, conv, videosink, NULL);
        gst_element_link_many (app->appsrc, conv, videosink, NULL);
        
        /* setup appsrc */
        g_object_set (G_OBJECT (app->appsrc),
                      "stream-type", 0,
                      "format", GST_FORMAT_TIME, NULL);
//        g_signal_connect (app->appsrc, "need-data", G_CALLBACK (cb_need_data), app);
        g_signal_connect (app->appsrc, "need-data", G_CALLBACK (start_feed), app);
        g_signal_connect (app->appsrc, "enough-data", G_CALLBACK (stop_feed), app);
        
        /* play */
        gst_element_set_state (pipeline, GST_STATE_PLAYING);
        g_main_loop_run (app->loop);
        
        /* clean up */
        gst_element_set_state (pipeline, GST_STATE_NULL);
        gst_object_unref (GST_OBJECT (pipeline));
        g_main_loop_unref (app->loop);

        pthread_exit(NULL);
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
        g_signal_connect (app->playbin, "deep-notify::source", (GCallback) found_source, app);
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
