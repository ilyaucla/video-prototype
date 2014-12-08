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
  ProducerCallback::processConstData(const Data& data){}
  
  void
  ProducerCallback::processInterest(const Interest& interest)
  {
    std::cout << "processInterest " << interest.toUri() << std::endl;
  }
  
  void
  ProducerCallback::processIncomingInterest(const Interest& interest)
  {
    std::cout << "processIncomingInterest " << interest.getName() << std::endl;
  }
  
  bool
  ProducerCallback::verifyInterest(Interest& interest){return true;}
  
  void
  ProducerCallback::processConstInterest(const Interest& interest){}

} // namespace ndn
