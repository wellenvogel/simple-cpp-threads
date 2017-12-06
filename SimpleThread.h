/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SimpleThread.h
 * Author: andreas
 *
 * Created on December 6, 2017, 7:25 AM
 */

#ifndef SIMPLETHREAD_H
#define SIMPLETHREAD_H
#include <thread>
#include <mutex>
#include <condition_variable>
//simple automatic unlocking mutex
typedef std::lock_guard<std::mutex> Synchronized;

//simple condition variable that encapsulates the monitor
class Condition{
private:
    std::mutex mutex;
    std::condition_variable cond;
public:
    void wait(){
        std::unique_lock<std::mutex> l(mutex);
        cond.wait(l);
    };
    void wait(int millis){
        std::unique_lock<std::mutex> l(mutex);
        cond.wait_for(l,std::chrono::milliseconds(millis));
    }
    void notify(){
        Synchronized g(mutex);
        cond.notify_one();
    }
    void notifyAll(){
        Synchronized g(mutex);
        cond.notify_all();
    }
};

//java like runnable interface
class Runnable{
public:
    virtual void run()=0;
};

//java like thread class
class Thread{
private:
    Runnable *runnable;
    std::thread *mthread=NULL;
    void run(){
        runnable->run();
    }
public:
    Thread(Runnable *runnable){
        this->runnable=runnable;
    };
    ~Thread(){
        if (mthread) delete mthread;
    }
    void start(){
        if (mthread) return;
        mthread=new std::thread([this]{this->run();});
    }
    void join(){
        if (! mthread) return;
        mthread->join();
        delete mthread;
        mthread=NULL;
    };
};


#endif /* SIMPLETHREAD_H */

