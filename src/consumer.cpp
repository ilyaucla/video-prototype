/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014 Regents of the University of California.
 *
 * @author Lijing Wang phdloli@ucla.edu
 */

#include "consumer.hpp"

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions


  time_t time_start;
  ConsumerCallback cb_consumer;

  //when control-c detected, doing the analysis 
  static void sig_int(int num)
  {
    time_t time_end = std::time(0);
    double seconds = difftime(time_end, time_start);
    std::cout << "  " << std::endl;
    std::cout << "｡:.ﾟヽ(*´∀`)ﾉﾟ. Test Result ヽ(✿ﾟ▽ﾟ)ノ  " << std::endl;
    std::cout << "  " << std::endl;
    std::cout << "-------- Throughput --------  " << std::endl;
    std::cout << "Video Bytes: " << cb_consumer.payload_v << "  PayLoad_Throughput: " << cb_consumer.payload_v/seconds << " Bytes/Seconds" <<std::endl;
    std::cout << "Audio Bytes: " << cb_consumer.payload_a << "  PayLoad_Throughput: " << cb_consumer.payload_a/seconds << " Bytes/Seconds" <<std::endl;
    std::cout << "Total Bytes: " << cb_consumer.payload_v + cb_consumer.payload_a << "  PayLoad_Throughput: " << (cb_consumer.payload_v + cb_consumer.payload_a)/seconds << " Bytes/Seconds" << std::endl;
    std::cout << "  " << std::endl;
    std::cout << "-------- Interest --------  " << std::endl;
    std::cout << "Interest_Send: " << cb_consumer.interest_s << "  Interest_Send_Speed: " <<  cb_consumer.interest_s/seconds << " /Seconds" <<  std::endl;
    std::cout << "Interest_Receive: " << cb_consumer.interest_r << "  Interest_Receive_Speed: " <<  cb_consumer.interest_r/seconds << " /Seconds" <<  std::endl;
    std::cout << "Interest_Retransfer: " << cb_consumer.interest_retx << " Interest_Retransfer_Speed: " << cb_consumer.interest_retx/seconds << " /Seconds" << std::endl;
    std::cout << "Interest_Useful Percentage: " << double(cb_consumer.interest_r)/double(cb_consumer.interest_s)*100 <<"%" << std::endl;
    std::cout << "  " << std::endl;
    std::cout << "---------- OVER ----------  " << seconds <<" seconds" << std::endl;

    return;
  }

  int
  main(int argc, char** argv)
  {
    try {
  
  		std::string filename = "/Users/Lijing/Movies/duoyan.mp4";
      filename = "/ndn/edu/ucla/capture";

      Name videoinfoName(filename + "/video/streaminfo");
      Consumer* videoinfoConsumer = new Consumer(videoinfoName, SDR);
      videoinfoConsumer->setContextOption(MUST_BE_FRESH_S, true);
//      videoinfoConsumer->setContextOption(RIGHTMOST_CHILD_S, true);
      videoinfoConsumer->setContextOption(INTEREST_LEAVE_CNTX, 
        (InterestCallback)bind(&ConsumerCallback::processLeavingStreaminfo, &cb_consumer, _1));
      videoinfoConsumer->setContextOption(CONTENT_RETRIEVED, 
        (ContentCallback)bind(&ConsumerCallback::processStreaminfo, &cb_consumer, _1, _2));
      videoinfoConsumer->setContextOption(DATA_ENTER_CNTX, 
        (DataCallback)bind(&ConsumerCallback::streaminfoEnter,&cb_consumer , _1));

      if(argc > 1)
        videoinfoConsumer->consume(Name(argv[1]));
      else
        videoinfoConsumer->consume(Name(""));

      Name audioinfoName(filename + "/audio/streaminfo");
      Consumer* audioinfoConsumer = new Consumer(audioinfoName, SDR);
      audioinfoConsumer->setContextOption(MUST_BE_FRESH_S, true);
//      audioinfoConsumer->setContextOption(RIGHTMOST_CHILD_S, true);
      audioinfoConsumer->setContextOption(INTEREST_LEAVE_CNTX, 
        (InterestCallback)bind(&ConsumerCallback::processLeavingStreaminfo, &cb_consumer, _1));
      audioinfoConsumer->setContextOption(CONTENT_RETRIEVED, 
        (ContentCallback)bind(&ConsumerCallback::processStreaminfoAudio, &cb_consumer, _1, _2));
      audioinfoConsumer->setContextOption(DATA_ENTER_CNTX, 
        (DataCallback)bind(&ConsumerCallback::streaminfoEnterAudio, &cb_consumer, _1));

      if(argc > 1)
        audioinfoConsumer->consume(Name(argv[1]));
      else
        audioinfoConsumer->consume(Name(""));

//      sleep(1); // because consume() is non-blocking
      std::cout << "consume whole start!" <<std::endl;
/*      
      Verificator* verificator = new Verificator();
      Name videoName(filename + "/video/content");
      */

      signal(SIGINT, sig_int);

      time_t time_start_0 = std::time(0);
      std::cout << "Before consume " << time_start_0 << std::endl;

      //audio consumer
      int rc_audio;
      Consumer_Need audioData;
      audioData.filename = filename;
      audioData.name = "audio";
      audioData.cb = &cb_consumer ;
      pthread_t thread_audio; 

      time_start = std::time(0);

      rc_audio = pthread_create(&thread_audio, NULL, consume_thread , (void *)&audioData);

      //video consumer
      int rc_video;
      Consumer_Need videoData;
      videoData.filename = filename;
      videoData.name = "video";
      videoData.cb = &cb_consumer ;
      pthread_t thread_video; 
      rc_video = pthread_create(&thread_video, NULL, consume_thread , (void *)&videoData);

      time_t time_end_0  = std::time(0);
      std::cout << "After consume " << time_end_0 << std::endl;
      double seconds = difftime(time_end_0, time_start_0);
      std::cout << seconds << " seconds have passed" << std::endl;

      sleep(30000000); // because consume() is non-blocking
      
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
