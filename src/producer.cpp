/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014 Regents of the University of California.
 *
 * @author Lijing Wang <phdloli@ucla.edu>
 */

#include "video-generator.hpp" 
//#include <iostream>
// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions

  time_t time_start;
  VideoGenerator::Producer_Need pro_video;
  VideoGenerator::Producer_Need pro_audio;

  static void sig_int(int num)
  {
    time_t time_end = std::time(0);
    double seconds = difftime(time_end, time_start);
    std::cout << "  " << std::endl;
    std::cout << "｡:.ﾟヽ(*´∀`)ﾉﾟ. Test Result ヽ(✿ﾟ▽ﾟ)ノ  " << std::endl;
    std::cout << "  " << std::endl;
    std::cout << "-------- Throughput --------  " << std::endl;
    std::cout << "Video Bytes: " << pro_video.throughput << "  Payload_Throughput: " << pro_video.throughput/seconds << " Bytes/Seconds" <<std::endl;
    std::cout << "Audio Bytes: " << pro_audio.throughput << "  Payload_Throughput: " << pro_audio.throughput/seconds << " Bytes/Seconds" <<std::endl;
    std::cout << "Total Bytes: " << pro_video.throughput + pro_audio.throughput << "  Payload_Throughput: " << (pro_video.throughput + pro_audio.throughput)/seconds << " Bytes/Seconds" << std::endl;

    unsigned long interest_in = pro_video.streaminfoCB.interest_incoming + pro_video.sampleCB.interest_incoming + pro_audio.streaminfoCB.interest_incoming + pro_audio.sampleCB.interest_incoming;
    unsigned long interest_out = pro_video.streaminfoCB.interest_outgoing + pro_video.sampleCB.interest_outgoing + pro_audio.streaminfoCB.interest_outgoing + pro_audio.sampleCB.interest_outgoing;
    unsigned long interest_noh = pro_video.streaminfoCB.interest_nohit + pro_video.sampleCB.interest_nohit + pro_audio.streaminfoCB.interest_nohit + pro_audio.sampleCB.interest_nohit;

    std::cout << "  " << std::endl;
    std::cout << "-------- Interest --------  " << std::endl;
    std::cout << "Interest_Incoming: " << interest_in << "  Interest_Incoming_Speed: " << interest_in/seconds << "/Seconds" << std::endl; 
    std::cout << "Interest_Outgoing: " << interest_out << "  Interest_Outgoing_Speed: " << interest_out/seconds << "/Seconds" << std::endl; 
    std::cout << "Interest_Nohit: " << interest_noh << "  Interest_Nohit_Speed: " << interest_noh/seconds << "/Seconds" << std::endl; 
    std::cout << "  " << std::endl;
    std::cout << "---------- OVER ----------  " << seconds <<" seconds" << std::endl;
   
    return;
  }

  int
  main(int argc, char** argv)
  {
    try {
      
//  		std::string filename = "/Users/Lijing/Movies/";
//
//  		if (argc >= 2)
//  			filename += argv[1];
//  		else
//  			filename += "duoyan.mp4";
//  
     
      /* streaminfoSampleProducer */
//      Producer* audioinfoProducer = new Producer(videoName);
//      cb_producer.setProducer(audioinfoProducer); // needed for some callback functionality
//      audioinfoProducer->setContextOption(DATA_LEAVE_CNTX,
//          (ConstDataCallback)bind(&ProducerCallback::processOutgoingData, &cb_producer, _1));
//      audioinfoProducer->setup();
//
//      
//      Producer* sampleProducer = new Producer(videoName2);
//      cb_producer.setProducer(sampleProducer); // needed for some callback functionality
//      sampleProducer->setContextOption(DATA_TO_SECURE,
//                      (DataCallback)bind(&Signer::onPacket, &signer, _1));
//      sampleProducer->setContextOption(SND_BUF_SIZE,100000);
//      sampleProducer->setContextOption(DATA_LEAVE_CNTX,
//          (ConstDataCallback)bind(&ProducerCallback::processOutgoingData, &cb_producer, _1));
//      sampleProducer->setContextOption(INTEREST_ENTER_CNTX,
//                        (ConstInterestCallback)bind(&ProducerCallback::processIncomingInterest, &cb_producer, _1));
//      sampleProducer->setup();

//      generator.h264_generate_whole(filename);
      VideoGenerator generator;
      std::string filename = "/ndn/edu/ucla/capture";
      
      signal(SIGINT, sig_int);
      time_start = std::time(0);
      generator.h264_generate_capture(filename, &pro_video, &pro_audio);

      std::cout << "ε٩(๑> ₃ <)۶з" << std::endl;

//      generator.h264_generate_frames(filename, frameProducer);
//      generator.playbin_generate_frames(filename, frameProducer);
//      There is no need for callback now 
//      ProducerCallback cb_producer;
//      
//      buffer = generator.generateVideoOnce(filename, size);
//
//      Name wholeSuffix;
//      frameProducer->produce(wholeSuffix, (uint8_t*)buffer, size);

//      sleep(30000); // because setup() is non-blocking
      
//      std::cout << "HERE!!" << std::endl;

    }
    catch(std::exception& e) {
      std::cout << "HAHA" << std::endl;
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
