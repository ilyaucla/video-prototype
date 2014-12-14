/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014 Regents of the University of California.
 *
 * @author Lijing Wang <phdloli@ucla.edu>
 */

#include "producer-callback.hpp"
#include <fstream>
#include <iostream>
#include <ctime>
#include <ndn-cxx/contexts/producer-context.hpp>
// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions
  
  int
  main(int argc, char** argv)
  {
    try {
//  
  		std::string filename = "";
//      std::string streaminfo;
      long size = 0;
  		filename = "/Users/Lijing/Movies/c.mp3";
     	std::ifstream filestr;
      char *buffer;
    	// binary open
    	filestr.open (filename, std::ios::binary);
    	// get the size
    	filestr.seekg(0, std::ios::end);
    	size = filestr.tellg();
    	filestr.seekg(0, std::ios::beg);
    	// malloc the memory
    	
      int chunk_size = 3000;
      int chunk_num = size/chunk_size; 
      std::cout <<"chunk_num "<< chunk_num << std::endl;
    	std::cout << "size = " << size << std::endl;
  //  	std::cout << "content = " << buffer << std::endl;
    

//
      Name videoName2("whole");
      ProducerCallback cb_producer;
      Producer* frameProducer = new Producer(videoName2);
//
      frameProducer->setContextOption(SND_BUF_SIZE,10000000);
      frameProducer->setContextOption(DATA_LEAVE_CNTX,
          (ConstDataCallback)bind(&ProducerCallback::processOutgoingData, &cb_producer, _1));
      frameProducer->setContextOption(INTEREST_ENTER_CNTX,
                        (ConstInterestCallback)bind(&ProducerCallback::processIncomingInterest, &cb_producer, _1));

      frameProducer->setup();
      int len;

      for(int i=0; i<=chunk_num; i++)
      {
        std::cout << "Chunk number: "<< i << std::endl;
        filestr.seekg(i*chunk_size);
        if(i<chunk_num)
        {
          len = chunk_size;
        }
        else
        {
          len = size - i*chunk_size;
        }
        std::cout << "Chunk size: "<< len << std::endl;
        filestr.seekg(i*chunk_size);
        buffer=new char[len]; 
      	filestr.read(buffer, len);
        frameProducer->produce(Name(std::to_string(i)),(uint8_t*)buffer,len);
      }
        
    	filestr.close();

      std::cout << "HERE!" << std::endl;
      sleep(30000); // because setup() is non-blocking
//
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
