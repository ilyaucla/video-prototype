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

  int times_video = 0;
  int times_audio = 0;
  time_t time_start;
  time_t time_end;
  gsize payload_v = 0;
  gsize payload_a = 0;
  gsize data_v = 0;
  gsize data_a = 0;

  static void sig_int(int num)
  {
    time_t time_end = std::time(0);
    double seconds = difftime(time_end, time_start);
    std::cout << "Test Result ---------  " << seconds <<" seconds:" << std::endl;
    std::cout << "Video Bytes: " << payload_v << "  PayLoad_Throughput: " << payload_v/seconds << " Bytes/Seconds" <<std::endl;
    std::cout << "Audio Bytes: " << payload_a << "  PayLoad_Throughput: " << payload_a/seconds << " Bytes/Seconds" <<std::endl;
    std::cout << "Total Bytes: " << payload_v + payload_a << "  PayLoad_Throughput: " << (payload_v + payload_a)/seconds << " Bytes/Seconds" << std::endl;
    return;
  }
 
  ConsumerCallback::ConsumerCallback()
  {
//    std::cout << "Construction" << std::endl;
//    player.playbin_appsrc_init();
//    std::cout << "Construction Over" << std::endl;
  }
  
  void
  ConsumerCallback::processPayload(const uint8_t* buffer, size_t bufferSize)
  {
    std::cout << "video times processPayload " << std::dec << times_video <<std::endl;
    std::cout << "video Size:" << std::dec << bufferSize <<std::endl;
//    std::cout << "video bufferSize " << bufferSize <<std::endl;
//    std::cout << "@buffer " << &buffer <<std::endl;
    payload_v += bufferSize;
    player.h264_appsrc_data(buffer, bufferSize);
    times_video ++;
//    std::cout << "processPayload video over " << std::endl;
  }

  void
  ConsumerCallback::processPayloadAudio(const uint8_t* buffer, size_t bufferSize)
  {
    std::cout << "audio times processPayload " << std::dec << times_audio <<std::endl;
    std::cout << "audio Size:" << std::dec << bufferSize <<std::endl;
//    std::cout << "audio bufferSize " << bufferSize <<std::endl;
//    std::cout << "@buffer " << &buffer <<std::endl;
    payload_a += bufferSize;
    player.h264_appsrc_data_audio(buffer, bufferSize);
    times_audio ++;
//    std::cout << "processPayload audio over " << std::endl;
  }
  
  void
  ConsumerCallback::processStreaminfo(const uint8_t* buffer, size_t bufferSize)
  {
    time_start = std::time(0);
    signal(SIGINT, sig_int);

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
  ConsumerCallback::processStreaminfoAudio(const uint8_t* buffer, size_t bufferSize)
  {
    std::string streaminfo((char*) buffer);
    std::cout << "processStreaminfo_audio " << streaminfo << std::endl;
    player.get_streaminfo_audio(streaminfo);
  }

  void
  ConsumerCallback::processData(Data& data)
  {
//    std::cout << "DATA IN CNTX Name: " << data.getName() << "FinalBlockId: " <<data.getFinalBlockId() << std::endl;
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
//   std::cout << "LEAVES " << interest.toUri() << std::endl;
//    std::cout << "LEAVES name " << interest.getName() << std::endl;
  }  
} // namespace ndn
