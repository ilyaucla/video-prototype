/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Regents of the University of California.
 *
 * @author Lijing Wang <phdloli@ucla.edu>
 */

#include "video-player.hpp"

namespace ndn {

  VideoPlayer::VideoPlayer()
  {
    playbin_appsrc_init();
  }

  void
  VideoPlayer::playbin_appsrc_data(const uint8_t* buffer, size_t bufferSize )
  {
    App *app = &s_app;

    /* get some vitals, this will be used to read data from the mmapped file and
     * feed it to appsrc. */
    DataNode dataNode;
    uint8_t* bufferTmp = new uint8_t[bufferSize];
    memcpy (bufferTmp, buffer, bufferSize);
    dataNode.length = bufferSize;
    dataNode.data = (guint8 *) bufferTmp;
    (app->dataQue).push_back(dataNode);

    std::cout << "CP Data Done! " << bufferSize <<std::endl;
  }

/*
 * First INIT the playbin_appsrc using thread
 * waiting in a loop
 */
  void
  VideoPlayer::playbin_appsrc_init ()
  {
    App *app = &s_app;

    /* get some vitals, this will be used to read data from the mmapped file and
     * feed it to appsrc. */

    app->length = 0;
    app->data = NULL;
    app->offset = 0;

    pthread_t thread; 
    int rc;
    rc = pthread_create(&thread, NULL, playbin_appsrc_thread , (void *)app);

    std::cout << "playbin_appsrc_init OK! " << std::endl;
  }
} // namespace ndn
