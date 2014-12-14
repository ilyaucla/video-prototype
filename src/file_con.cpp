/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014 Regents of the University of California.
 *
 * @author Lijing Wang phdloli@ucla.edu
 */

#include "consumer-callback.hpp"
//#include <pthread.h>

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions
  int
  main(int argc, char** argv)
  {
    try {
  
  		std::string filename = "";
  		filename += "/Users/Lijing/Movies/c.mp3";
      std::cout<<filename<<std::endl;

//      ConsumerCallback cb_consumer;
//      Name videoName(filename);
      ConsumerCallback cb_consumer;

      std::cout << "consume whole start!" <<std::endl;
      
      Name videoName2("whole");

      Consumer* frameConsumer = new Consumer(videoName2, RELIABLE, SEQUENCE);
      frameConsumer->setContextOption(MUST_BE_FRESH_S, true);
      frameConsumer->setContextOption(INTEREST_LIFETIME, 1000);
      frameConsumer->setContextOption(MIN_WINDOW_SIZE, 1);
/*
      Name videoName(filename);
      Consumer* frameConsumer = new Consumer(videoName, RELIABLE, SEQUENCE);
      ConsumerCallback cb_consumer;
 */     
     // there is no need for other callback now
      frameConsumer->setContextOption(INTEREST_LEAVE_CNTX, 
                                (InterestCallback)bind(&ConsumerCallback::processLeavingInterest, &cb_consumer, _1));
    
      frameConsumer->setContextOption(INTEREST_RETRANSMITTED, 
                                (ConstInterestCallback)bind(&ConsumerCallback::onRetx, &cb_consumer, _1));
      frameConsumer->setContextOption(DATA_ENTER_CNTX, 
                                (DataCallback)bind(&ConsumerCallback::processData, &cb_consumer, _1));
   
      frameConsumer->setContextOption(CONTENT_RETRIEVED, 
                                (ContentCallback)bind(&ConsumerCallback::processFile, &cb_consumer, _1, _2));
 
  //	frameConsumer->setContextOption(SND_BUF_SIZE, 1024*1024*5);
  //	frameConsumer->setContextOption(RCV_BUF_SIZE, 1024*1024*4);

//  		frameConsumer->setContextOption(CONTENT_CHUNK_SIZE, 1024*1024*10);
//  		frameConsumer->setContextOption(CONTENT_RETRIEVAL_SIZE, 1024*1024);
      int i = 0;
      while (i < 1981)
      {
        Name frameSuffix(std::to_string(i));
    //    std::cout << i << " Consume" << std::endl;
        frameConsumer->consume(frameSuffix);
        i++;
      }
//      frameConsumer->consume(Name());

//      std::cout << "appsrc_init" <<std::endl;
      //cb_consumer.player.playbin_appsrc_init ();
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
