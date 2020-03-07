
#pragma once
#ifndef __2CHANNEL_H__
#define __2CHANNEL_H__


//Library for channels that allow communcation between threads


//template <class T> class channel;

#include <atomic>
#include <thread>
#include <mutex>
//#include <queue>
#include <deque>

using namespace std;

template <class T>
class ChannelTwo{
  mutex * channelLock;
  deque<T> * channelQ;
  atomic<int> producerNumbers;

public:  
  ChannelTwo(){
    channelLock = new mutex;
    channelQ = new deque<T>();
    producerNumbers = 0;
  }

  ChannelTwo(ChannelTwo<T>& other){
    lock_guard<mutex> lock(*(other.channelLock));
    channelQ = other.channelQ;
    producerNumbers = other.producerNumbers;
  }

  void addProducer(){ producerNumbers++;}
  void removeProducer(){ producerNumbers--;}

  int getProducerNumbers(){return producerNumbers;}
  
  int size(){
    channelLock->lock();
    int sz = channelQ->size();
    channelLock->unlock();
    return sz;
  }
  
  T read(){//gets data from queue, removing it in the process
    channelLock->lock();
    while(channelQ->empty()){
      channelLock->unlock();
#ifndef TWINE_QUITE //this may not even neccisarily be an error is a producer is made latter
      if(producerNumbers == 0)throw invalid_argument("Trying to read channel when its empty and there are no producers: deadlock");
#endif //TWINE_QUITE
      this_thread::yield();
      channelLock->lock();
    }
    T tmp = channelQ->front();
    channelQ->pop_front();
    channelLock->unlock();
    return tmp;
  }

  T peek(){//gets data from the queue without changing it
    channelLock->lock();
    while(channelQ->empty()){
      channelLock->unlock();
      this_thread::yield();
      channelLock->lock();
    }
    T tmp = channelQ->front();
    channelLock->unlock();
    return tmp;
  }

  T at(int position){
    channelLock->lock();
    while(channelQ->size() < position){//wait for there to be at position
      channelLock->unlock();
      this_thread::yield();
      channelLock->lock();
    }
    T tmp = channelQ->at(position);
    channelQ->pop_front();
    channelLock->unlock();
    return tmp;
  }

  
  void write(const T &data){//adds data to message queue
    channelLock->lock();
    channelQ->push_back(data);
    channelLock->unlock();
  }

  void operator+=(const T &data){//adds data to message queue
    channelLock->lock();
    channelQ->push_back(data);
    channelLock->unlock();
  }

  void clear(){
    channelLock->lock();
    channelQ->clear();
    channelLock->unlock();
  }

};

template <class T>
class producer{ //: public ChannelTwo<T>{

public:
  ChannelTwo<T> * myChan;
  producer(ChannelTwo<T> * chan){
    myChan = chan;
    chan->addProducer();
  }
  
  ~producer(){
    myChan->removeProducer();
  }
  void write(T data){
    myChan->write(data);
  }
  int size(){
    return myChan->size();
  }
};

template <class T>
class consumer{

public:
  ChannelTwo<T> * myChan;
  consumer(ChannelTwo<T> * chan){
    myChan = chan;
  }
  int size(){
    return myChan->size();
  }
  T read(){
   return myChan->read();
  }
};

#endif //__2CHANNEL_H__
