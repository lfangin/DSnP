/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
      _isSorted = false;
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return *(this); }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () {
        _node = _node -> _next;
        return *(this);
      }
      iterator operator ++ (int) {
        iterator tmp = *this;
        _node = _node -> _next;
        return tmp;
      }
      iterator& operator -- () {
        _node = _node -> _prev;
        return *(this);
      }
      iterator operator -- (int) {
        iterator tmp = *this;
        _node = _node -> _prev;
        return tmp;
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
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_head -> _next);}
   iterator end() const { return iterator(_head); }
   bool empty() const { return (_head == _head -> _next); }
   size_t size() const {
     if(empty()) return 0;
     size_t count = 0;
     for(iterator li = begin();li != end();li++)
        count++;
     return count;
   }

   void push_back(const T& x) {
     DListNode<T>* tmp = new DListNode<T>(x,(_head -> _prev),_head);
     (_head -> _prev -> _next) = tmp;
     (_head -> _prev) = tmp;
     _isSorted = false;
   }
   void pop_front() {
     if(empty()) return;
     erase(begin());
   }
   void pop_back() {
     if(empty()) return;
     erase(iterator(_head -> _prev));
   }

   // return false if nothing to erase
   bool erase(iterator pos) {
     if(empty()) return false;
     //if(pos == end()) return false;
     (pos._node -> _prev -> _next) = (pos._node -> _next);
     (pos._node -> _next -> _prev) = (pos._node -> _prev);
     delete pos._node;
     return true;
   }
   bool erase(const T& x) {
     if(empty()) return false;
     iterator it ;
     if(!find(x,it)) return false;
     erase(it);
     return true;
   }

   void clear() {
     DListNode<T>* tmp = _head -> _prev -> _prev;
     while(tmp != _head){
       delete tmp -> _next;
       tmp = tmp -> _prev;
     }
     //delete _head -> _next;
     //whyyyyyyyyyyyyy......
     _head->_prev = _head->_next = _head;
     _isSorted = false;
   }  // delete all nodes except for the dummy node

   void sort() const {
     if(_isSorted == true) return;
     //bubbleSort();
     srand(time(NULL));
     quickSort(begin(),iterator(_head->_prev));
     _isSorted = true;
   }

private:
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions

   void bubbleSort() const{
      for(iterator i = iterator(_head->_prev);i!= begin();i--){
        bool swapped = false;
        for(iterator j = begin();j!=i;){
            bubbleswap(j,++j,swapped);
        }
        if(!swapped) break;
      }
   }

   void quickSort(iterator left, iterator right) const{
         if(right == left) return;
         if(left._node->_next == right._node){
           if(*(left) < *(right)) return;
           else {
              swap(left,right);
              return;
           }
         }
         int count = 0 ;
         if(isrighterthan(left,right,count)) return;
         iterator pivot = left;
         //cout << "count = "<<count<<endl;
         int pivotIndex = (rand()%count) +1;
         //cout <<"pivotIndex = "<<pivotIndex<<endl;
         for(int i =1 ;i!= pivotIndex;i++) ++pivot;

         swap(pivot,right);
         iterator swapIndex = left;
         for (iterator it = left; it != right; ++it){
             if (*(it) <= *(right)){
                 swap(it, swapIndex);
                 ++swapIndex;
             }
         }
         swap(swapIndex, right);

         //if(swapIndex != _head->_next)
            quickSort(left, iterator(swapIndex._node->_prev));
         //if(swapIndex != _head->_prev)
            quickSort( iterator(swapIndex._node->_next), right);
   }

   bool find(const T& x, iterator& it) const{
     for(it = begin();it!=end();it++)
       if(*(it) == x) return true;
     return false;
   }
   void bubbleswap(iterator p, iterator n,bool& swapped)const{
     if(*p > *n){
       T tmp = *p;
       *p = *n;
       *n = tmp;
       swapped = true;
     }
   }
   void swap(iterator p, iterator n)const{
       T tmp = *p;
       *p = *n;
       *n = tmp;
   }
   bool isrighterthan(iterator l , iterator r,int& c)const{
     iterator it = l;
     if(l._node == _head || r._node == _head) return true;
     while(it != r){
       if(it._node == _head) return true;
       ++it;
       ++c;
     }
     return false;
   }
};

#endif // DLIST_H
