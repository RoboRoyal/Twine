
#pragma once
#ifndef __CHANNEL_H__
#define __CHANNEL_H__


//Library for channels that allow communcation between threads
//impliments with a deque and a mutex lock
//Dakota A. June 4, 2019

#include <string>//for toString

#include <atomic>
#include <thread>//this_thread::yield();
#include <mutex>
#include <deque>

#include "Object.h"
#include "TypeInfoLib.h"

using namespace std;

template <class T>
class ChannelBase{
public:
  mutex * channelLock;
  deque<T> channelQ;
  atomic<int> * producerNumbers;
  unsigned long maxQueueSize;
};

template <class T>
class Channel: public Object{// : public ChannelBase<T>{
  
public:
  mutex channelLock;
  deque<T> channelQ;
  atomic<int> producerNumbers;
  unsigned long maxQueueSize;
  
  Channel(){
    this->channelQ = deque<T>();
    this->producerNumbers = 0;
    this->maxQueueSize = -1;
  }
  
  Channel(Channel<T>& other){//TODO im, pretty sure this is wrong
    lock_guard<mutex> lock(other.channelLock);
    this->channelQ = other.channelQ;
    this->producerNumbers = 0;
    this->maxQueueSize = other->maxQueueSize;
  }

  void addProducer(){ ((this->producerNumbers)++);}
  void removeProducer(){ (this->producerNumbers--);}
    
  int size(){
    this->channelLock.lock();
    int channelSize = this->channelQ.size();
    this->channelLock.unlock();
    return channelSize;
  }

  T receive(){//gets data from queue, removing it in the process
    this->channelLock.lock();
    while(this->channelQ.empty()){//wait for data
      this->channelLock.unlock();
#ifndef TWINE_QUITE //this may not even neccisarily be an error is a producer is made latter
      if(producerNumbers == 0)throw invalid_argument("Trying to read channel when its empty and there are no producers: deadlock");
#endif //TWINE_QUITE
      //sleep(waitTime);
      this_thread::yield();
      this->channelLock.lock();
    }
    T tmp = this->channelQ.front();
    this->channelQ.pop_front();
    this->channelLock.unlock();
    return tmp;
  }

  T peek(){//gets data from the queue without changing it
    this->channelLock.lock();
    while(this->channelQ.empty()){
      this->channelLock.unlock();
      this_thread::yield();
      this->channelLock.lock();
    }
    T tmp = this->channelQ.front();
    this->channelLock.unlock();
    return tmp;
  }

  T at(int position){
    this->channelLock.lock();
    while(this->channelQ.size() < position){//wait for there to be at position
      this->channelLock.unlock();
      this_thread::yield();
      this->channelLock.lock();
    }
    T tmp = this->channelQ.at(position);
    this->channelQ.pop_front();
    this->channelLock.unlock();
    return tmp;
  }
  
  int send(const T &data){

    this->channelLock.lock();
    if(this->channelQ.size() <= this->maxQueueSize)
      this->channelQ.push_back(data);
    else{
      this->channelLock.unlock();
      return 1;
    }
    this->channelLock.unlock();
    return 0;
  }
  //send multiple messages at once, is more efficent than one at a time
  void bulkSend(const T data[]){
    this->channelLock.lock();

    const unsigned size = sizeof(data)/sizeof(data[0]);
    //channelQ.get_allocator().allocate(size);
    
    for(unsigned i = 0; i < size;i++)
      this->channelQ.push_back(data[i]);
    
    this->channelLock.unlock();
  }
  void bulkSend(const vector<T> &data){
    this->channelLock.lock();
    //channelQ.get_allocator().allocate(data.size());

    for(unsigned i = 0; i < data.size();i++)
      this->channelQ.push_back(data[i]);
    this->channelLock.unlock();
  }
  void operator+=(const T &data){
    this->channelLock.lock();
    this->channelQ.push(data);
    this->channelLock.unlock();
  }

  void clear(){
    this->channelLock.lock();
    this->channelQ.clear();
    this->channelLock.unlock();    
  }
  //Channel& operator= (Channel& other){//TODO }

  string toString(){
    return string("Channel@")+to_string((long long)this);// + type(T);//This wont work with custom types
  }
  double toNum(){return this;}
  
};

//template <class T>
//const double Channel<T>::waitTime = .005;

//template <class T>
//static atomic<long> producerNumbers = 0;

template <class T>
class producer{//TODO it would be better to just extend channel and delete unbsused functions?
  
public:
  Channel<T> * myChan;
  producer(Channel<T>& chan){
    chan.addProducer();
    myChan = &chan;
  }
  ~producer(){
    myChan->removeProducer();
  }
  void write(T data){myChan->write(data);}

};

template <class T>
class consumer : public ChannelBase<T>{

public:
  Channel<T> * myChan;
  consumer(Channel<T>& chan){
    myChan = &chan;
  }
  T read(){return myChan->read();}

};

#endif //__CHANNEL_H__
