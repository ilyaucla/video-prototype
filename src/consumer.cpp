/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014 Regents of the University of California.
 *
 * @author Lijing Wang phdloli@ucla.edu
 */

#include "consumer-callback.hpp"
#include <pthread.h>
#include <ndn-cxx/security/validator.hpp>

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions

#define IDENTITY_NAME "/Lijing/Wang"

class Verificator
{
public:
  Verificator()
  {
    Name identity(IDENTITY_NAME);
    Name keyName = m_keyChain.getDefaultKeyNameForIdentity(identity);
    m_publicKey = m_keyChain.getPublicKey(keyName); 
  };
  
  bool
  onPacket(Data& data)
  {
    return true;
    if (Validator::verifySignature(data, *m_publicKey))
    {
      std::cout << "Verified" << std::endl;
      return true;
    }
    else
    {
      std::cout << "NOT Verified" << std::endl;
      return false;
    }
  }
  
private:
  KeyChain m_keyChain;
  shared_ptr<PublicKey> m_publicKey;
};

  int
  main(int argc, char** argv)
  {
    try {
  
  		std::string filename = "/Users/Loli/video/";
  		if(argc >=2 )
  			filename += argv[1];
  		else
  			filename += "duoyan.mp4";
      std::cout<<filename<<std::endl;

      Name videoName(filename + "streaminfo");
      ConsumerCallback cb_consumer;

      Consumer* streaminfoConsumer = new Consumer(videoName, RELIABLE, DATAGRAM );
      streaminfoConsumer->setContextOption(MUST_BE_FRESH_S, true);
      streaminfoConsumer->setContextOption(INTEREST_LEAVE_CNTX, 
        (InterestCallback)bind(&ConsumerCallback::processLeavingInterest, &cb_consumer, _1));
      streaminfoConsumer->setContextOption(CONTENT_RETRIEVED, 
        (ContentCallback)bind(&ConsumerCallback::processStreaminfo, &cb_consumer, _1, _2));

      streaminfoConsumer->consume(Name("video"));

      Consumer* audioinfoConsumer = new Consumer(videoName, RELIABLE, DATAGRAM );
      audioinfoConsumer->setContextOption(MUST_BE_FRESH_S, true);
      audioinfoConsumer->setContextOption(INTEREST_LEAVE_CNTX, 
        (InterestCallback)bind(&ConsumerCallback::processLeavingInterest, &cb_consumer, _1));
      audioinfoConsumer->setContextOption(CONTENT_RETRIEVED, 
        (ContentCallback)bind(&ConsumerCallback::processStreaminfoAudio, &cb_consumer, _1, _2));

      audioinfoConsumer->consume(Name("audio"));

      sleep(2); // because consume() is non-blocking
      std::cout << "consume whole start!" <<std::endl;
      
      Verificator* verificator = new Verificator();
      Name videoName2(filename + "content");

      Consumer* frameConsumer = new Consumer(videoName2, RELIABLE, SEQUENCE);
//      frameConsumer->setContextOption(EMBEDDED_MANIFESTS, true);
      frameConsumer->setContextOption(MUST_BE_FRESH_S, true);
      frameConsumer->setContextOption(INTEREST_LIFETIME, 200);
//      frameConsumer->setContextOption(MIN_WINDOW_SIZE, 1);
     // there is no need for other callback now
      frameConsumer->setContextOption(DATA_TO_VERIFY,
                      (DataVerificationCallback)bind(&Verificator::onPacket, verificator, _1));

//      frameConsumer->setContextOption(INTEREST_LEAVE_CNTX, 
//                                (InterestCallback)bind(&ConsumerCallback::processLeavingInterest, &cb_consumer, _1));
//    
//      frameConsumer->setContextOption(INTEREST_RETRANSMITTED, 
//                                (ConstInterestCallback)bind(&ConsumerCallback::onRetx, &cb_consumer, _1));
//      frameConsumer->setContextOption(DATA_ENTER_CNTX, 
//                                (DataCallback)bind(&ConsumerCallback::processData, &cb_consumer, _1));
   
      frameConsumer->setContextOption(CONTENT_RETRIEVED, 
                                (ContentCallback)bind(&ConsumerCallback::processPayload, &cb_consumer, _1, _2));
 
  //	frameConsumer->setContextOption(SND_BUF_SIZE, 1024*1024*5);
  //	frameConsumer->setContextOption(RCV_BUF_SIZE, 1024*1024*4);

//  		frameConsumer->setContextOption(CONTENT_CHUNK_SIZE, 1024*1024*10);
  		frameConsumer->setContextOption(CONTENT_RETRIEVAL_SIZE, 1024*1024);

      Consumer* sampleConsumer = new Consumer(videoName2, RELIABLE, SEQUENCE);
//      sampleConsumer->setContextOption(EMBEDDED_MANIFESTS, true);
      sampleConsumer->setContextOption(MUST_BE_FRESH_S, true);
      sampleConsumer->setContextOption(INTEREST_LIFETIME, 200);
     // there is no need for other callback now
 //     sampleConsumer->setContextOption(INTEREST_LEAVE_CNTX, 
 //                               (InterestCallback)bind(&ConsumerCallback::processLeavingInterest, &cb_consumer, _1));
    
      sampleConsumer->setContextOption(DATA_TO_VERIFY,
                      (DataVerificationCallback)bind(&Verificator::onPacket, verificator, _1));

//      sampleConsumer->setContextOption(INTEREST_RETRANSMITTED, 
//                                (ConstInterestCallback)bind(&ConsumerCallback::onRetx, &cb_consumer, _1));
//      sampleConsumer->setContextOption(DATA_ENTER_CNTX, 
//                                (DataCallback)bind(&ConsumerCallback::processData, &cb_consumer, _1));
   
      sampleConsumer->setContextOption(CONTENT_RETRIEVED, 
                                (ContentCallback)bind(&ConsumerCallback::processPayloadAudio, &cb_consumer, _1, _2));
 
      int i = 0;
      time_t time_start_0 = std::time(0);
      std::cout << "Before consume " << time_start_0 << std::endl;

      cb_consumer.player.consume_whole(frameConsumer, sampleConsumer);
//      while (i < 25*60*5)
//      {
//        Name frameSuffix("video/" + std::to_string(i));
//        frameConsumer->consume(frameSuffix);
//        Name sampleSuffix("audio/" + std::to_string(i));
//        sampleConsumer->consume(sampleSuffix);
//        i++;
//      }
      time_t time_end_0  = std::time(0);
      std::cout << "After consume " << time_end_0 << std::endl;
      double seconds = difftime(time_end_0, time_start_0);
      std::cout << seconds << " seconds have passed" << std::endl;

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
