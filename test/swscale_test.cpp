/*
 * swscale_test.cpp :
 * 
 * Copyright 2017, 2018 Valkka Security Ltd. and Sampsa Riikonen.
 * 
 * Authors: Sampsa Riikonen <sampsa.riikonen@iki.fi>
 * 
 * This file is part of the Valkka library.
 * 
 * Valkka is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>
 *
 */

/** 
 *  @file    swscale_test.cpp
 *  @author  Sampsa Riikonen
 *  @date    2017
 *  @version 0.1
 *  
 *  @brief 
 *
 */ 

#include "queues.h"
#include "livethread.h"
#include "avthread.h"
#include "logging.h"
#include "filters.h"
#include "sharedmem.h"

using namespace std::chrono_literals;
using std::this_thread::sleep_for;

const char* stream_1   =std::getenv("VALKKA_TEST_RTSP_1");
const char* stream_2   =std::getenv("VALKKA_TEST_RTSP_2");
const char* stream_sdp =std::getenv("VALKKA_TEST_SDP");


void test_1() {
  const char* name = "@TEST: swscale_test: test 1:";
  std::cout << name <<"** @@Test SwScaleFilter from RTSP cam **" << std::endl;
  
  if (!stream_1) {
    std::cout << name <<"ERROR: missing test stream 1: set environment variable VALKKA_TEST_RTSP_1"<< std::endl;
    exit(2);
  }
  std::cout << name <<"** test rtsp stream 1: "<< stream_1 << std::endl;

  
  // *************
  // filtergraph:
  // (LiveThread:livethread) --> {FrameFilter:info} --> {FifoFrameFilter:terminal} --> [FrameFifo:in_fifo] -->> (AVThread:avthread) 
  // {TimeIntervarFrameFilter:interval_filter} --> {SwScaleFilter:sw_scale} --> {InfoFrameFilter::out_filter}
  
  
  InfoFrameFilter     out_filter  ("out");
  SwScaleFrameFilter  sw_scale    ("cpu_interpolate",600,300,&out_filter);
  
  TimeIntervalFrameFilter interval_filter("interval",1000,&sw_scale); // pass a frame each 1000 milliseconds to sw_scale
  
  FrameFifo       in_fifo     ("in_fifo",10);                
  AVThread        avthread    ("avthread",in_fifo,interval_filter);
  
  FifoFrameFilter terminal    ("terminal",in_fifo); 
  InfoFrameFilter info        ("info",&terminal);       
  LiveThread      livethread  ("livethread"); 
  // *************
  
  LiveConnectionContext ctx;
  bool verbose;
  
  std::cout << name << "starting threads" << std::endl;
  livethread.startCall();
  avthread.  startCall();

  avthread.decodingOnCall();
  
  sleep_for(2s);
  
  std::cout << name << "registering stream" << std::endl;
  ctx = (LiveConnectionContext){LiveConnectionType::rtsp, std::string(stream_1), 2, &info}; // Request livethread to write into filter info
  livethread.registerStreamCall(ctx);
  
  sleep_for(1s);
  
  std::cout << name << "playing stream !" << std::endl;
  livethread.playStreamCall(ctx);
  
  sleep_for(10s);
  
  std::cout << name << "stopping threads" << std::endl;
  livethread.stopCall();
  avthread.  stopCall();
  
  
}


void test_2() {
  const char* name = "@TEST: swscale_test: test 2:";
  std::cout << name <<"** @@Test SwScaleFilter from RTSP cam and write to shared memory**" << std::endl;
  
  if (!stream_1) {
    std::cout << name <<"ERROR: missing test stream 1: set environment variable VALKKA_TEST_RTSP_1"<< std::endl;
    exit(2);
  }
  std::cout << name <<"** test rtsp stream 1: "<< stream_1 << std::endl;

  
  // *************
  // filtergraph:
  // (LiveThread:livethread) --> {FrameFilter:info} --> {FifoFrameFilter:terminal} --> [FrameFifo:in_fifo] -->> (AVThread:avthread) 
  // {TimeIntervarFrameFilter:interval_filter} --> {SwScaleFilter:sw_scale} --> {InfoFrameFilter::out_filter}
  
  SharedMemFrameFilter shmem      ("testing", 10, 600*300*3);
  
  SwScaleFrameFilter  sw_scale    ("cpu_interpolate",600,300,&shmem);
  
  TimeIntervalFrameFilter interval_filter("interval",1000,&sw_scale); // pass a frame each 1000 milliseconds to sw_scale
  
  FrameFifo       in_fifo     ("in_fifo",10);                
  AVThread        avthread    ("avthread",in_fifo,interval_filter);
  
  FifoFrameFilter terminal    ("terminal",in_fifo); 
  InfoFrameFilter info        ("info",&terminal);       
  LiveThread      livethread  ("livethread"); 
  // *************
  
  LiveConnectionContext ctx;
  bool verbose;
  
  std::cout << name << "starting threads" << std::endl;
  livethread.startCall();
  avthread.  startCall();

  avthread.decodingOnCall();
  
  sleep_for(2s);
  
  std::cout << name << "registering stream" << std::endl;
  ctx = (LiveConnectionContext){LiveConnectionType::rtsp, std::string(stream_1), 2, &info}; // Request livethread to write into filter info
  livethread.registerStreamCall(ctx);
  
  sleep_for(1s);
  
  std::cout << name << "playing stream !" << std::endl;
  livethread.playStreamCall(ctx);
  
  sleep_for(5s);
  
  std::cout << name << "stopping threads" << std::endl;
  livethread.stopCall();
  avthread.  stopCall();
  
  
  
}


void test_3() {
  
  const char* name = "@TEST: swscale_test: test 3: ";
  std::cout << name <<"** @@DESCRIPTION **" << std::endl;
  
}


void test_4() {
  
  const char* name = "@TEST: swscale_test: test 4: ";
  std::cout << name <<"** @@DESCRIPTION **" << std::endl;
  
}


void test_5() {
  
  const char* name = "@TEST: swscale_test: test 5: ";
  std::cout << name <<"** @@DESCRIPTION **" << std::endl;
  
}



int main(int argc, char** argcv) {
  if (argc<2) {
    std::cout << argcv[0] << " needs an integer argument.  Second interger argument (optional) is verbosity" << std::endl;
  }
  else {
    
    if  (argc>2) { // choose verbosity
      switch (atoi(argcv[2])) {
        case(0): // shut up
          ffmpeg_av_log_set_level(0);
          fatal_log_all();
          break;
        case(1): // normal
          break;
        case(2): // more verbose
          ffmpeg_av_log_set_level(100);
          debug_log_all();
          break;
        case(3): // extremely verbose
          ffmpeg_av_log_set_level(100);
          crazy_log_all();
          break;
        default:
          std::cout << "Unknown verbosity level "<< atoi(argcv[2]) <<std::endl;
          exit(1);
          break;
      }
    }
    
    switch (atoi(argcv[1])) { // choose test
      case(1):
        test_1();
        break;
      case(2):
        test_2();
        break;
      case(3):
        test_3();
        break;
      case(4):
        test_4();
        break;
      case(5):
        test_5();
        break;
      default:
        std::cout << "No such test "<<argcv[1]<<" for "<<argcv[0]<<std::endl;
    }
  }
} 

/*  Some useful code:


if (!stream_1) {
  std::cout << name <<"ERROR: missing test stream 1: set environment variable VALKKA_TEST_RTSP_1"<< std::endl;
  exit(2);
}
std::cout << name <<"** test rtsp stream 1: "<< stream_1 << std::endl;
  
if (!stream_2) {
  std::cout << name <<"ERROR: missing test stream 2: set environment variable VALKKA_TEST_RTSP_2"<< std::endl;
  exit(2);
}
std::cout << name <<"** test rtsp stream 2: "<< stream_2 << std::endl;
    
if (!stream_sdp) {
  std::cout << name <<"ERROR: missing test sdp file: set environment variable VALKKA_TEST_SDP"<< std::endl;
  exit(2);
}
std::cout << name <<"** test sdp file: "<< stream_sdp << std::endl;

  
*/


