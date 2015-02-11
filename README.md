NEXT-NDNVIDEO
----

NDN Streaming Video Project based on Consumer/Producer API

For license information see LICENSE.

http://named-data.net/

Description
----

NDN-based Streaming Video Project using Consumer/Producer API. 

The producer keeps capturing the video from the camera and the audio from the microphone then publishing/producing them frame by frame. 
The consumer consumes the video and audio frame by frame, then play them back together.

Structure
----

The directory structure is as follows:

* **/root**
    * **src/** *-- source code*
    * **waf** *-- binary waf file*
    * **wscript** *-- waf instruction file*
    * **.waf-tools/** *-- additional waf tools*
    * **examples/** *-- no use now*
    * **tests/** *-- no use now*
    * **LICENSE**
    * **README.md**
    * **INSTALL.md**

Building instructions
----
Please, see the [INSTALL.md](INSTALL.md) for build instructions.

Usage
----
Terminal 1 -- Publishing video & audio
$ .build/producer
Terminal 2 -- Playing video & audio
$ .build/consumer

License
---
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version, with the additional exemption that compiling, linking, and/or using OpenSSL is allowed.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
