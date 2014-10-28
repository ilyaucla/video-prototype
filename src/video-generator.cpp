/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014 Regents of the University of California.
 *
 * @author Lijing Wang <phdloli@ucla.edu>
 */

#include "video-generator.hpp"
#include <fstream>
#include <iostream>

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions
  VideoGenerator::VideoGenerator()
  {
  }

  char *
  VideoGenerator::generateVideoOnce(std::string filename, long &size)
  {
  	std::ifstream filestr;
    char *buffer;
  	// binary open
  	filestr.open (filename, std::ios::binary);
  	// get the size
  	filestr.seekg(0, std::ios::end);
  	size = filestr.tellg();
  	filestr.seekg(0, std::ios::beg);
  	// malloc the memory
  	buffer=new char[size];
    
  	filestr.read(buffer, size);
  	std::cout << "byte = " << size << std::endl;
  	std::cout << "content = " << buffer << std::endl;
  
  	filestr.close();
    return buffer;
  }
  /*
   * Videogenrator will call this function to setup the stream/file info
   */
  void
  VideoGenerator::playbin_file_info (std::string filename)
  {
    GstElement *playbin;
   // GstElement *fakevideosink, *fakeaudiosink;
    gchar *uri;
    GstState state = GST_STATE_NULL;
    GstPad *pad;
    GstCaps *caps;

    gst_init (NULL, NULL);

    playbin = gst_element_factory_make ("playbin", NULL);

    /* take the commandline argument and ensure that it is a uri */
    uri = gst_filename_to_uri (filename.c_str(), NULL);

//    fakevideosink = gst_element_factory_make ("fakesink", NULL);
//    fakeaudiosink = gst_element_factory_make ("fakesink", NULL);
//    g_object_set (playbin, "video-sink", fakevideosink, NULL);
//    g_object_set (playbin, "audio-sink", fakeaudiosink, NULL);

    g_object_set (playbin, "uri", uri, NULL);
    g_free (uri);

    gst_element_set_state (playbin, GST_STATE_PAUSED);
    gst_element_get_state (playbin, &state, NULL, GST_SECOND * 1);
    g_signal_emit_by_name (playbin, "get-video-pad", 0, &pad, NULL);

    caps =  gst_pad_get_current_caps (pad);
    read_video_props(caps);

    /* cleanup */
    gst_element_set_state (playbin, GST_STATE_NULL);
    g_object_unref (playbin);
    return; 
  }
} // namespace ndn
