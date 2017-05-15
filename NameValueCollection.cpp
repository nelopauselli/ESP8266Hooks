/*
  NameValueCollection.h - 
  Created by Nelo Pauselli, 21 de Enero, 2017.
*/
#ifndef NameValueCollection_h
#define NameValueCollection_h

#include "Arduino.h"
#include "assert.h"
#define MAX_SIZE 10

class NameValueCollection
{
  public:
    NameValueCollection(int capacity){
      assert(capacity <= MAX_SIZE);
      
      _capacity = capacity;
      _current = 0;
      //_values = new String[_capacity-1];
      //_keys = new String[_capacity-1];
    }

    bool push(String key, String value){
      if(_current >= _capacity) return false;
      _keys[_current]=key;
      _values[_current++]=value;

      return true;
    }

    String operator[] (const String key){
      for(int i=0;i<_current;i++){
        if(_keys[i]==key){
          return _values[i];
        }
      }
      return "";
    }

    String toString(){
      String sb = "";
      for(int i=0;i<_current;i++){
        sb += _keys[i] + ":" + _values[i] + "&";
      }
      return sb;
    }
    
  private:
    String _values[MAX_SIZE];
    String _keys[MAX_SIZE];
    int _capacity;
    int _current;
};

#endif
