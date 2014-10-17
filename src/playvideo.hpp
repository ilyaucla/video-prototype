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


      void
      play_bin_uri (std::string filename);

  };
} // namespace ndn
#endif //PLAYVIDEO_HPP
