/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014 Regents of the University of California.
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
  
  		std::string filename = "/Users/Loli/video/";
      std::string streaminfo;
      char *buffer;
      long size = 0;
      VideoGenerator generator;

  		if (argc >= 2)
  			filename += argv[1];
  		else
  			filename += "duoyan.mp4";

/* get the video file info first, width, height, format etc. Just get the whole caps info
 */
      streaminfo = generator.h264_file_info(filename);
      std::cout << "Streaminfo(Producer Part):  Size=" << streaminfo.size()<< std::endl;
      std::cout << streaminfo << std::endl;
  
      Name videoName(filename);

      Producer* producer = new Producer(videoName);
      producer->setup();
      Name streaminfoSuffix("streaminfo");
      producer->produce(streaminfoSuffix, (uint8_t *)streaminfo.c_str(), streaminfo.size()+1);

      std::cout << "Streaminfo OK!" << std::endl;
      sleep(2); // because setup() is non-blocking
      
      std::cout << "Now produce frames" << std::endl;
      Name videoName2(filename + "whole");
      Producer* producerFrame = new Producer(videoName2);
      ProducerCallback cb_producer;
      cb_producer.setProducer(producerFrame); // needed for some callback functionality

      producerFrame->setContextOption(SND_BUF_SIZE,100000);
      producerFrame->setContextOption(DATA_LEAVE_CNTX,
          (ConstDataCallback)bind(&ProducerCallback::processOutgoingData, &cb_producer, _1));

      producerFrame->setup();
      generator.h264_generate_frames(filename, producerFrame);
//      generator.playbin_generate_frames(filename, producerFrame);
//      There is no need for callback now 
//      ProducerCallback cb_producer;
//      
//      //setting callbacks
//      producerFrame->setContextOption(INTEREST_ENTER_CNTX,
//                        (ConstInterestCallback)bind(&ProducerCallback::processIncomingInterest, &cb_producer, _1));
//                        
//      producerFrame->setContextOption(INTEREST_TO_PROCESS,
//                        (ConstInterestCallback)bind(&ProducerCallback::processInterest, &cb_producer, _1));
  
      //listening
//      
//      buffer = generator.generateVideoOnce(filename, size);
//
//      Name wholeSuffix;
//      producerFrame->produce(wholeSuffix, (uint8_t*)buffer, size);

      std::cout << "HERE!" << std::endl;
      sleep(30000); // because setup() is non-blocking

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
