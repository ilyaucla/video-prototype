/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014 Regents of the University of California.
 *
 * @author Lijing Wang <phdloli@ucla.edu>
 */

#include "producer-callback.hpp"
#include <ndn-cxx/contexts/producer-context.hpp>
// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions
  
  int
  main(int argc, char** argv)
  {
    try {
//  
  		std::string filename = "";
//      std::string streaminfo;
//      char *buffer;
//      long size = 0;
//
  		if (argc >= 2)
  			filename += argv[1];
  		else
  			filename += "duoyan.mp4";
//
      std::cout << "Now produce frames" << std::endl;
      Name videoName2("whole");
      ProducerCallback cb_producer;
      Producer* frameProducer = new Producer(videoName2);
//
      frameProducer->setContextOption(SND_BUF_SIZE,100000);
      frameProducer->setContextOption(DATA_LEAVE_CNTX,
          (ConstDataCallback)bind(&ProducerCallback::processOutgoingData, &cb_producer, _1));
      frameProducer->setContextOption(INTEREST_ENTER_CNTX,
                        (ConstInterestCallback)bind(&ProducerCallback::processIncomingInterest, &cb_producer, _1));

      frameProducer->setup();
//
//	  //uint8_t* payload = new uint8_t[45000000];
//
    size_t lala = 2000;
	  for (int i = 0; i< 2500; i++)
	  {
      lala ++;
		uint8_t* payload = new uint8_t[900];
//		
		frameProducer->produce(Name(std::to_string(i)),payload,900);
		delete[] payload;
	  }
//	  
      std::cout << "HERE!" << std::endl;
      sleep(30000); // because setup() is non-blocking
//
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
