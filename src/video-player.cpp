/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Regents of the University of California.
 *
 * @author Lijing Wang <phdloli@ucla.edu>
 */

#include "video-player.hpp"


namespace ndn {

  #define BUFFER_SIZE  1024*1024*4

  VideoPlayer::VideoPlayer()
  {
  }

  void
  VideoPlayer::get_streaminfo(std::string streaminfo)
  {
    VideoAudio *va =  &s_va;

    App *video = &(va->v_app);

    video->size = BUFFER_SIZE; 
    video->data = new guint8[video->size];
    video->capstr = streaminfo; 
    std::cout << "LALALA streaminfo " << streaminfo << std::endl;
    h264_appsrc_init();
  }

  void
  VideoPlayer::h264_appsrc_data(const uint8_t* buffer, size_t bufferSize )
  {
    VideoAudio *va = &s_va;
    App *video = &(va->v_app);

    /* get some vitals, this will be used to read data from the mmapped file and
     * feed it to appsrc. */
    OffsetRecord dataNode;
    if((video -> dataQue).size() == 0)
    {
      dataNode.offset = 0;
      dataNode.length = bufferSize;
    }else
    {
      OffsetRecord tmpNode = (video -> dataQue).back();
      dataNode.offset = tmpNode.offset + tmpNode.length;
      if(dataNode.offset + bufferSize >= video->size)
        dataNode.offset = 0;
      dataNode.length = bufferSize;
    }
    memcpy (video->data + dataNode.offset, buffer, dataNode.length);
    (video->dataQue).push_back(dataNode);

    std::cout << "CP Data Done! " << bufferSize <<std::endl;
//    std::cout << video->capstr << std::endl;
  }

/*
 * First INIT the h264_appsrc using thread
 * waiting in a loop
 */
  void
  VideoPlayer::h264_appsrc_init()
  {
    VideoAudio *va = &s_va;

    /* get some vitals, this will be used to read data from the mmapped file and
     * feed it to appsrc. */

    pthread_t thread; 
    int rc;
    rc = pthread_create(&thread, NULL, h264_appsrc_thread , (void *)va);

    std::cout << "h264_appsrc_init OK! " << std::endl;
  }
} // namespace ndn
