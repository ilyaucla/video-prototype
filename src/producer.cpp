/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014 Regents of the University of California.
 *
 * @author Lijing Wang <phdloli@ucla.edu>
 */

#include "producer-callback.hpp"
#include "video-generator.hpp" 
//#include <iostream>
// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions
#define IDENTITY_NAME "/Lijing/Wang"  

class Signer
{
public:
  Signer()
  : m_identityName(IDENTITY_NAME)
  {
    m_keyChain.createIdentity(m_identityName);
  };
  
  void
  onPacket(Data& data)
  {
    m_keyChain.signByIdentity(data, m_identityName);
  }
  
private:
  KeyChain m_keyChain;
  Name m_identityName;
};

  int
  main(int argc, char** argv)
  {
    try {
      
  		std::string filename = "/Users/Loli/video/";
      VideoGenerator generator;

  		if (argc >= 2)
  			filename += argv[1];
  		else
  			filename += "duoyan.mp4";
  
      Name videoName(filename + "streaminfo");

      ProducerCallback cb_producer;
      Producer* streaminfoProducer = new Producer(videoName);
      cb_producer.setProducer(streaminfoProducer); // needed for some callback functionality
      streaminfoProducer->setContextOption(DATA_LEAVE_CNTX,
          (ConstDataCallback)bind(&ProducerCallback::processOutgoingData, &cb_producer, _1));
      streaminfoProducer->setup();
//      Name streaminfoVideoSuffix("streaminfo/video");
//      producer->produce(streaminfoVideoSuffix, (uint8_t *)streaminfo[0].c_str(), streaminfo[0].size()+1);

//      Name streaminfoAudioSuffix("streaminfo/audio");
//      producer->produce(streaminfoAudioSuffix, (uint8_t *)streaminfo[0].c_str(), streaminfo[0].size()+1);

//      std::cout << "Streaminfo Video with Audio OK!" << std::endl;
//      sleep(2); // because setup() is non-blocking
      
//      std::cout << "Now produce frames" << std::endl;
      Signer signer;
      Name videoName2(filename + "content");
      Producer* frameProducer = new Producer(videoName2);
      cb_producer.setProducer(frameProducer); // needed for some callback functionality

//      frameProducer->setContextOption(EMBEDDED_MANIFESTS, true);
//      frameProducer->setContextOption(DATA_TO_SECURE,
//                      (DataCallback)bind(&Signer::onPacket, &signer, _1));
      frameProducer->setContextOption(SND_BUF_SIZE,100000);
      frameProducer->setContextOption(DATA_LEAVE_CNTX,
          (ConstDataCallback)bind(&ProducerCallback::processOutgoingData, &cb_producer, _1));
//      frameProducer->setContextOption(INTEREST_ENTER_CNTX,
//                        (ConstInterestCallback)bind(&ProducerCallback::processIncomingInterest, &cb_producer, _1));
//      frameProducer->setContextOption(INTEREST_TO_PROCESS,
//                        (ConstInterestCallback)bind(&ProducerCallback::processInterest, &cb_producer, _1));
      frameProducer->setup();
      
      Producer* sampleProducer = new Producer(videoName2);
      cb_producer.setProducer(sampleProducer); // needed for some callback functionality
      sampleProducer->setContextOption(SND_BUF_SIZE,100000);
      sampleProducer->setContextOption(DATA_LEAVE_CNTX,
          (ConstDataCallback)bind(&ProducerCallback::processOutgoingData, &cb_producer, _1));
      sampleProducer->setContextOption(INTEREST_ENTER_CNTX,
                        (ConstInterestCallback)bind(&ProducerCallback::processIncomingInterest, &cb_producer, _1));
      sampleProducer->setup();

      generator.h264_generate_whole(filename, streaminfoProducer, frameProducer);
      std::cout << "COOL~" << std::endl;
//      generator.h264_generate_capture(streaminfoProducer, frameProducer);

//      generator.h264_generate_frames(filename, frameProducer);
//      generator.playbin_generate_frames(filename, frameProducer);
//      There is no need for callback now 
//      ProducerCallback cb_producer;
//      
//      //setting callbacks
      //listening
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
