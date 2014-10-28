/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014 Regents of the University of California.
 *
 * @author Lijing Wang <phdloli@ucla.edu>
 */

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
#ifndef VIDEO_GENERATOR_HPP
#define VIDEO_GENERATOR_HPP

#include <string>
#include <gst/gst.h>

namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions

  class VideoGenerator
  {
    public:

      VideoGenerator();
      char * 
      generateVideoOnce(std::string filename, long &size);
      void
      playbin_file_info (std::string filename);

    private:

      static void
      read_video_props (GstCaps *caps)
      {
        gint width, height, num, denom;
        const GstStructure *str;
        const char *format;
        const char *type;
      
        g_return_if_fail (gst_caps_is_fixed (caps));
      
        str = gst_caps_get_structure (caps, 0);
        type = gst_structure_get_name (str);
        format = gst_structure_get_string (str, "format");
        if (!gst_structure_get_int (str, "width", &width) ||
            !gst_structure_get_int (str, "height", &height) ||
            !gst_structure_get_fraction (str, "framerate", &num, &denom)) {
          g_print ("No width/height available\n");
          return;
         }
        g_print ("The video size of this set of capabilities is %d x %d and the frame rate is %d/%d\nformat:%s type:%s\n",width, height,num,denom,format,type);
        g_print("caps: %s\n", gst_caps_to_string(caps));
      }
  };
} // namespace ndn
#endif
