/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
 *
 * @author Lijing Wang phdloli@ucla.edu
 */

// correct way to include ndn-cxx headers
#include <ndn-cxx/contexts/consumer-context.hpp>
#include "playvideo.hpp"
//#include "contexts/consumer-context.hpp"

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions

class CallbackContainer
{

public:
  CallbackContainer()
  {
  }
  
  void
  processPayload(const uint8_t* buffer, size_t bufferSize)
  {
    std::string content1((char*)buffer);
    std::cout << "bufferSize " << bufferSize <<std::endl;
    //std::cout << "CONTENT " << content1 << std::endl;

		std::string filename = "sequence";

		std::ofstream fout(filename, std::ios::binary);
		fout.write((char*)buffer, sizeof(char)*bufferSize);
    
    PlayVideo play;
    play.play_bin_uri(filename);
    
    fout.write("", 0);
		fout.close();
  }
  
  void
  processData(Data& data)
  {
    std::cout << "DATA IN CNTX" << std::endl;
  }
  
  bool
  verifyData(Data& data)
  {
    return true;
  }
  
  void
  processConstData(const Data& data)
  {}
  
  void
  processLeavingInterest(Interest& interest)
  {
    std::cout << "LEAVES " << interest.toUri() << std::endl;
    std::cout << "LEAVES name" << interest.getName() << std::endl;
  }  
};


int
main(int argc, char** argv)
{
  try {

		std::string filename;
		if(argc >=2 )
			filename = argv[1];
		else
			filename = "/Users/Loli/Video/me.ogg";
    Name videoName(filename);
      
    CallbackContainer stubs;

    Consumer* sequenceConsumer = new Consumer(videoName, RELIABLE, SEQUENCE);
    
    sequenceConsumer->setContextOption(INTEREST_LEAVE_CNTX, 
                              (InterestCallback)bind(&CallbackContainer::processLeavingInterest, &stubs, _1));
  
    sequenceConsumer->setContextOption(DATA_ENTER_CNTX, 
                              (DataCallback)bind(&CallbackContainer::processData, &stubs, _1));
  
    sequenceConsumer->setContextOption(CONTENT_RETRIEVED, 
                              (ContentCallback)bind(&CallbackContainer::processPayload, &stubs, _1, _2));

//		sequenceConsumer->setContextOption(SND_BUF_SIZE, 1024*1024*5);

//		sequenceConsumer->setContextOption(RCV_BUF_SIZE, 1024*1024*4);
		sequenceConsumer->setContextOption(CONTENT_CHUNK_SIZE, 1024*1024*4);
  
    sequenceConsumer->consume(Name());
    
    sleep(3000); // because consume() is non-blocking
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
