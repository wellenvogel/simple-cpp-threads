#include <iostream>
#include <thread>
#include <unistd.h>
#include <mutex>

using namespace std;
//simple automatic unlocking mutex
class Synchronized{
private:
    mutex * mmutex;
public:
    Synchronized(mutex *m){
        mmutex=m;
        mmutex->lock();
    };
    ~Synchronized(){
        mmutex->unlock();
    };
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
    thread *mthread=NULL;
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
        mthread=new thread([this]{this->run();});
    }
    void join(){
        if (! mthread) return;
        mthread->join();
        delete mthread;
        mthread=NULL;
    };
};

//runnable class
class MyRunnable : public Runnable {
private:
    mutex mmutex;
    int doStop=0;
public:
    void stop(){
        Synchronized x(&mmutex);
        doStop=1;
    };
    virtual void run() {
        for (int i = 0; i < 5; i++) {
            sleep(1);
            {
                Synchronized x(&mmutex);
                if (doStop) return;
            }
            cout << "hello thread" << endl;
        }
    }
};
int main(int argc, char ** argv){
	cout << "hello world" << endl;
        MyRunnable runnable;
	Thread t(&runnable);
        t.start();
        sleep(3);
        runnable.stop();
        t.join();	
};
