#include <string>
#include <iostream>
#include <iomanip>
#include <limits>
#include <pthread.h>
#include "json.h"


using namespace std;

void test1()
{
    cout << "\t======= test1: construct a Json and serialize it =======" << endl;

    JsonString *js = new JsonString("王爷爷 is your 	father", JsonString::LITERAL);
    JsonInteger *ji = new JsonInteger(345);
    JsonReal *jr = new JsonReal(12345443555655666362.63567891);
    JsonNull *jn = new JsonNull();
    JsonBool *jb = new JsonBool(false);
    JsonArray *ja = new JsonArray();
    JsonObject *jo = new JsonObject();
    ja->add(new JsonString("haha\\ha\"ha", JsonString::LITERAL))->add(3905362.525);
    jo->add("who", js)->add("how old", ji)->add("how rich", jr)->add("nothing", jn)
      ->add("dead?", jb)->add("array", ja);

    cout << ">>> serialize: " << endl;
    string ss;
    jo->serialize(ss);
    cout << ss << endl;

    cout << ">>> readable:" << endl;
    ss = "";
    jo->readable(0, ss);
    cout << ss << endl;
    delete jo;
}

void test2()
{
    cout << "\n\t======= test2: parse JSON object from RFC8259 =======" << endl;
    setPrecision(12);
    string err;
    string ss = "{\
        \"Image\": {\
            \"Width\":  800,\
            \"Height\": 600,\
            \"Title\":  \"View from 15th Floor\",\
            \"Thumbnail\": {\
                \"Url\":    \"http://www.example.com/image/481989943\",\
                \"Height\": 125,\
                \"Width\":  100\
            },\
            \"Animated\" : false,\
            \"IDs\": [116, 943, 234, 38793]\
          }\
      }";
    JsonObject *jo = asObject(parseJson(ss, err));
    if (jo)
    {
        cout << ">>> parse ok!" << endl;
        cout << ">>> Thumbnail Url is: " << jo->getO("Image")->getO("Thumbnail")->getS("Url")->getLiteral() << endl;

        cout << ">>> now serialize it again." << endl;
        string zz;
        jo->serialize(zz);
        cout << zz << endl;

        cout << ">>> readable: " << endl;
        zz = "";
        jo->readable(0, zz);
        cout << zz << endl;
        delete jo;
    }
    else
    {
        cout << ">>> parse fail" << endl;
        cout << err << endl;
    }
}

void test3()
{
    cout << "\n\t======= test3: parse JSON array from RFC8259 =======" << endl;
    setPrecision(12);
    string err;
    string ss = "[\
        {\
           \"precision\": \"zip\",\
           \"Latitude\":  37.7668,\
           \"Longitude\": -122.3959,\
           \"Address\":   \"\\u738b\\u7237\\u7237\\u5BB6\",\
           \"City\":      \"SAN FRANCISCO\",\
           \"State\":     \"CA\",\
           \"Zip\":       \"94107\",\
           \"Country\":   \"US\"\
        },\
        {\
           \"precision\": \"zip\",\
           \"Latitude\":  37.371991,\
           \"Longitude\": -122.026020,\
           \"Address\":   \"some place\",\
           \"City\":      \"SUNNYVALE\",\
           \"State\":     \"CA\",\
           \"Zip\":       \"94085\",\
           \"Country\":   \"US\"\
        }\
      ]";
    JsonArray *ja = asArray(parseJson(ss, err));
    if (ja)
    {
        cout << ">>> parse ok!" << endl;
        cout << "array[0] Address is: " << ja->getO(0)->getS("Address")->getLiteral() << endl;

        cout << ">>> now serialize it again." << endl;
        string zz;
        ja->serialize(zz);
        cout << zz << endl;

        cout << ">>> readable: " << endl;
        zz = "";
        ja->readable(0, zz);
        cout << zz << endl;

        delete ja;
    }
    else
    {
        cout << ">>> parse fail" << endl;
        cout << err << endl;
    }
}

void test4()
{
    cout << "\n\t======= test4: conversion between literal and jsonstr =======" << endl;
    JsonString js("quoatation:\", reverse solidus:\\, solidus:/, backspace:\b, form feed:\f, line feed:\n, carriage return:\r, tab:\t, 测试test", JsonString::LITERAL);
    cout << "literal-1: " << js.getLiteral() << endl;
    cout << "jsonstr-1: " << js.getJsonstr() << endl << endl;;
    js.setJsonstr("quoatation:\\\", reverse solidus:\\\\, solidus:\\/, backspace:\\b, form feed:\\f, line feed:\\n, carriage return:\\r, tab:\\t, \\u6D4b\\u8bD5test");
    cout << "jsonstr-2: " << js.getJsonstr() << endl;
    cout << "literal-2: " << js.getLiteral() << endl << endl;
    
    JsonString js1("\\u6D4b\\u8bxxx", JsonString::JSONSTR);
    cout << "js1 jsonstr: " << js1.getJsonstr() << endl;
    cout << "js1 literal: " << js1.getLiteral() << endl;
    const vector<string::size_type> &v = js1.getErrPos();
    cout << "error position: ";
    for (auto ite : v)
        cout << ite << "  ";
    cout << endl;
}

struct Thread_Info
{
    pthread_t   tid;
    string      text;
    Json        *json;
    string      err;
};

void* thread_func(void *arg)
{
    Thread_Info *ti = (Thread_Info*)arg;

    ti->json = parseJson(ti->text, ti->err);
    return ti->json;
}

void test5()
{
    cout << "\n\t======= test5: reentrant json parser =======" << endl;
    const int CNT = 100;
    Thread_Info ti[CNT];

    JsonString *js = new JsonString("json gadget implemented by C++", JsonString::LITERAL);
    JsonInteger *ji = new JsonInteger(345);
    JsonReal *jr = new JsonReal(12345443555655666362.63567891);
    JsonNull *jn = new JsonNull();
    JsonBool *jb = new JsonBool(false);
    JsonArray *ja = new JsonArray();
    JsonObject *jo = new JsonObject();
    ja->add(new JsonString("A string in JsonArray", JsonString::LITERAL))->add(3905362.525);
    jo->add("who", js)->add("how old", ji)->add("how rich", jr)->add("nothing", jn)
      ->add("dead?", jb)->add("array", ja);

    string ss;
    jo->readable(0, ss);
    cout << ss << endl;
    delete jo;

    for (int i = 0; i < CNT; ++i)
    {
        ti[i].text = ss;
        if (pthread_create(&ti[i].tid, NULL, &thread_func, &ti[i]))
        {
            perror("pthread_create fail\n");
            exit(1);
        }
    }

    for (int i = 0; i < CNT; ++i)
    {
        if (pthread_join(ti[i].tid, NULL))
        {
            perror("pthread_join fail\n");
            exit(1);
        }
    }

    for (int i = 0; i < CNT; ++i)
    {
        cout << "T" << i << "  " << ti[i].tid << "  ";
        if (ti[i].json)
        {
            cout << "data=" << asObject(ti[i].json)->getA("array")->getS(0)->getLiteral();
            delete ti[i].json;
        }
        else
            cout << "error=" << ti[i].err;
        cout << endl;
    }
}

int main(int argc, char* argv[])
{
    test1();
    test2();
    test3();
    test4();
    test5();
    return 0;
}
