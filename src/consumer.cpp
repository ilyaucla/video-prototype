/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014 Regents of the University of California.
 *
 * @author Lijing Wang phdloli@ucla.edu
 */

#include "consumer-callback.hpp"

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions
  int
  main(int argc, char** argv)
  {
    try {
  
  		std::string filename;
  		if(argc >=2 )
  			filename = argv[1];
  		else
  			filename = "/Users/Loli/Video/me.ogg";
/*
      Name videoConfig(filename+"/Config");
      Consumer* configConsumer = new Consumer(videoConfig, RELIABLE, DATAGRAM );
      configConsumer->setContextOption(MUST_BE_FRESH_S, true);
      ConsumerCallback stubsConfig;
      configConsumer->setContextOption(CONTENT_RETRIEVED, 
                                (ContentCallback)bind(&ConsumerCallback::processConfig, &stubsConfig, _1, _2));

      configConsumer->consume(Name());
      sleep(3000); // because consume() is non-blocking
*/

      Name videoName(filename);
      Consumer* sequenceConsumer = new Consumer(videoName, RELIABLE, SEQUENCE);
      ConsumerCallback stubs;
      
     // there no need for other callback now
     // sequenceConsumer->setContextOption(INTEREST_LEAVE_CNTX, 
     //                           (InterestCallback)bind(&ConsumerCallback::processLeavingInterest, &stubs, _1));
    
     //sequenceConsumer->setContextOption(DATA_ENTER_CNTX, 
     //                           (DataCallback)bind(&ConsumerCallback::processData, &stubs, _1));
     
      sequenceConsumer->setContextOption(CONTENT_RETRIEVED, 
                                (ContentCallback)bind(&ConsumerCallback::processPayload, &stubs, _1, _2));
  
  //	sequenceConsumer->setContextOption(SND_BUF_SIZE, 1024*1024*5);
  //	sequenceConsumer->setContextOption(RCV_BUF_SIZE, 1024*1024*4);
  		sequenceConsumer->setContextOption(CONTENT_CHUNK_SIZE, 1024*1024*10);
      sequenceConsumer->consume(Name());
//      stubs.player.playbin_appsrc_init ();
      
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
