/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014 Regents of the University of California.
 *
 * @author Lijing Wang <phdloli@ucla.edu>
 */

#include "producer-callback.hpp"

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentiojs
  
  ProducerCallback::ProducerCallback()
  {
  }
  
  void
  ProducerCallback::setProducer(Producer* p)
  {
    m_producer = p;
  }
  
  void
  ProducerCallback::setSampleNumber(size_t* n)
  {
    m_curnum = n;
  }
  
  void
  ProducerCallback::setStreaminfoVideo(std::string video)
  {
    m_streaminfovideo = video;  
  }

  void 
  ProducerCallback::setStreaminfoAudio(std::string audio)
  {
    m_streaminfoaudio = audio;
  }

  void
  ProducerCallback::processConstData(const Data& data){}
  
   /* When the request can't be satisfied from the content store */
  void
  ProducerCallback::processStreaminfoInterest(const Interest& interest)
  {
    interest_nohit ++; 
    std::cout << "NO HIT STREAMINFO Interest!" << interest.getName().toUri() << std::endl;
    std::string type = interest.getName().get(-2).toUri();
    std::string streaminfo;
    if(type == "video")
    {
      streaminfo = m_streaminfovideo;
    }else
    {
      streaminfo = m_streaminfoaudio;
    }
    Name streaminfoSuffix("");
    m_producer->produce(streaminfoSuffix, (uint8_t *)streaminfo.c_str(), streaminfo.size()+1); 
    std::cout << "produce " << type << " streaminfo AGAIN" << streaminfo << std::endl;
    std::cout << "streaminfo size "<< streaminfo.size() + 1 << std::endl;
  }

  /* When the request can't be satisfied from the content store */
  void
  ProducerCallback::processInterest(const Interest& interest)
  {
    //if (interet.getName().get(-2).toSegment() < m_crrnFrameNumer)
    interest_nohit ++; 
    int sampleNumber =  std::stoi(interest.getName().get(-2).toUri());
//    std::cout << "Current Number" << std::dec << *m_curnum << std::endl;
    if (sampleNumber > *m_curnum)
    {
      std::cout << "My NACK!!!!!!" << std::endl;
      ApplicationNack appNack(interest, ApplicationNack::PRODUCER_DELAY);
      appNack.setDelay(5000); // in ms
      m_producer->nack(appNack);
    }
//    std::cout << "NO HIT Interest!" << interest.getName().toUri() << std::endl;
//    std::cout << "HAHA " <<std::dec<< sampleNumber << std::endl;
  }
  
  void
  ProducerCallback::processIncomingInterest(const Interest& interest)
  {
    interest_incoming ++;
//    std::cout << "processIncomingInterest " << interest.getName() << std::endl;
//    std::string type = interest.getName().get(-2).toUri();
//    std::string streaminfo;
//    std::cout << "processIncomingInterest type " << type << " streaminfo" << streaminfo << std::endl;
//    std::cout << "streaminfo size "<< streaminfo.size() + 1 << std::endl;

  }
  void
  ProducerCallback::processOutgoingData(const Data& data)
  {
    interest_outgoing ++;
  //  std::cout << "OutgoingData " << data.getName() << std::endl;
  //    std::cout << data.getFinalBlockId() << std::endl;
  }
 
  bool
  ProducerCallback::verifyInterest(Interest& interest){return true;}
  
  void
  ProducerCallback::processConstInterest(const Interest& interest){}

} // namespace ndn
