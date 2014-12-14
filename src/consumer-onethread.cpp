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
  //  return true;
    if (Validator::verifySignature(data, *m_publicKey))
    {
      std::cout << "Verified Name:" << data.getName() <<  std::endl;
      return true;
    }
    else
    {
      std::cout << "NOT Verified" << data.getName() << std::endl;
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
  
  		std::string filename = "/Users/Lijing/Movies/duoyan.mp4";
//  		if(argc >=2 )
//  			filename += argv[1];
//  		else
//  			filename += "duoyan.mp4";
      std::cout<<filename<<std::endl;

      ConsumerCallback cb_consumer;

      Name videoinfoName(filename + "/video/streaminfo");
      Consumer* videoinfoConsumer = new Consumer(videoinfoName, RELIABLE, DATAGRAM );
      videoinfoConsumer->setContextOption(MUST_BE_FRESH_S, true);
      videoinfoConsumer->setContextOption(INTEREST_LEAVE_CNTX, 
        (InterestCallback)bind(&ConsumerCallback::processLeavingInterest, &cb_consumer, _1));
      videoinfoConsumer->setContextOption(CONTENT_RETRIEVED, 
        (ContentCallback)bind(&ConsumerCallback::processStreaminfo, &cb_consumer, _1, _2));

      videoinfoConsumer->consume(Name());

      Name audioinfoName(filename + "/audio/streaminfo");
      Consumer* audioinfoConsumer = new Consumer(audioinfoName, RELIABLE, DATAGRAM );
      audioinfoConsumer->setContextOption(MUST_BE_FRESH_S, true);
      audioinfoConsumer->setContextOption(INTEREST_LEAVE_CNTX, 
        (InterestCallback)bind(&ConsumerCallback::processLeavingInterest, &cb_consumer, _1));
      audioinfoConsumer->setContextOption(CONTENT_RETRIEVED, 
        (ContentCallback)bind(&ConsumerCallback::processStreaminfoAudio, &cb_consumer, _1, _2));

      audioinfoConsumer->consume(Name());

      sleep(2); // because consume() is non-blocking
      std::cout << "consume whole start!" <<std::endl;
      
      Verificator* verificator = new Verificator();
      Name videoName(filename + "/video/content");

      Consumer* videoConsumer = new Consumer(videoName, RELIABLE, SEQUENCE);
      videoConsumer->setContextOption(EMBEDDED_MANIFESTS, true);
      videoConsumer->setContextOption(MUST_BE_FRESH_S, true);
      videoConsumer->setContextOption(INTEREST_LIFETIME, 200);
     // there is no need for other callback now
      videoConsumer->setContextOption(DATA_TO_VERIFY,
                      (DataVerificationCallback)bind(&Verificator::onPacket, verificator, _1));
      videoConsumer->setContextOption(CONTENT_RETRIEVED, 
                                (ContentCallback)bind(&ConsumerCallback::processPayload, &cb_consumer, _1, _2));
  		videoConsumer->setContextOption(CONTENT_RETRIEVAL_SIZE, 1024*1024);

//      videoConsumer->setContextOption(MIN_WINDOW_SIZE, 1);
      videoConsumer->setContextOption(INTEREST_RETX,0); //Retransmitted Attempted Time.
      videoConsumer->setContextOption(INTEREST_LEAVE_CNTX, 
                                (InterestCallback)bind(&ConsumerCallback::processLeavingInterest, &cb_consumer, _1));
    
      videoConsumer->setContextOption(INTEREST_RETRANSMITTED, 
                                (ConstInterestCallback)bind(&ConsumerCallback::onRetx, &cb_consumer, _1));
      videoConsumer->setContextOption(DATA_ENTER_CNTX, 
                                (DataCallback)bind(&ConsumerCallback::processData, &cb_consumer, _1));
 
//	    videoConsumer->setContextOption(SND_BUF_SIZE, 1024*1024*5);
//    	videoConsumer->setContextOption(RCV_BUF_SIZE, 1024*1024*4);
//  		videoConsumer->setContextOption(CONTENT_CHUNK_SIZE, 1024*1024*10);

      Name audioName(filename + "/audio/content");
      Consumer* audioConsumer = new Consumer(audioName, RELIABLE, SEQUENCE);
//      audioConsumer->setContextOption(EMBEDDED_MANIFESTS, true);
      audioConsumer->setContextOption(MUST_BE_FRESH_S, true);
      audioConsumer->setContextOption(INTEREST_LIFETIME, 200);
      audioConsumer->setContextOption(INTEREST_RETX,0); //Retransmitted Attempted Time.
      audioConsumer->setContextOption(DATA_TO_VERIFY,
                      (DataVerificationCallback)bind(&Verificator::onPacket, verificator, _1));
      audioConsumer->setContextOption(CONTENT_RETRIEVED, 
                                (ContentCallback)bind(&ConsumerCallback::processPayloadAudio, &cb_consumer, _1, _2));

      audioConsumer->setContextOption(INTEREST_LEAVE_CNTX, 
                                (InterestCallback)bind(&ConsumerCallback::processLeavingInterest, &cb_consumer, _1));
      audioConsumer->setContextOption(INTEREST_RETRANSMITTED, 
                                (ConstInterestCallback)bind(&ConsumerCallback::onRetx, &cb_consumer, _1));
      audioConsumer->setContextOption(DATA_ENTER_CNTX, 
                                (DataCallback)bind(&ConsumerCallback::processData, &cb_consumer, _1));
//      int i = 0;
      time_t time_start_0 = std::time(0);
      std::cout << "Before consume " << time_start_0 << std::endl;

//      cb_consumer.player.consume_whole(videoConsumer, audioConsumer);
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
