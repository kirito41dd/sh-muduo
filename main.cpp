#include "sh/base/a.h"
#include "sh/net/a.h"
#include "sh/base/Types.h"
#include "sh/base/noncopyable.h"
#include "sh/base/TimeStamp.h"
#include "sh/base/CurrentThread.h"
#include "sh/base/Exception.h"
#include "sh/base/Date.h"
#include <stdio.h>
#include <iostream>
#include <unistd.h>

using namespace std;
using namespace sh;


void test_timestamp(){
    sh::TimeStamp time = sh::TimeStamp::localNow();
    cout<<time.toString()<<endl;
    cout<<time.toFormattedString()<<endl;
    sleep(1);
    sh::TimeStamp time2 = sh::TimeStamp::now();
    if(time2 != time){
        cout<<time.toString()<<endl;
        cout<<time2.toString()<<endl;
    }
}
void test_Exception() throw(Exception)
{
    if(1)
    throw Exception("err");
}

int main(){

    test_timestamp();
    cout<<CurrentThread::stackTrace(1)<<endl;
    try{
        test_Exception();
    }catch(Exception e){
        cout<<e.what()<<endl;
        cout<<e.stackTrace()<<endl;
    }

    cout<<Date::getLocalDate().toIsoString()<<endl;

    return 0;
}
