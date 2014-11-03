/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014 Regents of the University of California.
 *
 * @author Lijing Wang <phdloli@ucla.edu>
 */

#include "video-generator.hpp"
#include <fstream>
#include <iostream>
#include <ctime>

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
//  	std::cout << "content = " << buffer << std::endl;
  
  	filestr.close();
    return buffer;
  }
  /*
   * Videogenrator will call this function to setup the stream/file info
   */
  char *
  VideoGenerator::playbin_file_info (std::string filename)
  {
    GstElement *playbin;
   // GstElement *fakevideosink, *fakeaudiosink;
    gchar *uri;
    GstState state = GST_STATE_NULL;
    GstPad *pad;
    GstCaps *caps;
    char * streaminfo;

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
//    read_video_props(caps);
    streaminfo = gst_caps_to_string(caps);

    /* cleanup */
    gst_element_set_state (playbin, GST_STATE_NULL);
    g_object_unref (playbin);
    return streaminfo; 
  }

  /*
   *generate frames from the video file, will block several seconds now
   */
  void
  VideoGenerator::playbin_generate_frames (std::string filename, Producer * producer)
  {
    GstElement *playbin, *fakevideosink, *fakeaudiosink;
    gchar *uri;

    GstBuffer *buffer;
    GstSample *sample;
    GstMapInfo map;
    //const GstStructure * info;
    //GstSegment *segment;
    //GstCaps *caps;

    gst_init (NULL, NULL);
  
    playbin = gst_element_factory_make ("playbin", NULL);
  
    /* take the commandline argument and ensure that it is a uri */
    uri = gst_filename_to_uri (filename.c_str(), NULL);
  
    fakevideosink = gst_element_factory_make ("appsink", NULL);
    fakeaudiosink = gst_element_factory_make ("appsink", NULL);
  
    g_object_set (playbin, "uri", uri, NULL);
    g_free (uri);
  
    g_object_set (playbin, "video-sink", fakevideosink, NULL);
    g_object_set (playbin, "audio-sink", fakeaudiosink, NULL);
  
    g_object_set (G_OBJECT (fakevideosink), "sync", FALSE, NULL); 
  
    time_t time_start = std::time(0);
    gst_element_set_state (playbin, GST_STATE_PLAYING);
    int framenumber = 0;
  
    do {
      g_signal_emit_by_name (fakevideosink, "pull-sample", &sample);
//      g_print("After calling pull-sample  ");
      if (sample == NULL){
        g_print("Meet the EOS!\n");
        break;
        }
//      caps = gst_sample_get_caps(sample);
      buffer = gst_sample_get_buffer (sample);
//      info = gst_sample_get_info(sample);
//      segment = gst_sample_get_segment(sample);
      gst_buffer_map (buffer, &map, GST_MAP_READ);
      Name frameSuffix("frames/"+std::to_string(framenumber));
      std::cout << "Frame number: "<<framenumber <<std::endl;
      producer->produce(frameSuffix, (uint8_t *)map.data, map.size);
      std::cout << "Frame number: "<<framenumber << "FINISHED" <<std::endl;
      framenumber ++;
  
//      std::cout << "Gstbuffer size: " << gst_buffer_get_size (buffer) << std::endl;
//      std::cout << "Map size: " << map.size << std::endl;
//      std::cout << "Segment Info: position " << segment->position <<" duration " << segment->duration << std::endl;
//      read_video_props(caps);
//      if (info !=NULL)
//        std::cout << "Sample INFO " << gst_structure_to_string (info) <<std::endl;
//    g_print ("got sample: %p", sample);
  
      if (sample)
        gst_sample_unref (sample);
//      }while (sample != NULL);
      }while (framenumber <= 24);
  
      g_print("Should be here\n");
  
  //  do {
  //    g_signal_emit_by_name (fakeaudiosink, "pull-sample", &sample);
  //    buffer = gst_sample_get_buffer (sample);
  //    gst_buffer_map (buffer, &map, GST_MAP_READ);
  //    std::cout << "Map size: " << map.size << std::endl;
  //
  //    if (sample)
  //      gst_sample_unref (sample);
  //    }while (sample != NULL);
  
    time_t time_end = std::time(0);
    double seconds = difftime(time_end, time_start);
    std::cout << seconds << " seconds have passed" << std::endl;
  
    /* cleanup */
    gst_element_set_state (playbin, GST_STATE_NULL);
    g_object_unref (playbin);
  }
} // namespace ndn
