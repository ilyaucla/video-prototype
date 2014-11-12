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
#include <ndn-cxx/contexts/producer-context.hpp>

namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions

  class VideoGenerator
  {
    public:

      VideoGenerator();
      char * 
      generateVideoOnce(std::string filename, long &size);
      char *
      playbin_file_info (std::string filename);
      void
      playbin_generate_frames (std::string filename, Producer * producer);
      void 
      h264_generate_frames (std::string filename, Producer * producer);
      char * 
      h264_file_info (std::string filename);

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
        format = gst_structure_get_string (str, "stream-format");
        if (!gst_structure_get_int (str, "width", &width) ||
            !gst_structure_get_int (str, "height", &height) ||
            !gst_structure_get_fraction (str, "framerate", &num, &denom)) {
          g_print ("No width/height available\n");
          return;
         }
      //  g_print ("Capabilities:\n the size is %d x %d\n the framerate is %d/%d\n the format is %s\n the type is %s\n", width, height,num,denom,format,type);
        g_print("caps: %s\n", gst_caps_to_string(caps));
      }

     static void 
     on_pad_added (GstElement *element, 
     GstPad *pad, 
     gpointer data) 
     { 
       GstPad *sinkpad; 
       GstCaps *caps;
       const GstStructure *str;
       std::string type;
       GstElement *parser = (GstElement *) data; 

       caps =  gst_pad_get_current_caps (pad);
       str = gst_caps_get_structure (caps, 0);
       type = gst_structure_get_name (str);
     
       std::cout << "TYPE: " << type << std::endl;
       if(type.find("video") != std::string::npos)
       {
         /* We can now link this pad with the h264parse sink pad */ 
         g_print ("Dynamic pad created, linking demuxer/parser\n"); 
         sinkpad = gst_element_get_static_pad (parser, "sink"); 
         gst_pad_link (pad, sinkpad); 
         gst_object_unref (sinkpad); 
       } 
     }
  };
} // namespace ndn
#endif
