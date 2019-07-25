#include "sh/base/a.h"
#include "sh/net/a.h"
#include "sh/base/Types.h"
#include "sh/base/noncopyable.h"
#include "sh/base/TimeStamp.h"
#include "sh/base/CurrentThread.h"
#include "sh/base/Exception.h"
#include "sh/base/Date.h"
#include "sh/base/Mutex.h"
#include "sh/base/Condition.h"
#include "sh/base/CountDownLatch.h"
#include "sh/base/Atomic.h"
#include "sh/base/BlockingQueue.h"
#include "sh/base/BoundedBlockingQueue.h"
#include <stdio.h>
#include <boost/circular_buffer.hpp>
#include <iostream>
#include <unistd.h>

using namespace std;
using namespace sh;

BoundedBlockingQueue<string> bq(3);

void* run(void *){
    sleep(1);
    pthread_detach(pthread_self());
    string str;
    while(1){
        sleep(1);
        str = bq.take();
         printf("%s size %d\n",str.c_str(),bq.size());
         fflush(stdout);
    }

}

int main(){

    pthread_t t;
    printf("test\n");
    pthread_create(&t,NULL,run,NULL);
    int i=0;
    while(1){
        ++i;
        string s = "hellow";
        s+=char('0'+i%10);
        bq.put(s);
        sleep(1);
    }
    return 0;
}
