/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

      public:
         iterator(vector<Data>* b,size_t n =0,size_t r = 0,size_t c =0):
          _buckets(b),_numBuckets(n),_nrow(r),_ncol(c) {
            //if(_nrow == _numBuckets) _node = 0;
            _node = &_buckets[_nrow][_ncol];
          }

         iterator(const iterator& i):
         _buckets(i._buckets),_nrow(i._nrow),_ncol(i._ncol),_node(i._node) {}
         ~iterator() {}
         const Data& operator * () const { return (*_node); }
         Data& operator * () { return (*_node); }
         iterator& operator ++ () {
           if(_numBuckets == _nrow) return (*this);//_node = 0;
           if(_ncol < _buckets[_nrow].size()-1) _ncol++;
           else{
             while(1){
               _nrow++;
               if(_nrow == _numBuckets) break;
               else if(!_buckets[_nrow].empty()) break;
             }
               _ncol = 0;
           }
           if(_nrow!= _numBuckets)
              _node = &_buckets[_nrow][_ncol];
           return (*this);
         }
         iterator operator ++ (int) {
           iterator tmp = *this;
           ++ *this;
           return tmp; }
         iterator& operator -- () {
           if(_ncol!=0) _ncol--;
           else{
             while(1){
               _nrow--;
               if(_buckets[_nrow].size()) break;
             }
             _ncol = _buckets[_nrow].size()-1;
           }
           _node = &_buckets[_nrow][_ncol];
           return (*this);
         }
         iterator operator -- (int) {
           iterator tmp = *this;
           -- *this;
           return tmp;
         }
         iterator& operator = (const iterator& i) {
           _buckets = i._buckets;
           _node = i._node;
           _nrow = i._nrow;
           _ncol = i._ncol;
           return (*this);
         }
         bool operator != (const iterator& i) const {
           if(_nrow!=i._nrow || _ncol!=i._ncol || _buckets!= i._buckets)
              return true;
           return false;
         }
         bool operator == (const iterator& i) const {
           if(_nrow==i._nrow && _ncol==i._ncol && _buckets== i._buckets)
              return true;
           return false;
          }

      private:
         vector<Data>* _buckets;
         size_t _numBuckets;
         size_t _nrow,_ncol;
         Data*    _node;
     };



   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const {
     size_t i ;
     for(i = 0; i < _numBuckets;i++){
       if(_buckets[i].size()!=0){
         return iterator(_buckets,_numBuckets,i);
       }
     }
     return end();
   }
   // Pass the end
   iterator end() const {return iterator(_buckets,_numBuckets,_numBuckets);}
   // return true if no valid data
   bool empty() const { return (end() == begin()); }
   // number of valid data
   size_t size() const {
     size_t s = 0;
     cout << "numBuckets" <<_numBuckets;
     for(size_t i =0;i < _numBuckets;i++){
       cout << _buckets[i].size() << endl;
       s += _buckets[i].size();
     }
     return s;
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const {
     size_t n = bucketNum(d);
     for(size_t i =0;i < _buckets[n].size();i++)
       if(_buckets[n][i] == d) return true;
     return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const {
     size_t n = bucketNum(d);
     size_t i;
     for(i = 0;i < _buckets[n].size();i++)
        if(d == _buckets[n][i]) break;
     if(i!=_buckets[n].size()){
       d = _buckets[n][i];
       return true;
     }
     return false;
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) {
     size_t n = bucketNum(d);
     size_t i;
     for(i = 0;i < _buckets[n].size();i++)
        if(d == _buckets[n][i]) break;
     if(i!=_buckets[n].size()){
       _buckets[n][i] = d;
       return true;
     }
     insert(d);
     return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) {
     size_t n = bucketNum(d);
     if(check(d)){
       //cout << "check false" << endl;
       return false;
     }
     _buckets[n].push_back(d);
     return true;
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) {
     size_t n = bucketNum(d);
     size_t i ;
     size_t s = _buckets[n].size();
     for(i = 0;i < s;++i)
        if(d == _buckets[n][i]) break;
     if(i!= s){
       _buckets[n][i] = _buckets[n].back();
       _buckets[n].resize(--s);
       return true;
     }
     return false;

   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
