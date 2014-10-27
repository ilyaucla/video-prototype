/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014 Regents of the University of California.
 *
 * @author Lijing Wang phdloli@ucla.edu
 */

#include "video-player.hpp"
#include "consumer-callback.hpp"
#include <string>
#include <pthread.h>

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions

  int times = 0;

  ConsumerCallback::ConsumerCallback()
  {
//    std::cout << "Construction" << std::endl;
//    player.playbin_appsrc_init();
//    std::cout << "Construction Over" << std::endl;
  }
  
  void
  ConsumerCallback::processPayload(const uint8_t* buffer, size_t bufferSize)
  {
    std::cout << "bufferSize " << bufferSize <<std::endl;
    std::cout << "@buffer " << &buffer <<std::endl;
    std::cout << "times " << times <<std::endl;

    player.playbin_appsrc_data(buffer, bufferSize);

    times ++;
  }
  
  void
  ConsumerCallback::processConfig(const uint8_t* buffer, size_t bufferSize)
  {
    std::string content((char*) buffer);
    long fileLength = std::stol(content);
  //  std::cout << "bufferSize " << bufferSize <<std::endl;
  //  std::cout << "buffer " << buffer <<std::endl;
  //  std::cout << "content " << content <<std::endl;
    std::cout << "fileLength " << fileLength <<std::endl;
    //player.playbin_appsrc_init(fileLength);
  }

  void
  ConsumerCallback::processData(Data& data)
  {
    std::cout << "DATA IN CNTX" << std::endl;
  }
  
  bool
  ConsumerCallback::verifyData(Data& data)
  {
    return true;
  }
  
  void
  ConsumerCallback::processConstData(const Data& data)
  {}
  
  void
  ConsumerCallback::processLeavingInterest(Interest& interest)
  {
    std::cout << "LEAVES " << interest.toUri() << std::endl;
    std::cout << "LEAVES name" << interest.getName() << std::endl;
  }  
} // namespace ndn
