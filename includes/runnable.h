
#pragma once
#ifndef __RUNNABLE_H__
#define __RUNNABLE_H__


#include <atomic>
#include <thread>
#include "Channel.h"

using namespace std;

template <class T, class C = void>
class runnable: public Channel<T>{

public:
  atomic<bool>  * __run__;
  atomic<bool> * __pause__;
  thread __thread__;
  bool __finish__ = false;
  bool __success__ = false;

  virtual void run() = 0;
  //virtual void init() = 0;
  
  //void (C::*function)(void);
  //void (*function)(void);
  //Channel<T> * myChannel;
  
  runnable(){
    //myChannel = new Channel<T>();
    __run__ = new atomic<bool>;
    __pause__ = new atomic<bool>;
    (*__run__) = true;
    (*__pause__) = false;
    this->addProducer();
  }
  
  /*runnable(void (C::*f)(void), bool isProducer, bool isConsumer){
    //myChannel = new Channel<T>();
    __run__ = new atomic<bool>;
    __pause__ = new atomic<bool>;
    function = f;
    (*__run__) = true;
    (*__pause__) = false;
    if(isProducer){
      //myChannel->addProducer();
      this->addProducer();
    }
    myThread = thread(function);
    }*/

  void masterRunFunction() {
    //if(&init != 0) init();
    try{
      //start
      while(*__run__){
	if(*__pause__){
	  this_thread::yield();
	}else{
	  run();
	}
      }//end of run
      //end
      __success__ = true;//if we get here, no uncaught were thrown
    }catch(const exception& e){
      __finish__ = true;//either way were done
      throw e;
    }
    __finish__ = true;
  }

  
  void start(){
    __thread__ = thread(&runnable::masterRunFunction, this);
  }

  void pause(const bool p = true){
    (*__pause__) = p;
  }

  void resume(){
    (*__pause__) = false;
  }

  void end(){
    (*__pause__) = false;
    (*__run__) = false;
  }
  
  void safeEnd(){
    (*__pause__) = false;
    (*__run__) = false;
    __thread__.join();
  }

  void forceEnd(){
    (*__pause__) = false;
    (*__run__) = false;
    __thread__.detach();
    __thread__.~thread();
  }

  void sync(){
    __thread__.join();
  }
  
  //reimplement receive to be non-blocking when thread is dead
  /*T receive(){//gets data from queue, removing it in the process
    this->channelLock.lock();
    while(this->channelQ.empty()){//wait for data
      this->channelLock.unlock();
      if(! *__run__)
	return T();
      this_thread::yield();
      this->channelLock.lock();
    }
    T tmp = this->channelQ.front();
    this->channelQ.pop_front();
    this->channelLock.unlock();
    return tmp;
    }*/


};


//class simpleRunnable{}//TODO?




#endif //__RUNNABLE_H__
