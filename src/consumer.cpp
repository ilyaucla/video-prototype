/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
 *
 * @author Lijing Wang phdloli@ucla.edu
 */

// correct way to include ndn-cxx headers
#include "consumer-callback.hpp"
//#include "contexts/consumer-context.hpp"

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
      Name videoName(filename);
        
      ConsumerCallback stubs;
  
      Consumer* sequenceConsumer = new Consumer(videoName, RELIABLE, SEQUENCE);
      
      sequenceConsumer->setContextOption(INTEREST_LEAVE_CNTX, 
                                (InterestCallback)bind(&ConsumerCallback::processLeavingInterest, &stubs, _1));
    
      sequenceConsumer->setContextOption(DATA_ENTER_CNTX, 
                                (DataCallback)bind(&ConsumerCallback::processData, &stubs, _1));
    
      sequenceConsumer->setContextOption(CONTENT_RETRIEVED, 
                                (ContentCallback)bind(&ConsumerCallback::processPayload, &stubs, _1, _2));
  
  //		sequenceConsumer->setContextOption(SND_BUF_SIZE, 1024*1024*5);
  
  //		sequenceConsumer->setContextOption(RCV_BUF_SIZE, 1024*1024*4);
  		sequenceConsumer->setContextOption(CONTENT_CHUNK_SIZE, 1024*1024*10);
    
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
