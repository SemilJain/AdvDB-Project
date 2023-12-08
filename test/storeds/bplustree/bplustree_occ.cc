// #include <stdio.h>
// #include <stdlib.h>
// #include <assert.h>
// #include <stdint.h>
// #include <string.h>
// #include <stdbool.h>
// #include <iostream>
// #include<vector>
// #include<queue>
// #include "lock.h"
// #include <sched.h>

// #include "bplustree_common.h"

// namespace ycsbc {
//     class Node{
//     public: 
//     std::vector<uint64_t> ele;
//     std::vector<Node *> addr;
//     std::vector<uint64_t> val;
//     uint64_t B;
//     bool isleaf;
//     ReaderWriterLock * lock;
//     Node(int size, bool b){
//         ele.resize(size,0);
//         addr.resize(size+1, NULL);
//         val.resize(size,0);
//         isleaf= b;
//         B= size;
//         this->lock = (ReaderWriterLock *) malloc(sizeof(ReaderWriterLock));
//         rw_lock_init(this->lock);
//     }
//     void insertInLNode(uint64_t x, uint64_t y){
//         for(int i=B-1;i>=0;i--){
//             if(ele[i] == 0){continue;}
//             if(x < ele[i]){
//                 ele[i+1]= ele[i];
//                 val[i+1] = val[i];}
//             else{
//                 ele[i+1]= x;
//                 val[i+1]= y;
//                 return;
//                 break;
//             }
//         }
//         ele[0] = x;
//         val[0] = y;
//     }
//     void insertInINode(Node * q, uint64_t x , Node *original){
//         int ind;
//         for(int k=0;k<=B;k++){
//             if(addr[k]==original){
//                 ind=k;
//                 break;
//             }
//             else{
//                 continue;
//             }
//         }
//         for(int k= B-2;k>=ind;k--){
//             ele[k+1]= ele[k];
//         }
//         for(int k= B-1;k>ind;k--){
//             addr[k+1]= addr[k];
//         }
//         ele[ind] = x;
//         addr[ind+1]= q; 
//     }
//     void splitRLNode1(){
//         Node * t1= new Node(B, 1); 
//         Node * t2= new Node(B, 1);
        
//         int j=0;
//         for(int k=0;k<B/2;k++){
//             t1->ele[j] =ele[k];
//             t1->val[j] =val[k];
//             ele[k]=0;
//             val[k]=0;
//             j++;  
//         }
//         j=0;
//         for(int k=B/2;k<B;k++){
//             t2->ele[j] =ele[k];
//             t2->val[j] =val[k];
//             ele[k]=0;
//             val[k]=0;
//             j++;  
//         } 
//         ele[0]= t2->ele[0];
//         addr[0]=t1;
//         addr[1]= t2;
//         isleaf=0;
//         t1->addr[1] = t2;
//         t2->addr[0]= t1;
        
//     }
//     Node * splitRINode(){
//        Node * t1= new Node(B, 0); 
//        Node * t2= new Node(B, 0);
        
//        int j=0;
//         for(int k=0;k<B/2-1;k++){
//             t1->ele[j] =ele[k];
//            // t1->val[j] =val[k];
//             ele[k]=0;
//             //val[k]=0;
//             j++;  
//         }
//         j=0;
//         uint64_t element = ele[B/2-1];
//         ele[B/2-1]=0;
//         for(int k=B/2;k<B;k++){
//             t2->ele[j] =ele[k];
//             ele[k]=0;
//             j++;  
//         }  
//         j=0;
//         for(int i=0;i<=B/2-1;i++){
//             t1->addr[j]= addr[i];
//             j++;
//             addr[i]=NULL;
//         }
//         j=0;
//         for(int i=B/2;i<=B;i++){
//             t2->addr[j]= addr[i];
//             j++;
//             addr[i]=NULL;
//         }
//         ele[0]= element;
//         addr[0]= t1;
//         addr[1]= t2;
//     }
//     Node * splitRLNode(){
//         Node * t= new Node(B, 1); 
//         Node * newroot= new Node(B,0);
//         int j=0;
//         for(int k=B/2;k<B;k++){
//             t->ele[j] =ele[k];
//             t->val[j] =val[k];
//             ele[k]=0;
//             val[k]=0;
//             j++;  
//         } 
//         this->addr[1] = t;
//         t->addr[0] = this;
//         newroot->ele[0] = t->ele[0];
//         newroot->addr[0] = this;
//         newroot->addr[1] =  t;
//         return newroot;
//     }
    
//     std::pair<Node *, uint64_t> splitINode(){
//         Node * t= new Node(B, 0); 
//         int j=0;
//         Node *addr1, *addr2;
//         addr1 = this;
//         addr2 = t;
//         uint64_t element = ele[B/2];
//         ele[B/2] = 0;

//         for(int k=B/2+1;k<B;k++){
//             t->ele[j] = ele[k];
//             t->addr[j]= addr[k];
//             ele[k]=0;
//             addr[k] =0;
//             j++;  
//         } 
//         t->addr[j] = addr[B];
//         addr[B]=0; 
//         return {t, element};
//     }
//     std::pair<Node *, uint64_t> splitLNode(){
//         Node * t= new Node(B, 1); 
//         int j=0;
//         for(int k=B/2;k<B;k++){
//             t->ele[j] =ele[k];
//             t->val[j] =val[k];
//             ele[k]=0;
//             val[k]=0;
//             j++;  
//         } 
//         uint64_t element =  t->ele[0];
//         t->addr[1] = this->addr[1];
//         this->addr[1] = t;
//         t->addr[0] = this;
//         if(t->addr[1]!=NULL)
//             t->addr[1]->addr[0]= t;

//         return {t, element};
//     }
//     void deleteLNode(Node *t ,uint64_t x){
//         int ind = -1;
//         for(uint64_t i =0; i<B;i++){
//             if (t->ele[i] == x){
//                 ind = i;
//                 break;
//             } 
//         }
//         for (uint64_t i = ind; i < B-1; i++){
//             t->ele[i] = t->ele[i+1];
//             t->val[i] = t->val[i+1];
//         }
//         t->ele[B-1]=0;
//         t->val[B-1]=0;
//     }
    
// };

//     class BPlusTreeOCC : public StoredsBase {
//     public:
//         BPlusTreeOCC(const char *path) {
//             BPlusTreeOCC::init(path);
//         }

//         int init(const char *path);

//         int read(const uint64_t key, void *&result);

//         int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

//         int update(const uint64_t key, void *value);

//         int insert(const uint64_t key, void *value);

//         // void destroy();

//         ~BPlusTreeOCC();

//         Node *root;
//         uint64_t B;

        
//     void insert1(uint64_t x, uint64_t y){
//         Node *t = root;
//         Node * par =NULL;
//         if(root == NULL){
//             Node  * temp = new Node(B, 1);
//             write_lock(temp->lock);
//             temp->ele[0] = x;
//             temp->val[0] = y;
//             root= temp;
//             write_unlock(temp->lock);
//         }else if(t->isleaf == 1){
//             if(t->ele[B-1] == 0){
//                 write_lock(t->lock);
//                 t->insertInLNode( x, y);
//                 write_unlock(t->lock);
//             }
//             else{
//                 write_lock(t->lock);
//                 t->splitRLNode1();
//                 if(x >= root->ele[0]){
//                     t= root->addr[1];
//                     // write_lock(t->lock);
//                 }
//                 else{
//                     t = root->addr[0];
//                     // write_lock(t->lock);
//                 }
//                 write_lock(t->lock);
//                 t->insertInLNode(x,y);
//                 write_unlock(root->lock);

//                 write_unlock(t->lock);
//             }
//         }
//         else{
//             while(!t->isleaf){
//                 if(t->ele[B-1] == 0){
//                     if(t==root){
//                         write_lock(root->lock);
//                     }
//                     if(par!=NULL)
//                         write_unlock(par->lock);
//                     par = t;
//                     t = gotochild2(t, x);
//                     write_lock(t->lock);
                    
//                 }else{
//                     if(t==root){
//                         write_lock(root->lock);
//                         t->splitRINode();
//                         par= t;
//                         t= gotochild2(t, x);
//                         write_lock(t->lock);
//                     }else{
//                         std::pair<Node *, uint64_t> p = t->splitINode();
//                         par->insertInINode(p.first, p.second, t);
//                         if(x >= p.second){
//                             write_unlock(t->lock);
//                             t= p.first;
//                             write_lock(t->lock);
//                         }
//                         write_unlock(par->lock);
//                         par= t;
//                         t= gotochild2(t, x);
//                         write_lock(t->lock);
//                     }
//                 }
//             }

//             if(t->isleaf){
//                 if(t->ele[B-1]==0){
//                     t->insertInLNode(x,y);
//                     write_unlock(par->lock);
//                     write_unlock(t->lock);
//                 }
//                 else{
//                     Node * temp = t->addr[1];
//                     if(temp != NULL){
//                         write_lock(temp->lock);
//                     }
//                     std::pair<Node *, uint64_t> p = t->splitLNode();
                    
//                     par->insertInINode(p.first, p.second, t);

//                     if(x >= p.second){
//                         write_unlock(t->lock);
//                         t= p.first;
//                         write_lock(t->lock);
//                     }
//                     t->insertInLNode(x,y);
//                     if(temp != NULL){
//                         write_unlock(temp->lock);
//                     }
//                     write_unlock(par->lock);
//                     write_unlock(t->lock);
//                 }
//             }
//         }
//     }
//     Node * gotochild(Node * t, uint64_t x){
//         int cpuid = sched_getcpu();
//         for(int i=0;i<B;i++){
//             if(t->ele[i]==0){
//                 read_unlock(t->lock, cpuid);
//                 t = t->addr[i];
//                 read_lock(t->lock, cpuid);
//                 return t;

//             }
//             if(x < t->ele[i]){
//                 read_unlock(t->lock, cpuid);
//                 t= t->addr[i];
//                 read_lock(t->lock, cpuid);
//                 return t; 
//             }
//         }
//         read_unlock(t->lock, cpuid);
//         t= t->addr[B];
//         read_lock(t->lock, cpuid);
//         return t;
//     }

//     Node * gotochild2(Node * t, uint64_t x){
//         for(int i=0;i<B;i++){
//             if(t->ele[i]==0){
//                 // read_unlock(t->lock, cpuid);
//                 t = t->addr[i];
//                 // read_lock(t->lock, cpuid);
//                 return t;

//             }
//             if(x < t->ele[i]){
//                 // read_unlock(t->lock, cpuid);
//                 t= t->addr[i];
//                 // read_lock(t->lock, cpuid);
//                 return t; 
//             }
//         }
//         // read_unlock(t->lock, cpuid);
//         t= t->addr[B];
//         // read_lock(t->lock, cpuid);
//         return t;
//     }

//     bool query(uint64_t x){
//         int cpuid = sched_getcpu();
//         Node * t= root;
//         read_lock(t->lock, cpuid);
//         while(!t->isleaf){   
//             t= gotochild(t, x);
            
//         }
//         for (int i=0; i<B; i++){
//             if (t->ele[i] == x){
//                 read_unlock(t->lock, cpuid);
//                 return 1;
//             }
//         }
//         read_unlock(t->lock, cpuid);
//         return 0;
//     }
//     // void print(){
//     //     queue<Node *> q;
//     //     q.push(root);
//     //     //cout<<root<<" ";
//     //     while(!q.empty()){
//     //         Node *t = q.front();
//     //         q.pop();
//     //         cout<<t->isleaf<<endl;
//     //         cout<<t<<endl;
//     //         for(int i=0;i<B;i++){
//     //             cout<<t->ele[i]<<" ";
//     //         }
//     //         cout<<endl;
//     //         for(int i=0;i<B;i++){
//     //             cout<<t->val[i]<<" ";
//     //         }
//     //         cout<<endl;
//     //         for(int i=0;i<=B;i++){
//     //             if(!t->isleaf && t->addr[i]!=NULL)
//     //                 q.push(t->addr[i]);
//     //             cout<<t->addr[i]<<" ";
//     //         }
//     //         cout<<endl;
//     //         cout<<endl;

//     //     }
//     // }
//    //remove
//    //delparele
//    //mergeleft
//    //mergeright
//    //canborrow
//    //borrow
//    //checksize
   

//    void remove(int x){
//         Node * t = root;
//         write_lock(root->lock);
//         Node *par = NULL;
        
//         while(!t->isleaf){
            
//             if (t == root){
                
//                 par = t;
//                 t = gotochild2(t,x);
//                 write_lock(t->lock);
//             }
//             else{
                
//                 if(t->ele[B/2-1] == 0 ){
                    
//                     std::vector<uint64_t> v = canBorrow(par, t);
//                     if (v[0] == 0){
//                         // merge function
//                         if (v[3]==0){
//                             Node *sib = par->addr[v[1]];
//                             write_lock(sib->lock);
//                             //write_lock(par->addr[v[1]]);
//                             Node *mnode=merge_left(par, t,v);
//                             write_unlock(sib->lock);
//                             write_unlock(t->lock);
//                             t= mnode;
//                             write_lock(t->lock);
//                         }
//                         else{
//                             Node *sib = par->addr[v[1]+1];
//                             write_lock(sib->lock);
//                             Node *mnode=merge_right(par, t,v);
//                             write_unlock(sib->lock);
//                             write_unlock(t->lock);
//                             t= mnode;
//                             write_lock(t->lock);
//                         }
//                         if(par== root && root->ele[0]==0){
//                             Node * n= root->addr[0];
//                             for(int i=0;i<B;i++){
//                                 root->ele[i] = n->ele[i];
//                                 root->addr[i] = n->addr[i];
//                             }
//                             root->addr[B] =n->addr[B];
//                             par= root;
//                             //root= root->addr[0];
//                         }
//                         else{
//                             write_unlock(par->lock);
//                             par = t;
//                         }
//                         // Node *q=t;
//                         t = gotochild2(t,x);
//                         // write_unlock(q);
//                         write_lock(t->lock);

//                     }
//                     else{
//                         // borrow function
//                         if(v[3]==0){
//                             write_lock(par->addr[v[1]]->lock);
//                         }else{
//                             write_lock(par->addr[v[1]+1]->lock);
//                         }
//                         borrow(par, t, v);
//                         if(v[3]==0){
//                             write_unlock(par->addr[v[1]]->lock);
//                         }else{
//                             write_unlock(par->addr[v[1]+1]->lock);
//                         }
//                         write_unlock(par->lock);
//                         par = t;
//                         t = gotochild2(t,x);
//                         write_lock(t->lock);
//                     }
//                 }
//                 else{
//                     write_unlock(par->lock);
//                     par = t;
//                     t = gotochild2(t,x);
//                     write_lock(t->lock);
//                 }
//             }
//         }

//         int ind = -1;
//         for(uint64_t i =0; i<B;i++){
//             if (t->ele[i] == x){
//                 ind = i;
//                 break;
//             } 
//         }
        
//         if(ind == -1){
//             write_unlock(par->lock);
//             write_unlock(t->lock);
//             return;
//         }
//         for (uint64_t i = ind; i < B-1; i++){
//             t->ele[i] = t->ele[i+1];
//             t->val[i] = t->val[i+1];
//         }
//         t->ele[B-1]=0;
//         t->val[B-1]=0;
//         if(t==root){
//             write_unlock(t->lock);
//         }else{
            
//             if(t->ele[0] == 0 ){
//                 delParEle(par, t);
//             }
        
//             if(root->ele[0] == 0){
//                 write_unlock(t->lock);
//                 Node * n= root->addr[0];
//                 write_lock(n->lock);
//                 for(int i=0;i<B;i++){
//                     root->ele[i] = n->ele[i];
//                     root->val[i] = n->val[i];
//                 }
//                 root->addr[0] =NULL;
//                 root->isleaf = 1;
//                 write_unlock(n->lock);
//                 write_unlock(root->lock);
//                 //root= root->addr[0];
//             }else{
//                 write_unlock(par->lock);
//                 write_unlock(t->lock);
//             }
//         }
//     }
//     void delParEle(Node *par, Node *t){
//         uint64_t ind =0;
//         for(int i=0;i<=B;i++){
//             if(par->addr[i] ==  t){
//                 ind= i;
//                 break;
//             }
//         }
//         for(int i=ind;i<B;i++){
//             par->addr[i]= par->addr[i+1];
//         }
//         par->addr[B]=NULL;
//         if(par->ele[ind] == 0)ind= ind-1;
//         for(int i=ind;i<B-1;i++){
//             par->ele[i]= par->ele[i+1];
//         }
//         par->ele[B-1]=0;

//     }
//     Node* merge_right( Node *par, Node *t, std::vector<u_int64_t> v){
//         uint64_t num = par->ele[v[1]];
//         //cout<<num<<endl;
//         Node * rsibling = par->addr[v[1]+1];
//         for(int i=v[1];i<B-1;i++){
//             par->ele[i] = par->ele[i+1];
//         }
//         par->ele[B-1] =0;
//         for(int i=v[1];i<B;i++){
//             par->addr[i] = par->addr[i+1];
//         }
//         //num<<endl;
//         par->addr[B]=NULL;
//         Node * newnode = new Node(B,0);
//         int k=0;
//         for(int i=0;i<B;i++){
//             if(t->ele[i]==0){
//                 break;
//             }
//             newnode->ele[k]= t->ele[i];
//             k++;
//         }
//         //cout<<num<<endl;
//         newnode->ele[k] = num;
//         k++;
//         for(int i=0;i<B;i++){
//             if(rsibling->ele[i]==0){
//                 break;
//             }
//             newnode->ele[k]= rsibling->ele[i];
//             k++;
//         }
//        // cout<<num<<endl;
//         k=0;
//         for(int i=0;i<=B;i++){
//             if(t->addr[i]==NULL){
//                 break;
//             }
//             newnode->addr[k]= t->addr[i];
//             k++;
//         }
//         for(int i=0;i<=B;i++){
//             if(rsibling->addr[i]==NULL){
//                 break;
//             }
//             newnode->addr[k]= rsibling->addr[i];
//             k++;
//         }
//         par->addr[v[1]] = newnode;
//         return newnode;
//     }
//     Node * merge_left( Node *par, Node *t, std::vector<u_int64_t> v){
//         uint64_t num = par->ele[v[1]];
//         Node * lsibling = par->addr[v[1]];
//         for(int i=v[1];i<B-1;i++){
//                 par->ele[i] = par->ele[i+1];
//         }
//         par->ele[B-1]=0;
//         for(int i=v[1];i<B;i++){
//                 par->addr[i] = par->addr[i+1];
//         }
//         par->addr[B]= NULL;
//         Node * newnode = new Node(B, 0);
//         int k=0;
//         for(int i=0;i<B;i++){
//             if(lsibling->ele[i]==0){
//                 break;
//             }
//             newnode->ele[k]= lsibling->ele[i];
//             k++;
//         }
//         newnode->ele[k] = num;
//         k++;
//         for(int i=0;i<B;i++){
//                 if(t->ele[i]==0){
//                     break;
//                 }
//                 newnode->ele[k]= t->ele[i];
//                 k++;
//         }
//         k =0;
//         for(int i=0;i<=B;i++){
//             if(lsibling->addr[i]==NULL){
//                 break;
//             }
//             newnode->addr[k]= lsibling->addr[i];
//             k++;
//         }
//         for(int i=0;i<=B;i++){
//             if(t->addr[i]==NULL){
//                 break;
//             }
//             newnode->addr[k]= t->addr[i];
//             k++;
//         }
//         par->addr[v[1]] = newnode;
//         return newnode;

//     }
//     void borrow( Node *par, Node *t, std::vector<u_int64_t> v){
//         if (v[3] == 0){
//             uint64_t num1= par->ele[v[1]];
//             par->ele[v[1]] = par->addr[v[1]]->ele[v[2]-1]; // changing the parent element to be sibling's last element
//             Node * rchild = par->addr[v[1]]->addr[v[2]]; // getting the last child of sibling
            
//             par->addr[v[1]]->ele[v[2]-1]=0;
//             par->addr[v[1]]->addr[v[2]] =NULL;
//             for(int i=B-2;i>=0;i--){
//                 t->ele[i+1]= t->ele[i];
//             }
//             for(int i=B-1;i>=0;i--){
//                 t->addr[i+1]= t->addr[i];
//             }
//             t->ele[0]= num1;
//             t->addr[0] = rchild;
//         }
//         else{
//             int num1=par->ele[v[1]];
//             par->ele[v[1]] = par->addr[v[1]+1]->ele[0];
//             Node * lchild = par->addr[v[1]+1]->addr[0];
//             Node *sibling= par->addr[v[1]+1];
//             for(int i=0;i<B-1;i++){
//                 sibling->ele[i]= sibling->ele[i+1];
//             }
//             for(int i=0;i<B;i++){
//                 sibling->addr[i]= sibling->addr[i+1];
//             }
//             sibling->addr[B]=NULL;
//             sibling->ele[B-1]=0;
//             t->ele[B/2-1] = num1;
//             t->addr[B/2] =lchild;
//         }
//     }
//     uint64_t checksize(Node *t){
        
//         uint64_t i =0;
//         while(i < B && t->ele[i] !=0){
//             i++;
//         }
//         return i;
//     }
//     std::vector<uint64_t> canBorrow(Node *par, Node *child){
//         uint64_t ind = 0;
//         for (int i=0; i<=B; i++){
//             if (par->addr[i] == child){
//                 ind = i;
//                 break;

//             }
            
//         }
        
//         if (ind == 0 ){
//             uint64_t rightsize = checksize(par->addr[ind + 1] );
//             if (rightsize >= B/2){
//                 return {1,ind, rightsize,1};
//             }
//             else{
//                 return {0, ind, rightsize,1};
//             }
//         }
//         else if(ind==B || par->ele[ind]==0){
//             uint64_t leftsize = checksize(par->addr[ind -1] );
//             if (leftsize >= B/2){
//                 return {1,ind -1, leftsize,0};
//             }
//             else{
//                 return {0, ind -1, leftsize,0};
//             }
//         }
//         else{
//             //if (ind !=0 && ind != B && par->addr[ind -1] !=NULL &&par->addr[ind + 1]!=NULL){
//             uint64_t leftsize = checksize(par->addr[ind -1] );
//             uint64_t rightsize = checksize(par->addr[ind + 1] );
//             if (leftsize >= B/2){
//                 return {1,ind -1, leftsize,0};
//             }
//             else if (rightsize >= B/2){
//                 return {1,ind, rightsize, 1};
//             }
//             else{
//                 return {0, ind -1, leftsize, 0};
//             }

        
//         }
        
//     }
//     std::vector<uint64_t> rangesearch(uint64_t x, uint64_t y){
//             int cpuid = sched_getcpu();
//             Node * t = root;
//             std::vector<uint64_t> v;
//             read_lock(root->lock, cpuid);
//             while(!t->isleaf){
//                 t= gotochild(t, x);
//             }
            
//             int i = 0;
//             for(i=0;i<= B-1; i++ ){
//                 if (t->ele[i] < x)
//                     continue;
//                 break;
//             }
//             if(i==B){
//                 read_unlock(t->lock, cpuid);
//                 return v;
//             }

//             while(t!=NULL&&t->ele[i] <= y){
//                 v.push_back(t->ele[i]);
//                 i++;
//                 if (i> B-1 || t->ele[i] == 0){
//                     read_unlock(t->lock, cpuid);
//                     t = t->addr[1];
//                     if(t!=NULL)
//                         read_lock(t->lock, cpuid);
//                     i = 0;
//                 }
//             }
//             if (t != NULL)
//                 read_unlock(t->lock, cpuid);
//             return v;   
//     }
//     void insert_occ(uint64_t x, uint64_t y){
//         int cpuid = sched_getcpu();    
//         Node * t= root;
//         if(t->isleaf)
//             write_lock(t->lock);
//         else
//             read_lock(t->lock, cpuid);
//         while(!t->isleaf){   
//             Node * temp= gotochild2(t, x);
//             read_unlock(t->lock, cpuid);
//             if(temp->isleaf)
//                 write_lock(temp->lock);
//             else
//                 read_lock(temp->lock,cpuid);
//             t=temp;
//         }
//         if(t->ele[B-1]==0){
//             t->insertInLNode(x,y);
//             write_unlock(t->lock);
//         }else{
//             write_unlock(t->lock);
//             insert1(x,y);
//         }
//     }
//     void remove_occ(uint64_t x){
//         int cpuid = sched_getcpu();

//         Node * t= root;
//         if(t->isleaf)
//             write_lock(t->lock);
//         else
//             read_lock(t->lock, cpuid);
//         while(!t->isleaf){   
//             Node * temp= gotochild2(t, x);
//             read_unlock(t->lock, cpuid);
//             if(temp->isleaf)
//                 write_lock(temp->lock);
//             else
//                 read_lock(temp->lock, cpuid);
//             t=temp;
//         }
//         if(t->ele[1]==0 && t->ele[0]==x){
//             write_unlock(t->lock);
//             remove(x);
//         }else{
//             t->deleteLNode(t, x);
//             write_unlock(t->lock);
//         }
//     }

//     };
    

//     /**
//      * BPlusTreeDram::init -- initialize bplus tree
//      */
//     int BPlusTreeOCC::init(const char *path) {
//         B = BPLUSTREE_DEGREE;
//         root = NULL;
//         // insert1(1000,1000);
//         return 1;
//     }

//     /**
//      * BPlusTreeDram::scan -- perform range query
//      */
//     int BPlusTreeOCC::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
//         rangesearch(key, key + len);
//         return 1;
//     }

//     /**
//      * BPlusTreeDram::read -- read 'value' of 'key' from btree and place it into '&result'
//      */
//     int BPlusTreeOCC::read(const uint64_t key, void *&result) {
//         bool res = query(key);
//         return 1;
//     }

//     /**
//      * BPlusTreeDram::update -- update 'value' of 'key' into btree, will insert the 'value' if 'key' not exists
//      */
//     int BPlusTreeOCC::update(const uint64_t key, void *value) {
//         remove_occ(key);
//         return 1;
//     }

//     /**
//      * BPlusTreeDram::insert -- inserts <key, value> pair into bplus tree, will update the 'value' if 'key' already exists
//      */
//     int BPlusTreeOCC::insert(const uint64_t key, void *value) {
//         insert_occ(key, key);
        
//         return 1;
//     }

// }   //ycsbc