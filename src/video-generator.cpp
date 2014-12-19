/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014 Regents of the University of California.
 *
 * @author Lijing Wang <phdloli@ucla.edu>
 */

#include "video-generator.hpp"


// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions
  VideoGenerator::VideoGenerator()
  {
  }

  static gboolean
  bus_call (GstBus * bus, GstMessage *msg, GstElement *pipeline)
  {
//    g_print ("bus_call");
    switch (GST_MESSAGE_TYPE (msg)) {
      case GST_MESSAGE_BUFFERING: {
        gint percent = 0;
        gst_message_parse_buffering (msg, &percent);
        g_print ("Buffering (%3d%%)\r", percent);
        /* Wait until buffering is complete before start/resume playing */
        if (percent < 100)
          gst_element_set_state (pipeline, GST_STATE_PAUSED);
        else
          gst_element_set_state (pipeline, GST_STATE_PLAYING);
        break;
      }
      case GST_MESSAGE_EOS:{
        g_print ("End-of-stream\n");
//        g_main_loop_quit (app->loop);
        break;
      }
      case GST_MESSAGE_ERROR:{
        gchar *debug;
        GError *err;
        gst_message_parse_error (msg, &err, &debug);
        g_printerr ("Debugging info: %s\n", (debug) ? debug : "none");
        g_free (debug);
        g_print ("Error: %s\n", err->message);
        g_error_free (err);
//        g_main_loop_quit (app->loop);
        break;
      }
      case GST_MESSAGE_CLOCK_LOST:{
      /* Get a new clock */
        gst_element_set_state (pipeline, GST_STATE_PAUSED);
        gst_element_set_state (pipeline, GST_STATE_PLAYING);
        break;
      }
      default:
        break;
    }
  return TRUE;
  }

  char *
  VideoGenerator::generateVideoOnce(std::string filename, long &size)
  {
  	std::ifstream filestr;
    char *buffer;
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
//  	std::cout << "content = " << buffer << std::endl;
  
  	filestr.close();
    return buffer;
  }
  /*
   * Videogenrator will call this function to setup the stream/file info
   */
  char *
  VideoGenerator::playbin_file_info (std::string filename)
  {
    GstElement *playbin;
   // GstElement *fakevideosink, *fakeaudiosink;
    gchar *uri;
    GstState state = GST_STATE_NULL;
    GstPad *pad;
    GstCaps *caps;
    char * streaminfo;

    gst_init (NULL, NULL);

    playbin = gst_element_factory_make ("playbin", NULL);

    /* take the commandline argument and ensure that it is a uri */
    uri = gst_filename_to_uri (filename.c_str(), NULL);

//    fakevideosink = gst_element_factory_make ("fakesink", NULL);
//    fakeaudiosink = gst_element_factory_make ("fakesink", NULL);
//    g_object_set (playbin, "video-sink", fakevideosink, NULL);
//    g_object_set (playbin, "audio-sink", fakeaudiosink, NULL);

    g_object_set (playbin, "uri", uri, NULL);
    g_free (uri);

    gst_element_set_state (playbin, GST_STATE_PAUSED);
    gst_element_get_state (playbin, &state, NULL, GST_SECOND * 1);
    g_signal_emit_by_name (playbin, "get-video-pad", 0, &pad, NULL);

    caps =  gst_pad_get_current_caps (pad);
//    read_video_props(caps);
    streaminfo = gst_caps_to_string(caps);

    /* cleanup */
    gst_element_set_state (playbin, GST_STATE_NULL);
    g_object_unref (playbin);
    return streaminfo; 
  }

  void 
  VideoGenerator::h264_file_info (std::string filename)
  {
    GstElement *pipeline, *source, *demuxer; 
    GstElement_Duo queue, parser, sink;
    GstCaps_Duo caps;
    GstSample_Duo sample;
    std::string streaminfo[2];

    /* Initialisation */ 
    gst_init (NULL, NULL); 
    /* Create gstreamer elements */ 
    pipeline = gst_pipeline_new ("mp4-player"); 
    source = gst_element_factory_make ("filesrc", "file-source"); 
    demuxer = gst_element_factory_make ("qtdemux", "demuxer"); 
    queue.video = gst_element_factory_make ("queue", "video_queue"); 
    queue.audio = gst_element_factory_make ("queue", "audio_queue"); 
    parser.video = gst_element_factory_make ("h264parse", "video_parser"); 
    parser.audio = gst_element_factory_make ("aacparse", "audio_parser"); 
    sink.video = gst_element_factory_make ("appsink", "video_sink"); 
    sink.audio = gst_element_factory_make ("appsink", "audio_sink"); 
    if (!pipeline || !source || !demuxer || !queue.video || !queue.audio  || !parser.video || !parser.audio || !sink.audio || !sink.video) { 
      g_printerr ("One element could not be created. Exiting.\n"); 
    } 

    g_object_set (G_OBJECT (sink.video), "sync", FALSE, NULL); 
    g_object_set (G_OBJECT (sink.audio), "sync", FALSE, NULL); 

    /* Set up the pipeline */ 
    /* we set the input filename to the source element */ 
    g_object_set (G_OBJECT (source), "location", filename.c_str() , NULL); 
    /* we add all elements into the pipeline */ 
    gst_bin_add_many (GST_BIN (pipeline), source, demuxer, queue.audio, queue.video, parser.video, sink.video, parser.audio, sink.audio, NULL); 
  /* we link the elements together */ 
    gst_element_link (source, demuxer); 
    gst_element_link_many (queue.audio, parser.audio, sink.audio, NULL); 
    gst_element_link_many (queue.video, parser.video, sink.video, NULL); 
//  g_signal_connect (demuxer, "pad-added", G_CALLBACK (on_pad_added_queue), queue); 
    g_signal_connect (demuxer, "pad-added", G_CALLBACK (on_pad_added), &queue); 
    /* Set the pipeline to "playing" state*/ 

    gst_element_set_state (pipeline, GST_STATE_PLAYING); 
    
    g_signal_emit_by_name (sink.video, "pull-sample", &sample.video);
    caps.video = gst_sample_get_caps(sample.video);
    streaminfo[0] = gst_caps_to_string(caps.video);
    std::cout << streaminfo[0] << std::endl;

    g_signal_emit_by_name (sink.audio, "pull-sample", &sample.audio);
    caps.audio = gst_sample_get_caps(sample.audio);
    streaminfo[1] = gst_caps_to_string(caps.audio);
    std::cout << streaminfo[1] << std::endl;

    gst_element_set_state (pipeline, GST_STATE_NULL); 

    return;
  }

  /* get video and audio from Camera */
  void 
  VideoGenerator::h264_generate_capture (std::string filename)
  {
    GstElement *pipeline, *convert, *muxer, *demuxer, *mqueue, *filesink; 
    GstElement_Duo source, queue, queue1, encoder, parser, sink, queue2, decoder;
    GMainLoop *loop;
    GstBus *bus;
  
    /* Initialisation */ 
    gst_init (NULL, NULL); 
    loop = g_main_loop_new (NULL, FALSE);

    /* Create gstreamer elements */ 
    pipeline = gst_pipeline_new ("capture-player"); 

    source.video = gst_element_factory_make ("autovideosrc", "camera_source"); 
    source.audio = gst_element_factory_make ("autoaudiosrc", "MIC_source"); 

    convert = gst_element_factory_make ("audioconvert", "audio_convert"); 
    muxer = gst_element_factory_make ("mp4mux", "muxer"); 
    demuxer = gst_element_factory_make ("qtdemux", "demuxer"); 
    mqueue = gst_element_factory_make("multiqueue", "multi_queue");

    encoder.video = gst_element_factory_make("x264enc", "video_encoder");
    encoder.audio =  gst_element_factory_make("voaacenc", "audio_encoder");

    queue.video = gst_element_factory_make ("queue", "video_queue"); 
    queue.audio = gst_element_factory_make ("queue", "audio_queue"); 

    queue1.video = gst_element_factory_make ("queue", "video_queue1"); 
    queue1.audio = gst_element_factory_make ("queue", "audio_queue1"); 

    parser.video = gst_element_factory_make ("h264parse", "video_parser"); 
    parser.audio = gst_element_factory_make ("aacparse", "audio_parser"); 

    queue2.video = gst_element_factory_make ("queue", "video_queue2"); 
    queue2.audio = gst_element_factory_make ("queue", "audio_queue2"); 

    decoder.video = gst_element_factory_make ("avdec_h264", "video_decoder");
    decoder.audio = gst_element_factory_make ("faad", "audio_decoder"); 

//    sink.video = gst_element_factory_make ("appsink", "video_sink"); 
//    sink.audio = gst_element_factory_make ("appsink", "audio_sink"); 

    sink.video = gst_element_factory_make ("autovideosink", "video_sink"); 
    sink.audio = gst_element_factory_make ("autoaudiosink", "audio_sink"); 

    filesink = gst_element_factory_make("filesink", "filesink");

    if (!pipeline || !source.video || !source.audio || !convert || !encoder.video || !encoder.audio 
        || !queue1.video || !queue1.audio  || !parser.video || !parser.audio || !sink.audio || !sink.video) { 
      g_printerr ("One element could not be created. Exiting.\n"); 
    } 

//    g_object_set (G_OBJECT (source.video), "do-timestamp", 1, NULL);
//    g_object_set (G_OBJECT (source.audio), "do-timestamp", 1, NULL);

//    g_object_set (G_OBJECT (source.video), "sync", TRUE, NULL); 
//    g_object_set (G_OBJECT (source.audio), "sync", TRUE, NULL); 

//    g_object_set (G_OBJECT (sink.video), "sync", FALSE, NULL); 
//    g_object_set (G_OBJECT (sink.audio), "sync", FALSE, NULL); 

    /* Set up the pipeline */ 
    /* we set the input filename to the source element */ 
    /* we add all elements into the pipeline */ 
    gst_bin_add_many (GST_BIN (pipeline),source.video, queue1.video, encoder.video, queue2.video,
      source.audio, queue1.audio, convert, encoder.audio, queue2.audio,
      parser.video, decoder.video, sink.video,
      parser.audio, decoder.audio, sink.audio, NULL); 

    gst_element_link_many (source.video, queue1.video, encoder.video, queue2.video, parser.video, decoder.video, sink.video, NULL); 
    gst_element_link_many (source.audio, queue1.audio, convert, encoder.audio, queue2.audio, parser.audio, decoder.audio, sink.audio, NULL); 

//    gst_element_link (muxer,filesink); 

//    g_assert(gst_pad_link(gst_element_get_static_pad(queue2.video, "src"),gst_element_get_request_pad(muxer,"video_0")) == GST_PAD_LINK_OK); 
//    g_assert(gst_pad_link(gst_element_get_static_pad(queue2.audio, "src"),gst_element_get_request_pad(muxer,"audio_0")) == GST_PAD_LINK_OK); 
//    g_assert(gst_pad_link(gst_element_get_static_pad(mqueue,"src_0"),gst_element_get_static_pad(parser.video, "sink")) == GST_PAD_LINK_OK); 
//    g_assert(gst_pad_link(gst_element_get_static_pad(mqueue,"src_1"),gst_element_get_static_pad(parser.audio, "sink")) == GST_PAD_LINK_OK); 
//    gst_element_link_many (queue.video, parser.video, decoder.video, sink.video, NULL); 
//    gst_element_link_many (queue.audio, parser.audio, decoder.audio, sink.audio, NULL); 
//    g_signal_connect (demuxer, "pad-added", G_CALLBACK (on_pad_added), &queue);

//    g_object_set (G_OBJECT (filesink), "location", "/Users/lijing/next-ndnvideo/hehe2.mp4" , NULL); 
    /* Set the pipeline to "playing" state*/ 
    bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    gst_bus_add_watch (bus, (GstBusFunc)bus_call, pipeline);
    gst_object_unref (bus);

    gst_element_set_state (pipeline, GST_STATE_PLAYING); 
    g_main_loop_run (loop);

    std::cout << "audio thread start!" << std::endl;
    Producer_Need pro_audio;
    pthread_t thread_audio; 
    int rc_audio;
    pro_audio.filename = filename;
    pro_audio.sink = sink.audio;
    pro_audio.name = "audio";
//    rc_audio = pthread_create(&thread_audio, NULL, produce_thread , (void *)&pro_audio);


    std::cout << "video thread start!" << std::endl;
    Producer_Need pro_video;
    pthread_t thread_video; 
    int rc_video;
    pro_video.filename = filename;
    pro_video.sink = sink.video;
    pro_video.name = "video";
//    rc_video = pthread_create(&thread_video, NULL, produce_thread , (void *)&pro_video);
 
//    sleep(2);
//    sleep(5);
    gst_element_set_state (pipeline, GST_STATE_NULL); 
    gst_object_unref (GST_OBJECT (pipeline));
    g_main_loop_unref (loop);

    g_print ("Deleting pipeline\n"); 
    return;
  }

/* Generate frames and samples from MP4 File */
  void 
  VideoGenerator::h264_generate_whole (std::string filename)
  {
    GstElement *pipeline, *source, *demuxer; 
    GstElement_Duo queue, parser, sink;
//    GstCaps *caps;
//    GstSample *sample;
//    std::string streaminfo;
//    GstBuffer *buffer;
//    GstMapInfo map;
  
    /* Initialisation */ 
    gst_init (NULL, NULL); 
    /* Create gstreamer elements */ 
    pipeline = gst_pipeline_new ("mp4-player"); 
    source = gst_element_factory_make ("filesrc", "file-source"); 
    demuxer = gst_element_factory_make ("qtdemux", "demuxer"); 
    queue.video = gst_element_factory_make ("queue", "video_queue"); 
    queue.audio = gst_element_factory_make ("queue", "audio_queue"); 
    parser.video = gst_element_factory_make ("h264parse", "video_parser"); 
    parser.audio = gst_element_factory_make ("aacparse", "audio_parser"); 
    sink.video = gst_element_factory_make ("appsink", "video_sink"); 
    sink.audio = gst_element_factory_make ("appsink", "audio_sink"); 
    if (!pipeline || !source || !demuxer || !queue.video || !queue.audio  || !parser.video || !parser.audio || !sink.audio || !sink.video) { 
      g_printerr ("One element could not be created. Exiting.\n"); 
    } 

    g_object_set (G_OBJECT (sink.video), "sync", FALSE, NULL); 
    g_object_set (G_OBJECT (sink.audio), "sync", FALSE, NULL); 

    /* Set up the pipeline */ 
    /* we set the input filename to the source element */ 
    g_object_set (G_OBJECT (source), "location", filename.c_str() , NULL); 
    /* we add all elements into the pipeline */ 
    gst_bin_add_many (GST_BIN (pipeline), source, demuxer, queue.audio, queue.video, parser.video, sink.video, parser.audio, sink.audio, NULL); 
    /* we link the elements together */ 
    gst_element_link (source, demuxer); 
    gst_element_link_many (queue.audio, parser.audio, sink.audio, NULL); 
    gst_element_link_many (queue.video, parser.video, sink.video, NULL); 
  //  g_signal_connect (demuxer, "pad-added", G_CALLBACK (on_pad_added_queue), queue); 
    g_signal_connect (demuxer, "pad-added", G_CALLBACK (on_pad_added), &queue); 
    /* Set the pipeline to "playing" state*/ 

    gst_element_set_state (pipeline, GST_STATE_PLAYING); 
    

    std::cout << "video thread start!" << std::endl;
    Producer_Need pro_video;
    pthread_t thread_video; 
    int rc_video;
    pro_video.filename = filename;
    pro_video.sink = sink.video;
    pro_video.name = "video";
    rc_video = pthread_create(&thread_video, NULL, produce_thread , (void *)&pro_video);
 
//    sleep(2);

    std::cout << "audio thread start!" << std::endl;
    Producer_Need pro_audio;
    pthread_t thread_audio; 
    int rc_audio;
    pro_audio.filename = filename;
    pro_audio.sink = sink.audio;
    pro_audio.name = "audio";
    rc_audio = pthread_create(&thread_audio, NULL, produce_thread , (void *)&pro_audio);

//    time_t time_start = std::time(0);
//    size_t samplenumber = 0;
//    
//    do {
//      g_signal_emit_by_name (sink.audio, "pull-sample", &sample);
//      if (sample == NULL){
//        g_print("Meet the EOS!\n");
//        break;
//        }
//      if ( samplenumber == 0)
//      {
//        caps = gst_sample_get_caps(sample);
//        streaminfo = gst_caps_to_string(caps);
//        Name streaminfoAudioSuffix("audio");
////        std::cout << streaminfo << std::endl;
//        audioinfoProducer->produce(streaminfoAudioSuffix, (uint8_t *)streaminfo.c_str(), streaminfo.size()+1);
//        std::cout << "produce audio streaminfo OK! " << streaminfo << std::endl;
//        std::cout << "streaminfo size "<< streaminfo.size() + 1 << std::endl;
//      }
//      buffer = gst_sample_get_buffer (sample);
//      gst_buffer_map (buffer, &map, GST_MAP_READ);
//      Name sampleSuffix("audio/" + std::to_string(samplenumber));
//      std::cout << "Sample number: "<< samplenumber <<std::endl;
//      std::cout << "Sample Size: "<< map.size * sizeof(uint8_t) <<std::endl;
//
//      sampleProducer->produce(sampleSuffix, (uint8_t *)map.data, map.size * sizeof(uint8_t));
//      samplenumber ++;
// //     if ( samplenumber > 220)
// //       break;
//      if (sample)
//        gst_sample_unref (sample);
//      }while (sample != NULL);
//
//    time_t time_end = std::time(0);
//    double seconds = difftime(time_end, time_start);
//    std::cout << seconds << " seconds have passed" << std::endl;

    sleep(30000);
//    gst_element_set_state (pipeline, GST_STATE_NULL); 
    g_print ("Deleting pipeline\n"); 
    return;
  }
  /*
   *generate frames from the video file, will block several seconds now
   */
  void 
  VideoGenerator::h264_generate_frames (std::string filename, Producer * producer)
  {
    GstElement *pipeline, *source, *demuxer, *parser, *sink; 
    GstCaps *caps;
      
    GstBuffer *buffer;
    GstSample *sample;
    GstMapInfo map;

    /* Initialisation */ 
    gst_init (NULL, NULL); 
    /* Create gstreamer elements */ 
    pipeline = gst_pipeline_new ("mp4-player"); 
    source = gst_element_factory_make ("filesrc", "file-source"); 
    demuxer = gst_element_factory_make ("qtdemux", "demuxer"); 
    parser = gst_element_factory_make ("h264parse", "parser"); 
    sink = gst_element_factory_make ("appsink", NULL);

    g_object_set (G_OBJECT (sink), "sync", FALSE, NULL); 

    if (!pipeline || !source || !demuxer || !parser || !sink) { 
      g_printerr ("One element could not be created. Exiting.\n"); 
    } 
    /* Set up the pipeline */ 
    /* we set the input filename to the source element */ 
    g_object_set (G_OBJECT (source), "location", filename.c_str() , NULL); 
    /* we add all elements into the pipeline */ 
    gst_bin_add_many (GST_BIN (pipeline), 
    source, demuxer, parser, sink, NULL); 
    /* we link the elements together */ 
    gst_element_link (source, demuxer); 
    gst_element_link_many (parser, sink, NULL); 
    g_signal_connect (demuxer, "pad-added", G_CALLBACK (on_pad_added), parser); 
    /* Set the pipeline to "playing" state*/ 
    g_print ("Now playing: %s\n", filename.c_str()); 
    gst_element_set_state (pipeline, GST_STATE_PLAYING); 
    time_t time_start = std::time(0);
    size_t framenumber = 0;
    
    do {
      g_signal_emit_by_name (sink, "pull-sample", &sample);
      if (sample == NULL){
        g_print("Meet the EOS!\n");
        break;
        }
      caps = gst_sample_get_caps(sample);
//      read_video_props(caps);
      buffer = gst_sample_get_buffer (sample);
      gst_buffer_map (buffer, &map, GST_MAP_READ);
      Name frameSuffix(std::to_string(framenumber));
      std::cout << "Frame number: "<< framenumber <<std::endl;
      std::cout << "Frame Size: "<< map.size * sizeof(uint8_t) <<std::endl;

      producer->produce(frameSuffix, (uint8_t *)map.data, map.size * sizeof(uint8_t));
      framenumber ++;
      if ( framenumber > 2500)
        break;
    
      if (sample)
        gst_sample_unref (sample);
      }while (sample != NULL);

    time_t time_end = std::time(0);
    double seconds = difftime(time_end, time_start);
    std::cout << seconds << " seconds have passed" << std::endl;
    
    /* Iterate */ 
    gst_element_set_state (pipeline, GST_STATE_NULL); 
    g_print ("Deleting pipeline\n"); 
  }

  void
  VideoGenerator::playbin_generate_frames (std::string filename, Producer * producer)
  {
    GstElement *playbin, *fakevideosink, *fakeaudiosink;
    gchar *uri;

    GstBuffer *buffer;
    GstSample *sample;
    GstMapInfo map;
    //const GstStructure * info;
    //GstSegment *segment;
    //GstCaps *caps;

    gst_init (NULL, NULL);
  
    playbin = gst_element_factory_make ("playbin", NULL);
  
    /* take the commandline argument and ensure that it is a uri */
    uri = gst_filename_to_uri (filename.c_str(), NULL);
  
    fakevideosink = gst_element_factory_make ("appsink", NULL);
    fakeaudiosink = gst_element_factory_make ("appsink", NULL);
  
    g_object_set (playbin, "uri", uri, NULL);
    g_free (uri);
  
    g_object_set (playbin, "video-sink", fakevideosink, NULL);
    g_object_set (playbin, "audio-sink", fakeaudiosink, NULL);
  
    g_object_set (G_OBJECT (fakevideosink), "sync", FALSE, NULL); 
  
    time_t time_start = std::time(0);
    gst_element_set_state (playbin, GST_STATE_PLAYING);
    int framenumber = 0;
  
    do {
      g_signal_emit_by_name (fakevideosink, "pull-sample", &sample);
//      g_print("After calling pull-sample  ");
      if (sample == NULL){
        g_print("Meet the EOS!\n");
        break;
        }
//      caps = gst_sample_get_caps(sample);
      buffer = gst_sample_get_buffer (sample);
//      info = gst_sample_get_info(sample);
//      segment = gst_sample_get_segment(sample);
      gst_buffer_map (buffer, &map, GST_MAP_READ);
      Name frameSuffix(std::to_string(framenumber));
      std::cout << "Frame number: "<<framenumber <<std::endl;
      producer->produce(frameSuffix, (uint8_t *)map.data, map.size);
      std::cout << "Frame number: "<<framenumber << "FINISHED" <<std::endl;
      framenumber ++;
  
//      std::cout << "Gstbuffer size: " << gst_buffer_get_size (buffer) << std::endl;
//      std::cout << "Map size: " << map.size << std::endl;
//      std::cout << "Segment Info: position " << segment->position <<" duration " << segment->duration << std::endl;
//      read_video_props(caps);
//      if (info !=NULL)
//        std::cout << "Sample INFO " << gst_structure_to_string (info) <<std::endl;
//    g_print ("got sample: %p", sample);
  
      if (sample)
        gst_sample_unref (sample);
//      }while (sample != NULL);
      }while (framenumber <= 24);
  
      g_print("Should be here\n");
  
  //  do {
  //    g_signal_emit_by_name (fakeaudiosink, "pull-sample", &sample);
  //    buffer = gst_sample_get_buffer (sample);
  //    gst_buffer_map (buffer, &map, GST_MAP_READ);
  //    std::cout << "Map size: " << map.size << std::endl;
  //
  //    if (sample)
  //      gst_sample_unref (sample);
  //    }while (sample != NULL);
  
    time_t time_end = std::time(0);
    double seconds = difftime(time_end, time_start);
    std::cout << seconds << " seconds have passed" << std::endl;
  
    /* cleanup */
    gst_element_set_state (playbin, GST_STATE_NULL);
    g_object_unref (playbin);
  }
} // namespace ndn
