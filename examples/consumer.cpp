/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * @author Lijing Wang phdloli@ucla.edu
 */

// correct way to include ndn-cxx headers
#include <ndn-cxx/contexts/consumer-context.hpp>
#include <fstream>
#include <gst/gst.h>
//#include "contexts/consumer-context.hpp"

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions

class CallbackContainer
{

public:
  CallbackContainer()
  {
  }
  
static gboolean
bus_call (GstBus * bus, GstMessage * msg, gpointer data)
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
  processPayload(const uint8_t* buffer, size_t bufferSize)
  {
    std::string content1((char*)buffer);
    std::cout << "bufferSize " << bufferSize <<std::endl;
    //std::cout << "CONTENT " << content1 << std::endl;

		std::string filename = "sequence";

		std::ofstream fout(filename, std::ios::binary);
		fout.write((char*)buffer, sizeof(char)*bufferSize);
		fout.close();
		
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

		fout.write("", 0);
		fout.close();

  }
  
  void
  processData(Data& data)
  {
    std::cout << "DATA IN CNTX" << std::endl;
  }
  
  bool
  verifyData(Data& data)
  {
    return true;
  }
  
  void
  processConstData(const Data& data)
  {}
  
  void
  processLeavingInterest(Interest& interest)
  {
    std::cout << "LEAVES " << interest.toUri() << std::endl;
    std::cout << "LEAVES name" << interest.getName() << std::endl;
  }  
};


int
main(int argc, char** argv)
{
  try {

		std::string filename;
		if(argc >=2 )
			filename = argv[1];
		else
			filename = "/Users/Loli/Video/me.ogg";
    Name videoName(filename);
      
    CallbackContainer stubs;

    Consumer* sequenceConsumer = new Consumer(videoName, RELIABLE, SEQUENCE);
    
    sequenceConsumer->setContextOption(INTEREST_LEAVE_CNTX, 
                              (InterestCallback)bind(&CallbackContainer::processLeavingInterest, &stubs, _1));
  
    sequenceConsumer->setContextOption(DATA_ENTER_CNTX, 
                              (DataCallback)bind(&CallbackContainer::processData, &stubs, _1));
  
    sequenceConsumer->setContextOption(CONTENT_RETRIEVED, 
                              (ContentCallback)bind(&CallbackContainer::processPayload, &stubs, _1, _2));

//		sequenceConsumer->setContextOption(SND_BUF_SIZE, 1024*1024*5);

//		sequenceConsumer->setContextOption(RCV_BUF_SIZE, 1024*1024*4);
		sequenceConsumer->setContextOption(CONTENT_CHUNK_SIZE, 1024*1024*4);
  
    sequenceConsumer->consume(Name());
    
    sleep(3000); // because consume() is non-blocking
  }
  catch(std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}

} // namespace ndn

int
main(int argc, char** argv)
{
  return ndn::main(argc, argv);
}
