#include <iostream>
#include <vector>
#include "SimpleThread.h"

using namespace std;

typedef enum{
    INIT,
    RUNNING,
    STOP        
} ThreadState;
//runnable class
class MyRunnable : public Runnable {
private:
    Condition condition;
    mutex mmutex;
    ThreadState requestedState=INIT;
    ThreadState currentState=INIT;
    int id;
    void wakeUp(){
        condition.notifyAll();
    };
public:
    MyRunnable(int id){
        this->id=id;
    }
    int getId(){
        return id;
    }
    void setState(ThreadState nState){
        {
        Synchronized x(mmutex);
        requestedState=nState;
        }
        wakeUp();
    };
    ThreadState getState(){
        Synchronized x(mmutex);
        return currentState;
    };
    virtual void run() {
        {
            Synchronized x(mmutex);
            currentState=RUNNING;
        }
        cout << "Thread "<<id<<" started" << endl;
        while(true){
            condition.wait(1000);
            {
                Synchronized x(mmutex);
                if (requestedState==STOP){
                    currentState=STOP;
                    cout << "stopping thread "<<id<<endl;
                    return;
                }
            }
            cout << "hello thread " <<id<< endl;
        }
    }
};
int main(int argc, char ** argv){
	cout << "hello world" << endl;
        int numThreads=5;
        if (argc > 1){
            numThreads=atoi(argv[1]);
        }
        vector<MyRunnable*> runnables;
        for (int i=0;i< numThreads;i++){
            MyRunnable *runnable=new MyRunnable(i);
            Thread *t=new Thread(runnable);
            t->start();
            runnables.push_back(runnable);
        }
        //"misuse" of a condition for ms waits
        Condition waiter;
        //waiting for start
        bool allRunning=false;
        cout << "waiting for threads to start" << endl;
        while (! allRunning){
            allRunning=true;
            vector<MyRunnable*>::iterator it=runnables.begin();
            while (it != runnables.end()){
                if ((*it)->getState() != RUNNING) {
                    cout << "##state "<<((*it)->getId())<<" is "<<((*it)->getState())<<endl;
                    allRunning=false;
                }
                it++;
            }
            waiter.wait(20); //avoid busy loop
        }
        cout << numThreads << " are running" <<endl;
        waiter.wait(10000);
        cout << "stopping" << endl;
        vector<MyRunnable*>::iterator it=runnables.begin();
        while (it != runnables.end()){
            (*it)->setState(STOP);
            it++;
        }
        bool allStopped=false;
        int maxWait=50*10; //max 10s
        while (! allStopped && maxWait){
            allStopped=true;
            vector<MyRunnable*>::iterator it=runnables.begin();
            while (it != runnables.end()){
                if ((*it)->getState() != STOP) allStopped=false;
                it++;
            }
            maxWait--;
            waiter.wait(20); //avoid busy loop
        }
        if (allStopped) cout << "all threads stopped" << endl;
        else cout << "unable to stop all threads" << endl;
        //if we would like to wait here for all threads
        //we also would need to keep the thread objects...
};
