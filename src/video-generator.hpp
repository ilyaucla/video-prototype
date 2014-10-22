/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014 Regents of the University of California.
 *
 * @author Lijing Wang <phdloli@ucla.edu>
 */

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
#ifndef VIDEO_GENERATOR_HPP
#define VIDEO_GENERATOR_HPP

#include <string>

namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions

  class VideoGenerator
  {
    public:

      VideoGenerator();

      char * 
      generateVideoOnce(std::string filename, long &size);
  };

} // namespace ndn
#endif
