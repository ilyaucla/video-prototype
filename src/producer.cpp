/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014 Regents of the University of California.
 *
 * @author Lijing Wang <phdloli@ucla.edu>
 */

#include "video-generator.hpp" 
//#include <iostream>
// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions


  int
  main(int argc, char** argv)
  {
    try {
      
  		std::string filename = "/Users/Lijing/Movies/";
      VideoGenerator generator;

  		if (argc >= 2)
  			filename += argv[1];
  		else
  			filename += "duoyan.mp4";
  
     
      /* streaminfoSampleProducer */
//      Producer* audioinfoProducer = new Producer(videoName);
//      cb_producer.setProducer(audioinfoProducer); // needed for some callback functionality
//      audioinfoProducer->setContextOption(DATA_LEAVE_CNTX,
//          (ConstDataCallback)bind(&ProducerCallback::processOutgoingData, &cb_producer, _1));
//      audioinfoProducer->setup();
//
//      
//      Producer* sampleProducer = new Producer(videoName2);
//      cb_producer.setProducer(sampleProducer); // needed for some callback functionality
//      sampleProducer->setContextOption(DATA_TO_SECURE,
//                      (DataCallback)bind(&Signer::onPacket, &signer, _1));
//      sampleProducer->setContextOption(SND_BUF_SIZE,100000);
//      sampleProducer->setContextOption(DATA_LEAVE_CNTX,
//          (ConstDataCallback)bind(&ProducerCallback::processOutgoingData, &cb_producer, _1));
//      sampleProducer->setContextOption(INTEREST_ENTER_CNTX,
//                        (ConstInterestCallback)bind(&ProducerCallback::processIncomingInterest, &cb_producer, _1));
//      sampleProducer->setup();

//      generator.h264_generate_whole(filename);
      filename = "/Capture";
      generator.h264_generate_capture(filename);

      std::cout << "COOL~" << std::endl;

//      generator.h264_generate_frames(filename, frameProducer);
//      generator.playbin_generate_frames(filename, frameProducer);
//      There is no need for callback now 
//      ProducerCallback cb_producer;
//      
//      buffer = generator.generateVideoOnce(filename, size);
//
//      Name wholeSuffix;
//      frameProducer->produce(wholeSuffix, (uint8_t*)buffer, size);

      sleep(30000); // because setup() is non-blocking
      
      std::cout << "HERE!!" << std::endl;

    }
    catch(std::exception& e) {
      std::cout << "HAHA" << std::endl;
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
