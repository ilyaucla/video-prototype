/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014 Regents of the University of California.
 *
 * @author Lijing Wang phdloli@ucla.edu
 */

#include "video-player.hpp"
#include "consumer-callback.hpp"
#include <string>

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions

  VideoPlayer player;

  ConsumerCallback::ConsumerCallback()
  {
  }
  
  void
  ConsumerCallback::processPayload(const uint8_t* buffer, size_t bufferSize)
  {
    std::cout << "bufferSize " << bufferSize <<std::endl;
    std::cout << "@buffer " << &buffer <<std::endl;
/*
		std::string filename = "sequence";
		std::ofstream fout(filename, std::ios::binary);
		fout.write((char*)buffer, sizeof(char)*bufferSize);
    player.play_bin_uri(filename);
 */   
    player.playbin_appsrc_init(buffer, bufferSize);
  /*  
    fout.write("", 0);
		fout.close();
    */
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
