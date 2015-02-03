/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Regents of the University of California.
 *
 * @author Lijing Wang <phdloli@ucla.edu>
 */

#ifndef CONSUMER_HPP
#define CONSUMER_HPP
#include "consumer-callback.hpp"
#include <pthread.h>
#include <ndn-cxx/security/validator.hpp>

namespace ndn{

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

  struct Consumer_Need
  {
    std::string filename;
    std::string name;
    ConsumerCallback *cb;
  };

  static void
  *consume_thread (void * threadData)
  {
//    Verificator* verificator = new Verificator();
    Consumer_Need *con;
    con = (Consumer_Need *) threadData;
    int end;
    int sleeptime;

    Name sampleName(con->filename + "/" + con->name + "/" + "content");

    Consumer* sampleConsumer = new Consumer(sampleName, RELIABLE, SEQUENCE);
    if(con->name == "video")
    {
//      sampleConsumer->setContextOption(EMBEDDED_MANIFESTS, true);
      sampleConsumer->setContextOption(CONTENT_RETRIEVAL_SIZE, 1024*1024);
      sampleConsumer->setContextOption(CONTENT_RETRIEVED, 
                          (ContentCallback)bind(&ConsumerCallback::processPayload, con->cb, _1, _2));
//      sampleConsumer->setContextOption(DATA_TO_VERIFY,
//            (DataVerificationCallback)bind(&Verificator::onPacket, verificator, _1));

      end = 13000000;
      sleeptime = 0;
    }else
    {
      sampleConsumer->setContextOption(CONTENT_RETRIEVED, 
                          (ContentCallback)bind(&ConsumerCallback::processPayloadAudio, con->cb, _1, _2));
//      sampleConsumer->setContextOption(DATA_TO_VERIFY,
//            (DataVerificationCallback)bind(&Verificator::onPacket, verificator, _1));
      end = 13000000;
      sleeptime = 0;
    }
        
    sampleConsumer->setContextOption(MUST_BE_FRESH_S, true);
    sampleConsumer->setContextOption(INTEREST_LIFETIME, 200);
//    sampleConsumer->setContextOption(INTEREST_RETX,5); //Retransmitted Attempted Time.
   // there is no need for other callback now

//    sampleConsumer->setContextOption(MIN_WINDOW_SIZE, 1);
    sampleConsumer->setContextOption(INTEREST_LEAVE_CNTX, 
                              (InterestCallback)bind(&ConsumerCallback::processLeavingInterest, con->cb, _1));
    sampleConsumer->setContextOption(INTEREST_RETRANSMIT, 
                              (InterestCallback)bind(&ConsumerCallback::onRetx, con->cb, _1));
    sampleConsumer->setContextOption(DATA_ENTER_CNTX, 
                              (DataCallback)bind(&ConsumerCallback::processData, con->cb, _1));

    
    for (int i=0; i<end; i++)
    { 
      Name sampleSuffix(std::to_string(i));
      sampleConsumer->consume(sampleSuffix);
      usleep(sleeptime);
    }
    pthread_exit(NULL);
  }
}
#endif
