#include "sh/base/a.h"
#include "sh/net/a.h"
#include "sh/base/Types.h"
#include "sh/base/noncopyable.h"
#include "sh/base/TimeStamp.h"
#include <stdio.h>
#include <iostream>
#include <unistd.h>
using namespace std;
using namespace sh;
class A{int x;};
class B{int y;};
class C:public A,public B{int z;};
class D:public noncopyable{};
void foo(A a){printf("class &a\n");}
void foo(B b){printf("class &b\n");}
void foo(C c){printf("class &c\n");}
void test_Types(){
    C c;
    A * a=&c;
    foo(c);
    foo(implicit_cast<A>(c));
    //down_cast<C*>(a);
   // foo(*down_cast<C*>((A*)&c));
}
void test_noncopyable(){
    D a;
   // D a2=a;
}
void test_timestamp(){
    sh::TimeStamp time = sh::TimeStamp::now();
    cout<<time.toString()<<endl;
    cout<<time.toFormattedString()<<endl;
    sleep(1);
    sh::TimeStamp time2 = sh::TimeStamp::now();
    if(time2 != time){
        cout<<time.toString()<<endl;
        cout<<time2.toString()<<endl;
    }
}

int main(){
    test_Types();
    test_noncopyable();
    test_timestamp();
    return 0;
}
