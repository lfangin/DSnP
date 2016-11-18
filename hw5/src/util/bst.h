/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
  friend class BSTree<T>;
  friend class BSTree<T>::iterator;

  BSTreeNode(const T& d, BSTreeNode<T>* p = 0, BSTreeNode<T>* r = 0, BSTreeNode<T>* l = 0):
     _data(d),_parent(p), _right(r), _left(l){}

  T              _data;
  BSTreeNode<T>* _parent;
  BSTreeNode<T>*  _right;
  BSTreeNode<T>*  _left;

};


template <class T>
class BSTree
{
public:
  BSTree() {
     _tail = new BSTreeNode<T>(T());
     //_root = _tail;
     //_root->_right = _root->_left = _root; // _root is a dummy node
     _size = 0;
  }
  ~BSTree() { clear(); delete _tail; }
  class iterator {
    friend class BSTree;

  public:
     iterator(BSTreeNode<T>* n= 0): _node(n) {}
     iterator(const iterator& i) : _node(i._node) {}
     ~iterator() {}
     const T& operator * () const { return *(this); }
     T& operator * () { return _node->_data; }
     iterator& operator ++ () {
       if(_node -> _right) _node = min(_node -> _right);
       //else if(_node == _tail)
       else {
         while(1){
           if(_node -> _parent -> _left == _node){
             _node = _node -> _parent;
             break;
           }
           //else if(_node -> _parent == NULL) break;
           _node = _node -> _parent;
         }
       }
       return *(this);
     }

     iterator& operator -- () {
       if(_node -> _left) _node = max(_node -> _left);
       else{
         while(1){
           if(_node -> _parent -> _right == _node){
             _node = _node -> _parent;
             break;
           }
           _node = _node -> _parent;
         }
       }
       return *(this);
     }

     iterator operator ++ (int) {
       iterator tmp = *(this);
       ++ *this;
       return tmp;
     }
     iterator operator -- (int) {
       iterator tmp = *(this);
       -- *this;
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
     BSTreeNode<T>* _node;

     BSTreeNode<T>* min(BSTreeNode<T>* b)const{
       if(b -> _left == NULL) return b;
       else return (min(b -> _left));
     }
     BSTreeNode<T>* max(BSTreeNode<T>* b)const{
       if(b -> _right == NULL) return b;
       else return (max(b -> _right));
     }
   };

   iterator begin() const { return iterator(min(_root)); }
   iterator end() const { return iterator(_tail); }
   bool empty() const {return !(_size);}
   size_t size() const {return _size;}

   void insert(const T& x) {
     if(empty()){
       _root = new BSTreeNode<T>(x,0,_tail);
     }
     else{
       BSTreeNode<T>* i = _root;
       bool atLeft = false;
       bool ismax = false;
       while(1){
         if( x < (i->_data)){
           if(i -> _left) i = i -> _left;
           else { atLeft = true; break;}
         }
         else {
           if(i -> _right){
             if(i -> _right == _tail){
               ismax = true;
               break;
             }
             else i = i -> _right;
           }
           else break;
         }
       }
       BSTreeNode<T>* tmp = new BSTreeNode<T>(x,i);
       if(atLeft) i -> _left = tmp;
       else{
         i -> _right = tmp;
         if(ismax){
           (_tail -> _parent) = tmp;
           tmp -> _right = _tail;
         }
       }
     }
     _size++;
   }
   void pop_front() {
     if(empty()) return;
     erase(begin());
   }
   void pop_back() {
     if(empty()) return;
     erase(--end());
   }

   // return false if nothing to erase
   bool erase(iterator pos) {
     if(empty()) return false;
     if(_size == 1){
       delete _root;
       _tail -> _parent = NULL;
     }
     int Child = 0 ;
     if(pos._node -> _right) Child += 1;
     if(pos._node -> _left)  Child += 2;

     if(Child == 0){
       if(pos._node -> _parent -> _right == pos._node)
          pos._node -> _parent -> _right = NULL;
       else pos._node -> _parent -> _left = NULL;
       delete pos._node;
     }
     else if(Child == 1){
       if(pos._node -> _parent -> _right)
     }
     else if(Child == 2){

     }
     else{
       iterator it = iterator(succ(pos._node));
       T tmp = *it;
       *it = *pos;
       *pos = tmp;
       erase(it);
     }
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
     BSTreeNode<T>* tmp;
     iterator it = begin();
     while(1){
       tmp = it._node;
       ++it;
       delete tmp;
       if(it == end()) break;
     }
   }  // delete all nodes except for the dummy node

   void sort() const { }
   void print() const {}

private:
   BSTreeNode<T>*  _tail;
   BSTreeNode<T>*  _root;

   size_t _size;

   bool find(const T& x, iterator& it)const{
     for(iterator it = begin();it != end();it++)
       if(*it == x) return true;
     return false;
   }
   BSTreeNode<T>* min(BSTreeNode<T>* b)const{
     if(b -> _left == NULL) return b;
     else return (min(b -> _left));
   }
   BSTreeNode<T>* max(BSTreeNode<T>* b)const{
     if(b -> _right == NULL) return b;
     else return (max(b -> _right));
   }
   BSTreeNode<T>* succ(BSTreeNode<T>* b)const{
     if(b -> _right == NULL) return NULL;
     else return(min(b -> _right));
   }

};

#endif // BST_H
