/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Regents of the University of California.
 *
 * @author Lijing Wang <phdloli@ucla.edu>
 */


#ifndef CONSUMER_CALLBACK_HPP
#define CONSUMER_CALLBACK_HPP
#include <ndn-cxx/contexts/consumer-context.hpp>
#include <fstream>
#include <iostream>
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
    processPayloadAudio(const uint8_t* buffer, size_t bufferSize);
    
    void
    processStreaminfoAudio(const uint8_t* buffer, size_t bufferSize);
    
    void
    processData(Data& data);
    
    void
    streaminfoEnter(Data& data);

    void
    streaminfoEnterAudio(Data& data);

    bool
    verifyData(Data& data);
    
    void
    processConstData(const Data& data);
    
    void
    processLeavingInterest(Interest& interest);
    
    void
    processLeavingStreaminfo(Interest& interest);

    void
    onRetx(Interest& interest);

    void
    processFile(const uint8_t* buffer, size_t bufferSize)
    {
      std::ofstream filestr;
    	// binary open
      
    	filestr.open ("/Users/lijing/next-ndnvideo/build/haha.mp3",  std::ios::out | std::ios::app | std::ios::binary);
      filestr.write((char *)buffer, bufferSize);
      filestr.close();
 
    }

    VideoPlayer player;
    gsize payload_v = 0;
    gsize payload_a = 0;
    gsize interest_s = 0;
    gsize interest_r = 0;
    gsize interest_retx = 0;
    gsize data_v = 0;
    gsize data_a = 0;
    gsize start_frame_v = 0;
    gsize start_frame_a = 0;
  };

} // namespace ndn
#endif
