 /*
 * live_thread_test.cpp : Testing the LiveThread class
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
 *  @file    live_thread_test.cpp
 *  @author  Sampsa Riikonen
 *  @date    2017
 *  @version 0.1
 *  
 *  @brief Testing the LiveThread class
 *  
 */


#include "livethread.h"
#include "filters.h"
#include "logging.h"

using namespace std::chrono_literals;
using std::this_thread::sleep_for;  // http://en.cppreference.com/w/cpp/thread/sleep_for

const char* stream_1   =std::getenv("VALKKA_TEST_RTSP_1");
const char* stream_2   =std::getenv("VALKKA_TEST_RTSP_2");
const char* stream_sdp =std::getenv("VALKKA_TEST_SDP");


void test_1() {
  const char* name = "@TEST: live_thread_test: test 1: ";
  std::cout << name <<"** @@Starting and stopping, single rtsp connection **" << std::endl;
  
  if (!stream_1) {
    std::cout << name <<"ERROR: missing test stream 1: set environment variable VALKKA_TEST_RTSP_1"<< std::endl;
    exit(2);
  }
  std::cout << name <<"** test rtsp stream 1: "<< stream_1 << std::endl;
  
  // filtergraph:
  // (LiveThread:livethread) --> {FrameFilter:dummyfilter)
  LiveThread livethread("livethread");
  DummyFrameFilter dummyfilter("dummy");
  
  LiveConnectionContext ctx;
  
  std::cout << "starting live thread" << std::endl;
  livethread.startCall();
  
  sleep_for(2s);
  
  ctx = (LiveConnectionContext){LiveConnectionType::rtsp, std::string(stream_1), 2, &dummyfilter};
  livethread.registerStreamCall(ctx);
    
  sleep_for(3s);
  
  std::cout << "stopping live thread" << std::endl;
  livethread.stopCall();
}


void test_2() {
  const char* name = "@TEST: live_thread_test: test 2: ";
  std::cout << name <<"** @@Two connections and filters, starting and stopping, two rtsp connections **" << std::endl;
  
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
  
  bool verbose;
  verbose=true;
  // verbose=false;
  
  // filtergraph:
  // (LiveThread:livethread) --> {FrameFilter:dummyfilter) 
  //                         --> {FrameFilter:dummyfilter2)  
  LiveThread livethread("livethread");
  InfoFrameFilter  dummyfilter1("dummy1");
  DummyFrameFilter dummyfilter2("dummy2",verbose);
  
  LiveConnectionContext ctx;
  
  std::cout << "starting live thread" << std::endl;
  livethread.startCall();
  
  sleep_for(2s);
  
  ctx = (LiveConnectionContext){LiveConnectionType::rtsp, std::string(stream_1), 2, &dummyfilter1};
  livethread.registerStreamCall(ctx);
  
  sleep_for(1s);
  
  livethread.playStreamCall(ctx);
  
  sleep_for(1s);
  
  livethread.stopStreamCall(ctx);
  
  sleep_for(1s);
  
  livethread.stopStreamCall(ctx); // already stopped
  
  sleep_for(3s);
  
  livethread.playStreamCall(ctx); // play again
  
  sleep_for(1s);

  ctx = (LiveConnectionContext){LiveConnectionType::rtsp, std::string(stream_1), 2, &dummyfilter2};
  livethread.registerStreamCall(ctx); // slot already taken
  
  sleep_for(1s);

  ctx = (LiveConnectionContext){LiveConnectionType::rtsp, std::string(stream_2), 3, &dummyfilter2};
  livethread.registerStreamCall(ctx); // register another stream from another
  livethread.playStreamCall(ctx); // play immediately
  
  sleep_for(2s);

  std::cout << "stopping live thread" << std::endl;
  
  livethread.stopCall();
}


void test_3() {
  const char* name = "@TEST: live_thread_test: test 3: ";
  std::cout << name <<"** @@Print payload, one rtsp connection **" << std::endl;
  
  if (!stream_1) {
    std::cout << name <<"ERROR: missing test stream 1: set environment variable VALKKA_TEST_RTSP_1"<< std::endl;
    exit(2);
  }
  std::cout << name <<"** test rtsp stream 1: "<< stream_1 << std::endl;
  
  // filtergraph:
  // (LiveThread:livethread) --> {InfoFrameFilter:dummyfilter1)   
  LiveThread livethread("livethread");
  InfoFrameFilter dummyfilter1("dummy1");
  
  LiveConnectionContext ctx;
  
  std::cout << "starting live thread" << std::endl;
  livethread.startCall();
  
  sleep_for(2s);
  
  ctx = (LiveConnectionContext){LiveConnectionType::rtsp, std::string(stream_1), 2, &dummyfilter1};
  livethread.registerStreamCall(ctx);  
  livethread.playStreamCall(ctx);
  
  sleep_for(1s);
  
  livethread.deregisterStreamCall(ctx); 
  
  sleep_for(1s);
  
  livethread.registerStreamCall(ctx);  
  livethread.playStreamCall(ctx);
  
  
  sleep_for(5s);

  std::cout << "stopping live thread" << std::endl;
  
  livethread.stopCall();
  
}


void test_4() {
  const char* name = "@TEST: live_thread_test: test 4: ";
  std::cout << name <<"** @@Starting and stopping, single sdp connection **" << std::endl;
  
  if (!stream_sdp) {
    std::cout << name <<"ERROR: missing test sdp file: set environment variable VALKKA_TEST_SDP"<< std::endl;
    exit(2);
  }
  std::cout << name <<"** test sdp file: "<< stream_sdp << std::endl;
  
  // filtergraph:
  // (LiveThread:livethread) --> {FrameFilter:dummyfilter) 
  LiveThread livethread("livethread");
  DummyFrameFilter dummyfilter("dummy");
  
  LiveConnectionContext ctx;
  
  std::cout << "starting live thread" << std::endl;
  livethread.startCall();
  
  sleep_for(2s);
  
  ctx = (LiveConnectionContext){LiveConnectionType::sdp, std::string(stream_sdp), 2, &dummyfilter};
  livethread.registerStreamCall(ctx);
  livethread.playStreamCall(ctx);  
  
  sleep_for(3s);
  
  std::cout << "stopping live thread" << std::endl;
  livethread.stopCall();
  
}


void test_5() {
  const char* name = "@TEST: live_thread_test: test 5: ";
  std::cout << name <<"** @@Starting and stopping, various sdp connections **" << std::endl;
  
  if (!stream_sdp) {
    std::cout << name <<"ERROR: missing test sdp file: set environment variable VALKKA_TEST_SDP"<< std::endl;
    exit(2);
  }
  std::cout << name <<"** test sdp file: "<< stream_sdp << std::endl;
  
  // TODO: vectorize this
  // filtergraph:
  // (LiveThread:livethread) 
  //                           --> {FrameFilter:dummyfilter1}
  //                           --> {FrameFilter:dummyfilter2}
  //                           --> {FrameFilter:dummyfilter3}
  //                           ...
  //                           --> {FrameFilter:dummyfilterN}
  //
  LiveThread livethread("livethread");
  DummyFrameFilter dummyfilter1("dummy1");
  DummyFrameFilter dummyfilter2("dummy2");
  DummyFrameFilter dummyfilter3("dummy3");
  DummyFrameFilter dummyfilter4("dummy4");
  DummyFrameFilter dummyfilter5("dummy5");

  LiveConnectionContext ctx1;
  LiveConnectionContext ctx2;
  LiveConnectionContext ctx3;
  LiveConnectionContext ctx4;
  LiveConnectionContext ctx5;
  
  std::cout << "starting live thread" << std::endl;
  livethread.startCall();
  
  sleep_for(2s);
  
  ctx1 = (LiveConnectionContext){LiveConnectionType::sdp, std::string(stream_sdp), 2, &dummyfilter1};
  ctx2 = (LiveConnectionContext){LiveConnectionType::sdp, std::string(stream_sdp), 3, &dummyfilter2};
  ctx3 = (LiveConnectionContext){LiveConnectionType::sdp, std::string(stream_sdp), 4, &dummyfilter3};
  ctx4 = (LiveConnectionContext){LiveConnectionType::sdp, std::string(stream_sdp), 5, &dummyfilter4};
  ctx5 = (LiveConnectionContext){LiveConnectionType::sdp, std::string(stream_sdp), 6, &dummyfilter5};
  
  livethread.registerStreamCall(ctx1);
  livethread.registerStreamCall(ctx2);
  livethread.registerStreamCall(ctx3);
  livethread.registerStreamCall(ctx4);
  livethread.registerStreamCall(ctx5);
  
  livethread.playStreamCall(ctx1);  
  livethread.playStreamCall(ctx2);  
  livethread.playStreamCall(ctx3);  
  livethread.playStreamCall(ctx4);  
  livethread.playStreamCall(ctx5);  
  
  sleep_for(5s);
  
  std::cout << "stopping live thread" << std::endl;
  livethread.stopCall();
  
}


void test_6() {
  const char* name = "@TEST: live_thread_test: test 6: ";
  std::cout << name <<"** @@Inspect stream from a rtsp connection for 10 secs **" << std::endl;
  
  if (!stream_1) {
    std::cout << name <<"ERROR: missing test stream 1: set environment variable VALKKA_TEST_RTSP_1"<< std::endl;
    exit(2);
  }
  std::cout << name <<"** test rtsp stream 1: "<< stream_1 << std::endl;
  
  // filtergraph:
  // (LiveThread:livethread) --> {BriefInfoFrameFilter:dummyfilter1)   
  LiveThread livethread("livethread");
  
  InfoFrameFilter dummyfilter1("info");
  // BriefInfoFrameFilter dummyfilter1("info");
  
  LiveConnectionContext ctx;
  
  std::cout << "starting live thread" << std::endl;
  livethread.startCall();
  
  ctx = (LiveConnectionContext){LiveConnectionType::rtsp, std::string(stream_1), 2, &dummyfilter1};
  livethread.registerStreamCall(ctx);  
  livethread.playStreamCall(ctx);
  
  sleep_for(10s);

  std::cout << "stopping live thread" << std::endl;
  
  livethread.stopCall();
  
}


void test_7() {
  const char* name = "@TEST: live_thread_test: test 7: ";
  std::cout << name <<"** @@Starting and stopping, single rtsp connection.  Testing triggerEvent. **" << std::endl;
  
  if (!stream_1) {
    std::cout << name <<"ERROR: missing test stream 1: set environment variable VALKKA_TEST_RTSP_1"<< std::endl;
    exit(2);
  }
  std::cout << name <<"** test rtsp stream 1: "<< stream_1 << std::endl;
  
  // filtergraph:
  // (LiveThread:livethread) --> {FrameFilter:dummyfilter)
  LiveThread livethread("livethread");
  DummyFrameFilter dummyfilter("dummy");
  
  LiveConnectionContext ctx;
  
  std::cout << "starting live thread" << std::endl;
  livethread.startCall();
  
  sleep_for(2s);
  
  ctx = (LiveConnectionContext){LiveConnectionType::rtsp, std::string(stream_1), 2, &dummyfilter};
  livethread.registerStreamCall(ctx);
    
  livethread.testTrigger();
  sleep_for(1s);
  
  livethread.testTrigger();
  sleep_for(1s);
  
  livethread.testTrigger();
  sleep_for(1s);
  
  // let's try swarming calls..
  livethread.testTrigger();
  // the following calls are ignored. ..!
  livethread.testTrigger();
  livethread.testTrigger();
  livethread.testTrigger();
  livethread.testTrigger();
  livethread.testTrigger();
  livethread.testTrigger();
  livethread.testTrigger();
  
  sleep_for(3s);
  
  std::cout << "stopping live thread" << std::endl;
  livethread.stopCall();
}


void test_8() {
  const char* name = "@TEST: live_thread_test: test 8: ";
  std::cout << name <<"** @@Sending frames between LiveThreads**" << std::endl;
  
  if (!stream_1) {
    std::cout << name <<"ERROR: missing test stream 1: set environment variable VALKKA_TEST_RTSP_1"<< std::endl;
    exit(2);
  }
  std::cout << name <<"** test rtsp stream 1: "<< stream_1 << std::endl;
  
  // filtergraph:
  // (LiveThread:livethread) --> {InfoFrameFilter:info_filter) --> {FifoFrameFilter:fifo_filter} --> [LiveFifo:live_fifo] -->> (LiveThread:livethread2) 
  LiveThread  livethread("livethread");
  LiveThread  livethread2("livethread",20); // stack size for incoming fifo
  
  LiveFifo &live_fifo =livethread2.getFifo();
  FifoFrameFilter fifo_filter("in_live_filter",live_fifo);
  // BriefInfoFrameFilter info_filter("info_filter",&fifo_filter);
  CountFrameFilter info_filter("info_filter",&fifo_filter);
  
  LiveConnectionContext ctx;
  
  std::cout << "starting live threads" << std::endl;
  livethread. startCall();
  livethread2.startCall();
  
  sleep_for(1s);

  ctx = (LiveConnectionContext){LiveConnectionType::rtsp, std::string(stream_1), 2, &info_filter};
  livethread.registerStreamCall(ctx);
  livethread.playStreamCall(ctx);
  
  sleep_for(5s);
  
  livethread.stopStreamCall(ctx);
  
  sleep_for(1s);
  
  std::cout << "stopping live thread" << std::endl;
  livethread. stopCall();
  livethread2.stopCall();
}


void test_9() {
  const char* name = "@TEST: live_thread_test: test 9: ";
  std::cout << name <<"** @@Feeding frames back to LiveThread**" << std::endl;
  
  if (!stream_1) {
    std::cout << name <<"ERROR: missing test stream 1: set environment variable VALKKA_TEST_RTSP_1"<< std::endl;
    exit(2);
  }
  std::cout << name <<"** test rtsp stream 1: "<< stream_1 << std::endl;
  
  // filtergraph:
  // (LiveThread:livethread) --> {InfoFrameFilter:info_filter) --> {FifoFrameFilter:fifo_filter} --> [LiveFifo:live_fifo] -->> (LiveThread:livethread) 
  LiveThread  livethread("livethread",20);
  
  LiveFifo &live_fifo =livethread.getFifo();
  
  FifoFrameFilter fifo_filter("in_live_filter",live_fifo);
  // BriefInfoFrameFilter info_filter("info_filter",&fifo_filter);
  CountFrameFilter info_filter("info_filter",&fifo_filter);
  
  LiveConnectionContext ctx;
  
  std::cout << "starting live threads" << std::endl;
  livethread. startCall();
  
  sleep_for(1s);

  ctx = (LiveConnectionContext){LiveConnectionType::rtsp, std::string(stream_1), 2, &info_filter};
  livethread.registerStreamCall(ctx);
  livethread.playStreamCall(ctx);
  
  sleep_for(5s);
  
  livethread.stopStreamCall(ctx);
  
  sleep_for(1s);
  
  std::cout << "stopping live thread" << std::endl;
  livethread. stopCall();
}


void test_10() {
  const char* name = "@TEST: live_thread_test: test 10: ";
  std::cout << name <<"** @@Sending frames between LiveThreads**" << std::endl;
  
  if (!stream_1) {
    std::cout << name <<"ERROR: missing test stream 1: set environment variable VALKKA_TEST_RTSP_1"<< std::endl;
    exit(2);
  }
  std::cout << name <<"** test rtsp stream 1: "<< stream_1 << std::endl;
  
  // filtergraph:
  // (LiveThread:livethread) --> {InfoFrameFilter:info_filter) --> {FifoFrameFilter:fifo_filter} --> [LiveFifo:live_fifo] -->> (LiveThread:livethread2) 
  LiveThread  livethread("livethread");
  LiveThread  livethread2("livethread2",20); // stack size for incoming fifo
  
  LiveFifo &live_fifo =livethread2.getFifo();
  FifoFrameFilter fifo_filter("in_live_filter",live_fifo);
  // BriefInfoFrameFilter info_filter("info_filter",&fifo_filter);
  CountFrameFilter info_filter("info_filter",&fifo_filter);
  
  LiveConnectionContext ctx;
  LiveOutboundContext out_ctx;
  
  std::cout << "starting live threads" << std::endl;
  livethread. startCall();
  livethread2.startCall();
  
  sleep_for(1s);

  out_ctx = (LiveOutboundContext){LiveConnectionType::sdp, std::string("224.1.168.91"),50000,255,2};
  livethread2.registerOutboundCall(out_ctx);
  
  sleep_for(1s);
  
  ctx = (LiveConnectionContext){LiveConnectionType::rtsp, std::string(stream_1), 2, &info_filter};
  livethread.registerStreamCall(ctx);
  livethread.playStreamCall(ctx);
  
  sleep_for(120s);
  
  livethread.stopStreamCall(ctx);
  
  sleep_for(1s);
  
  std::cout << "stopping live thread" << std::endl;
  livethread. stopCall();
  livethread2.stopCall();
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
      case(6):
        test_6();
        break;
      case(7):
        test_7();
        break;
      case(8):
        test_8();
        break;
      case(9):
        test_9();
        break;
      case(10):
        test_10();
        break;
      default:
        std::cout << "No such test "<<argcv[1]<<" for "<<argcv[0]<<std::endl;
    }
  }
} 




