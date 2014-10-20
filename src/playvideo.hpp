/*
 * @author Lijing Wang <phdloli@ucla.edu>
 * 
 */

#ifndef PLAYVIDEO_HPP
#define PLAYVIDEO_HPP

#include <fstream>
#include <gst/gst.h>

namespace ndn {

  class PlayVideo
  {
    public:

      PlayVideo();

      void
      play_bin_uri (std::string filename);

      void
      play_bin_appsrc (const uint8_t* buffer, size_t bufferSize);

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
      
        guint8 *data;
        gsize length;
        guint64 offset;
      };
      
      typedef struct _App App;

      App s_app;
      
      #define CHUNK_SIZE  4096
      
      /* This method is called by the need-data signal callback, we feed data into the
       * appsrc.
       */
      static void
      feed_data (GstElement * appsrc, guint size, App * app)
      {
        GstBuffer *buffer;
        guint len;
        GstFlowReturn ret;
      
        buffer = gst_buffer_new ();
      
        if (app->offset >= app->length) {
          /* we are EOS, send end-of-stream */
          g_signal_emit_by_name (app->appsrc, "end-of-stream", &ret);
          return;
        }
      
        /* read the next chunk */
        len = CHUNK_SIZE;
        if (app->offset + len > app->length)
          len = app->length - app->offset;
      
        buffer = gst_buffer_new_wrapped (app->data + app->offset, len);
      
        GST_DEBUG ("feed buffer %p, offset %" G_GUINT64_FORMAT "-%u", buffer,
            app->offset, len);
        g_signal_emit_by_name (app->appsrc, "push-buffer", buffer, &ret);
        //gst_buffer_unref (buffer);
      
        app->offset += len;
      
        return;
      }
      
      /* this callback is called when playbin has constructed a source object to read
       * from. Since we provided the appsrc:// uri to playbin, this will be the
       * appsrc that we must handle. We set up a signals to push data into appsrc. */
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
      
        /* configure the appsrc, we will push a buffer to appsrc when it needs more
         * data */
        g_signal_connect (app->appsrc, "need-data", G_CALLBACK (feed_data), app);
      }
  };
} // namespace ndn
#endif //PLAYVIDEO_HPP
