/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
 *
 * @author Lijing Wang <phdloli@ucla.edu>
 */

#include "producer-callback.hpp"
#include "video-generator.hpp"

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions
  
  int
  main(int argc, char** argv)
  {
    try {
  
  		std::string filename;
      char *buffer;
      long size = 0;
  		if (argc >= 2)
  			filename = argv[1];
  		else
  			filename = "/Users/Loli/Video/me.ogg";
  
      Name videoName(filename);
      
      ProducerCallback stubs;
  
      Producer* sequenceProducer = new Producer(videoName);
      
      stubs.setProducer(sequenceProducer); // needed for some callback functionality
      
      //setting callbacks
      sequenceProducer->setContextOption(INTEREST_ENTER_CNTX,
                        (ConstInterestCallback)bind(&ProducerCallback::processIncomingInterest, &stubs, _1));
                        
      sequenceProducer->setContextOption(INTEREST_TO_PROCESS,
                        (ConstInterestCallback)bind(&ProducerCallback::processInterest, &stubs, _1));
  
      
      //listening
      sequenceProducer->setup();
      
   //   buffer = NULL;
      VideoGenerator generator;
      buffer = generator.generateVideoOnce(filename, size);

      Name emptySuffix;
      sequenceProducer->produce(emptySuffix, (uint8_t*)buffer, size);

      std::cout << "HERE!" << std::endl;
      sleep(300); // because setup() is non-blocking
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
