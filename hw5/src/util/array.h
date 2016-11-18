/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // TODO: decide the initial value for _isSorted
   Array() : _data(0), _size(0), _capacity(0) , _isSorted(false){}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return (*this); }
      T& operator * () { return (*_node); }
      // what's the dofference.............
      iterator& operator ++ () {
        _node ++ ;
        return (*this);
      }
      iterator operator ++ (int) {
        iterator tmp = *this;
        ++ *this;
        return tmp; }
      iterator& operator -- () {
        _node -- ;
        return (*this);
      }
      iterator operator -- (int) {
        iterator tmp = *this;
        -- *this;
        return tmp;
      }
      iterator operator + (int i) const {
        iterator tmp = *this;
        tmp += i;
        return tmp;
      }
      iterator& operator += (int i) {
        _node += i;
        return (*this);
      }
      iterator& operator = (const iterator& i) {
        _node = i._node;
        return (*this);
      }
      bool operator != (const iterator& i) const {
        if(_node == i._node) return false;
        return true;
      }
      bool operator == (const iterator& i) const {
        if(_node == i._node) return true;
        return false;
       }

   private:
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_data); }
   iterator end() const { return iterator(_data+_size); }
   bool empty() const { if(_size == 0) return true; return false; }
   size_t size() const { return _size; }

   T& operator [] (size_t i) { return _data[i]; }
   const T& operator [] (size_t i) const { return _data[i]; }

   void push_back(const T& x) {
     if(_size == _capacity) expand();
     _data[_size] = x;
     _size ++;
     _isSorted = false;
   }
   void pop_front() {
     if(empty()) return;
     if(_size >= 2) *_data = *(_data+_size-1);
     -- _size;
   }
   void pop_back() {
     -- _size ;
   }

   bool erase(iterator pos) {
    if(empty()) return false;
    if(_size >= 2) *(pos._node) = *(_data+_size-1);
    _isSorted = false;
    -- _size;
    return true;
   }
   bool erase(const T& x) {
     iterator it ;
     if(!find(x,it)) return false;
     erase(it);
     return true;
   }

   void clear() { _size = 0; _isSorted = false; }

   // This is done. DO NOT change this one.
   void sort() const { if (!empty()) ::sort(_data, _data+_size);
    _isSorted = true;
   }

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }

private:
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted
   void expand(){
     if(_capacity == 0) _capacity = 1;
     else _capacity *= 2;
     T* tmp = _data;
     _data = new T[_capacity];
     for(int i=0;i<_size;i++) _data[i] = tmp[i];
     delete[] tmp;
   }
   bool find(const T& x, iterator& it) const{
     for(T* i = _data;i < _data+ _size ;i++){
       if(*i == x){
         it._node = i;
         return true;
       }
     }
     return false;
   }

   // [OPTIONAL TODO] Helper functions; called by public member functions
};

#endif // ARRAY_H
