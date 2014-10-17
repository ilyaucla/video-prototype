/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
 *
 * @author Lijing Wang <phdloli@ucla.edu>
 */

// correct way to include ndn-cxx headers
#include <ndn-cxx/contexts/producer-context.hpp>
#include <fstream>
//#include "contexts/producer-context.hpp"

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
  setProducer(Producer* p)
  {
    m_producer = p;
  }
  
  void
  processConstData(const Data& data){}
  
  void
  processInterest(const Interest& interest)
  {
    std::cout << "REPLY TO " << interest.toUri() << std::endl;
  }
  
  void
  processIncomingInterest(const Interest& interest)
  {
    std::cout << "COMES IN " << interest.getName() << std::endl;
  }
  
  bool
  verifyInterest(Interest& interest){return true;}
  
  void
  processConstInterest(const Interest& interest){}

private:
  Producer* m_producer;
};

void produceVideo(Producer *producer, std::string filename)
{
	std::ifstream filestr;
	long size;
	char * buffer;
	// binary open
	filestr.open (filename, std::ios::binary);
	// get the size
	filestr.seekg(0, std::ios::end);
	size = filestr.tellg();
	filestr.seekg(0, std::ios::beg);
	// malloc the memory
	buffer=new char[size];
  
	filestr.read(buffer, size);
	std::cout << "byte = " << size << std::endl;
	std::cout << "content = " << buffer << std::endl;

	filestr.close();

  Name emptySuffix;
  producer->produce(emptySuffix, (uint8_t*)buffer, size);
}

int
main(int argc, char** argv)
{
  try {

		std::string filename;
		if (argc >= 2)
			filename = argv[1];
		else
			filename = "/Users/Loli/Video/me.ogg";

    Name videoName(filename);
    
    CallbackContainer stubs;

    Producer* sequenceProducer = new Producer(videoName);
    
    stubs.setProducer(sequenceProducer); // needed for some callback functionality
    
    //setting callbacks
    sequenceProducer->setContextOption(INTEREST_ENTER_CNTX,
                      (ConstInterestCallback)bind(&CallbackContainer::processIncomingInterest, &stubs, _1));
                      
    sequenceProducer->setContextOption(INTEREST_TO_PROCESS,
                      (ConstInterestCallback)bind(&CallbackContainer::processInterest, &stubs, _1));

    
    //listening
    sequenceProducer->setup();

	produceVideo(sequenceProducer, filename);
    
	std::cout << "HERE!" << std::endl;
    sleep(300); // because setup() is non-blocking
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
