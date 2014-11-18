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
#include <ctime>

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions

  int times = 0;
  time_t time_start;
  time_t time_end;

  ConsumerCallback::ConsumerCallback()
  {
//    std::cout << "Construction" << std::endl;
//    player.playbin_appsrc_init();
//    std::cout << "Construction Over" << std::endl;
  }
  
  void
  ConsumerCallback::processPayload(const uint8_t* buffer, size_t bufferSize)
  {
    std::cout << "times " << times <<std::endl;
    std::cout << "bufferSize " << bufferSize <<std::endl;
    std::cout << "@buffer " << &buffer <<std::endl;

//    if(times == 0)
//      time_start = std::time(0);
//    if(times == 249)
//    {
//       time_end = std::time(0);
//       double seconds = difftime(time_end, time_start);
//       std::cout << seconds << " seconds have passed" << std::endl;
//    }
    player.h264_appsrc_data(buffer, bufferSize);
    times ++;
//    std::cout << "over " << std::endl;
  }
  
  void
  ConsumerCallback::processStreaminfo(const uint8_t* buffer, size_t bufferSize)
  {
    std::string streaminfo((char*) buffer);
  //  long fileLength = std::stol(content);
  //  std::cout << "bufferSize " << bufferSize <<std::endl;
  //  std::cout << "buffer " << buffer <<std::endl;
 //   std::cout << "streaminfo " << streaminfo <<std::endl;
  //  std::cout << "fileLength " << fileLength <<std::endl;
    std::cout << "processStreaminfo " << streaminfo << std::endl;
    player.get_streaminfo(streaminfo);
  }

  void
  ConsumerCallback::processData(Data& data)
  {
//    std::cout << "DATA IN CNTX" << std::endl;
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
//    std::cout << "LEAVES " << interest.toUri() << std::endl;
//    std::cout << "LEAVES name " << interest.getName() << std::endl;
  }  
} // namespace ndn
