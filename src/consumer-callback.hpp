/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Regents of the University of California.
 *
 * @author Lijing Wang <phdloli@ucla.edu>
 */


#ifndef CONSUMER_CALLBACK_HPP
#define CONSUMER_CALLBACK_HPP
#include <ndn-cxx/contexts/consumer-context.hpp>
#include "video-player.hpp"

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions

  class ConsumerCallback
  {
      
  public:
    ConsumerCallback();
    
    void
    processPayload(const uint8_t* buffer, size_t bufferSize);
    

    void
    processStreaminfo(const uint8_t* buffer, size_t bufferSize);
    
    void
    processData(Data& data);
    
    bool
    verifyData(Data& data);
    
    void
    processConstData(const Data& data);
    
    void
    processLeavingInterest(Interest& interest);
    
    void
    onRetx(const Interest& interest)
    {
      std::cout << "Retransmitted " << interest.getName() << std::endl;
    }

    VideoPlayer player;
  };

} // namespace ndn
#endif
