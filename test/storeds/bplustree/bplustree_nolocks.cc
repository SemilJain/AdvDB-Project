#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <iostream>
#include<vector>
#include<queue>
// #include "lock.h"
#include <sched.h>

#include "bplustree_common.h"

namespace ycsbc {
    class Node{
    public: 
    std::vector<uint64_t> ele;
    std::vector<Node *> addr;
    std::vector<uint64_t> val;
    uint64_t B;
    bool isleaf;
    Node(int size, bool b){
        ele.resize(size,0);
        addr.resize(size+1, NULL);
        val.resize(size,0);
        isleaf= b;
        B= size;
    }
    Node(Node * cn){
        ele.resize(cn->B,0);
        addr.resize((cn->B)+1, NULL);
        val.resize(cn->B,0);
        isleaf= cn->isleaf;
        B = cn->B;

        for(int i=0;i<B;i++){
            ele[i] = cn->ele[i];
            addr[i] = cn->addr[i];
            val[i] = cn->val[i];
        }
        addr[B] = cn->addr[B];
    }
    void insertInLNode(uint64_t x, uint64_t y){
        for(int i=B-1;i>=0;i--){
            if(ele[i] == 0){continue;}
            if(x < ele[i]){
                ele[i+1]= ele[i];
                val[i+1] = val[i];}
            else{
                ele[i+1]= x;
                val[i+1]= y;
                return ;
                break;
            }
        }
        ele[0] = x;
        val[0] = y;
    }
    void insertInINode(Node * q, uint64_t x , Node *original){
        int ind;
        for(int k=0;k<=B;k++){
            if(addr[k]==original){
                ind=k;
                break;
            }
            else{
                continue;
            }
        }
        for(int k= B-2;k>=ind;k--){
            ele[k+1]= ele[k];
        }
        for(int k= B-1;k>ind;k--){
            addr[k+1]= addr[k];
        }
        ele[ind] = x;
        addr[ind+1]= q; 
    }
    void splitRLNode1(){
        Node * t1= new Node(B, 1); 
        Node * t2= new Node(B, 1);
        
        int j=0;
        for(int k=0;k<B/2;k++){
            t1->ele[j] =ele[k];
            t1->val[j] =val[k];
            ele[k]=0;
            val[k]=0;
            j++;  
        }
        j=0;
        for(int k=B/2;k<B;k++){
            t2->ele[j] =ele[k];
            t2->val[j] =val[k];
            ele[k]=0;
            val[k]=0;
            j++;  
        } 
        ele[0]= t2->ele[0];
        addr[0]=t1;
        addr[1]= t2;
        isleaf=0;
        t1->addr[1] = t2;
        t2->addr[0]= t1;
        
    }
    Node * splitRINode(){
       Node * t1= new Node(B, 0); 
       Node * t2= new Node(B, 0);
        
       int j=0;
        for(int k=0;k<B/2-1;k++){
            t1->ele[j] =ele[k];
           // t1->val[j] =val[k];
            ele[k]=0;
            //val[k]=0;
            j++;  
        }
        j=0;
        uint64_t element = ele[B/2-1];
        ele[B/2-1]=0;
        for(int k=B/2;k<B;k++){
            t2->ele[j] =ele[k];
            ele[k]=0;
            j++;  
        }  
        j=0;
        for(int i=0;i<=B/2-1;i++){
            t1->addr[j]= addr[i];
            j++;
            addr[i]=NULL;
        }
        j=0;
        for(int i=B/2;i<=B;i++){
            t2->addr[j]= addr[i];
            j++;
            addr[i]=NULL;
        }
        ele[0]= element;
        addr[0]= t1;
        addr[1]= t2;
    }
    Node * splitRLNode(){
        Node * t= new Node(B, 1); 
        Node * newroot= new Node(B,0);
        int j=0;
        for(int k=B/2;k<B;k++){
            t->ele[j] =ele[k];
            t->val[j] =val[k];
            ele[k]=0;
            val[k]=0;
            j++;  
        } 
        this->addr[1] = t;
        t->addr[0] = this;
        newroot->ele[0] = t->ele[0];
        newroot->addr[0] = this;
        newroot->addr[1] =  t;
        return newroot;
    }
    
    std::pair<Node *, uint64_t> splitINode(){
        Node * t= new Node(B, 0); 
        int j=0;
        Node *addr1, *addr2;
        addr1 = this;
        addr2 = t;
        uint64_t element = ele[B/2];
        ele[B/2] = 0;

        for(int k=B/2+1;k<B;k++){
            t->ele[j] = ele[k];
            t->addr[j]= addr[k];
            ele[k]=0;
            addr[k] =0;
            j++;  
        } 
        t->addr[j] = addr[B];
        addr[B]=0; 
        return {t, element};
    }
    std::pair<Node *, uint64_t> splitLNode(){
        Node * t= new Node(B, 1); 
        int j=0;
        for(int k=B/2;k<B;k++){
            t->ele[j] =ele[k];
            t->val[j] =val[k];
            ele[k]=0;
            val[k]=0;
            j++;  
        } 
        uint64_t element =  t->ele[0];
        t->addr[1] = this->addr[1];
        this->addr[1] = t;
        t->addr[0] = this;
        if(t->addr[1]!=NULL)
            t->addr[1]->addr[0]= t;

        return {t, element};
    }
    
};

    class BPlusTreeNoLock : public StoredsBase {
    public:
        BPlusTreeNoLock(const char *path) {
            BPlusTreeNoLock::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        // void destroy();

        ~BPlusTreeNoLock();

        Node *root;
        uint64_t B;

        void insert1(uint64_t x, uint64_t y, Node *croot){
        Node *t = croot;
        Node * par =NULL;
        if(croot == NULL){
            Node  * temp = new Node(B, 1);
            temp->ele[0] = x;
            temp->val[0] = y;
            croot= temp;
        }else if(croot->isleaf == 1){
            if(t->ele[B-1] == 0){
                t->insertInLNode( x, y);
            }
            else{
                
                t->splitRLNode1();
                
                if(x >= croot->ele[0])t= croot->addr[1];
                else  t= croot->addr[0];
                t->insertInLNode(x,y);
            }
        }
        else{
            while(!t->isleaf){
                if(t->ele[B-1] == 0){
                    par= t;
                    t= gotochild(t, x);
                }else{
                    if(t==croot){
                        t->splitRINode();
                        par= t;
                        t= gotochild(t, x);
                        // pair<Node *, uint64_t> p = t->splitINode();
                        // Node *newroot= new Node(B, 0);
                        // newroot->ele[0]= p.second;
                        // newroot->addr[0] = t;
                        // newroot->addr[1]= p.first;
                        // if(x >= p.second)t= p.first;
                        // root = newroot;
                        // par= t;
                        // t= gotochild(t, x);
                    }else{
                        std::pair<Node *, uint64_t> p = t->splitINode();
                        par->insertInINode(p.first, p.second, t);
                        if(x >= p.second)t= p.first;
                        par= t;
                        t= gotochild(t, x);
                    }
                }
            }
            if(t->isleaf){
                if(t->ele[B-1]==0){
                    t->insertInLNode(x,y);
                }
                else{
                    
                    std::pair<Node *, uint64_t> p = t->splitLNode();
                    
                    par->insertInINode(p.first, p.second, t);
                    if(x >= p.second)t= p.first;
                    t->insertInLNode(x,y);
                }
            }
        }
    }
    Node * gotochild(Node * t, uint64_t x){
        for(int i=0;i<B;i++){
            if(t->ele[i]==0){
                return t->addr[i];
            }
            if(x< t->ele[i]){
                t= t->addr[i];
                return t; 
            }
        }
        t= t->addr[B];
        return t;
    }
    bool query(uint64_t x){
        Node * t= root;
        while(!t->isleaf){
            t= gotochild(t, x);
        }
        for (int i=0; i<B; i++){
            if (t->ele[i] == x){
                return 1;
            }
        }
        return 0;
    }
    // void print(){
    //     queue<Node *> q;
    //     q.push(root);
    //     cout<<root<<" ";
    //     while(!q.empty()){
    //         Node *t = q.front();
    //         q.pop();
    //         cout<<t->isleaf<<endl;
    //         cout<<t<<endl;
    //         for(int i=0;i<B;i++){
    //             cout<<t->ele[i]<<" ";
    //         }
    //         cout<<endl;
    //         for(int i=0;i<B;i++){
    //             cout<<t->val[i]<<" ";
    //         }
    //         cout<<endl;
    //         for(int i=0;i<=B;i++){
    //             if(!t->isleaf && t->addr[i]!=NULL)
    //                 q.push(t->addr[i]);
    //             cout<<t->addr[i]<<" ";
    //         }
    //         cout<<endl;
    //         cout<<endl;

    //     }
    // }


   void remove(int x, Node * croot){
        //cout<<"dsds";
        Node * t = croot;
        Node *par = NULL;
        while(!t->isleaf){
            if (t == croot){
                par = t;
                t = gotochild(t,x);
            }
            else{
                if(t->ele[B/2-1] == 0 ){
                    
                    std::vector<uint64_t> v = canBorrow(par, t);
                    if (v[0] == 0){
                        // merge function
                        if (v[3]==0){
                            t=merge_left(par, t,v);
                        }
                        else{
                            t=merge_right(par, t,v);
                        }
                        if(par== croot &&croot->ele[0]==0){
                            Node * n= croot->addr[0];
                            for(int i=0;i<B;i++){
                                croot->ele[i] = n->ele[i];
                                croot->addr[i] = n->addr[i];
                            }
                            croot->addr[B] =n->addr[B];
                            par= croot;
                            //root= root->addr[0];
                        }
                        else
                        par = t;
                        t = gotochild(t,x);
                    }
                    else{
                        // borrow function
                        borrow(par, t, v);
                        par = t;
                        t = gotochild(t,x);
                    }
                }
                else{
                    par = t;
                    t = gotochild(t,x);
                }
            }
        }
        uint64_t ind = 0;
        for(uint64_t i =0; i<B;i++){
            if (t->ele[i] == x){
                ind = i;
                break;
            } 
        }
        for (uint64_t i = ind; i < B-1; i++){
            t->ele[i] = t->ele[i+1];
            t->val[i] = t->val[i+1];
        }
        t->ele[B-1]=0;
        t->val[B-1]=0;
        if(t==croot){
            
        }else{
            if(t->ele[0] == 0 ){
                delParEle(par, t);
            }
        
            if(croot->ele[0] == 0){
                Node * n= croot->addr[0];
                for(int i=0;i<B;i++){
                    croot->ele[i] = n->ele[i];
                    croot->val[i] = n->val[i];
                }
                croot->addr[0] =NULL;
                croot->isleaf = 1;
            }
        }
    }
    void delParEle(Node *par, Node *t){
        uint64_t ind =0;
        for(int i=0;i<=B;i++){
            if(par->addr[i] ==  t){
                ind= i;
                break;
            }
        }
        for(int i=ind;i<B;i++){
            par->addr[i]= par->addr[i+1];
        }
        par->addr[B]=NULL;
        if(par->ele[ind] == 0)ind= ind-1;
        for(int i=ind;i<B-1;i++){
            par->ele[i]= par->ele[i+1];
        }
        par->ele[B-1]=0;

    }
    Node* merge_right( Node *par, Node *t, std::vector<u_int64_t> v){
        uint64_t num = par->ele[v[1]];
        //cout<<num<<endl;
        Node * rsibling = par->addr[v[1]+1];
        for(int i=v[1];i<B-1;i++){
            par->ele[i] = par->ele[i+1];
        }
        par->ele[B-1] =0;
        for(int i=v[1];i<B;i++){
            par->addr[i] = par->addr[i+1];
        }
        //num<<endl;
        par->addr[B]=NULL;
        Node * newnode = new Node(B,0);
        int k=0;
        for(int i=0;i<B;i++){
            if(t->ele[i]==0){
                break;
            }
            newnode->ele[k]= t->ele[i];
            k++;
        }
        //cout<<num<<endl;
        newnode->ele[k] = num;
        k++;
        for(int i=0;i<B;i++){
            if(rsibling->ele[i]==0){
                break;
            }
            newnode->ele[k]= rsibling->ele[i];
            k++;
        }
       // cout<<num<<endl;
        k=0;
        for(int i=0;i<=B;i++){
            if(t->addr[i]==NULL){
                break;
            }
            newnode->addr[k]= t->addr[i];
            k++;
        }
        for(int i=0;i<=B;i++){
            if(rsibling->addr[i]==NULL){
                break;
            }
            newnode->addr[k]= rsibling->addr[i];
            k++;
        }
        par->addr[v[1]] = newnode;
        return newnode;
    }
    Node * merge_left( Node *par, Node *t, std::vector<u_int64_t> v){
        uint64_t num = par->ele[v[1]];
        Node * lsibling = par->addr[v[1]];
        for(int i=v[1];i<B-1;i++){
                par->ele[i] = par->ele[i+1];
        }
        par->ele[B-1]=0;
        for(int i=v[1];i<B;i++){
                par->addr[i] = par->addr[i+1];
        }
        par->addr[B]= NULL;
        Node * newnode = new Node(B, 0);
        int k=0;
        for(int i=0;i<B;i++){
            if(lsibling->ele[i]==0){
                break;
            }
            newnode->ele[k]= lsibling->ele[i];
            k++;
        }
        newnode->ele[k] = num;
        k++;
        for(int i=0;i<B;i++){
                if(t->ele[i]==0){
                    break;
                }
                newnode->ele[k]= t->ele[i];
                k++;
        }
        k =0;
        for(int i=0;i<=B;i++){
            if(lsibling->addr[i]==NULL){
                break;
            }
            newnode->addr[k]= lsibling->addr[i];
            k++;
        }
        for(int i=0;i<=B;i++){
            if(t->addr[i]==NULL){
                break;
            }
            newnode->addr[k]= t->addr[i];
            k++;
        }
        par->addr[v[1]] = newnode;
        return newnode;

    }
    void borrow( Node *par, Node *t, std::vector<u_int64_t> v){
        if (v[3] == 0){
            uint64_t num1= par->ele[v[1]];
            par->ele[v[1]] = par->addr[v[1]]->ele[v[2]-1]; // changing the parent element to be sibling's last element
            Node * rchild = par->addr[v[1]]->addr[v[2]]; // getting the last child of sibling
            
            par->addr[v[1]]->ele[v[2]-1]=0;
            par->addr[v[1]]->addr[v[2]] =NULL;
            for(int i=B-2;i>=0;i--){
                t->ele[i+1]= t->ele[i];
            }
            for(int i=B-1;i>=0;i--){
                t->addr[i+1]= t->addr[i];
            }
            t->ele[0]= num1;
            t->addr[0] = rchild;
        }
        else{
            int num1=par->ele[v[1]];
            par->ele[v[1]] = par->addr[v[1]+1]->ele[0];
            Node * lchild = par->addr[v[1]+1]->addr[0];
            Node *sibling= par->addr[v[1]+1];
            for(int i=0;i<B-1;i++){
                sibling->ele[i]= sibling->ele[i+1];
            }
            for(int i=0;i<B;i++){
                sibling->addr[i]= sibling->addr[i+1];
            }
            sibling->addr[B]=NULL;
            sibling->ele[B-1]=0;
            t->ele[B/2-1] = num1;
            t->addr[B/2] =lchild;
        }
    }
    uint64_t checksize(Node *t){
        
        uint64_t i =0;
        while(i < B && t->ele[i] !=0){
            i++;
        }
        return i;
    }
    std::vector<uint64_t> canBorrow(Node *par, Node *child){
        uint64_t ind = 0;
        for (int i=0; i<=B; i++){
            if (par->addr[i] == child){
                ind = i;
                break;

            }
            
        }
        
        if (ind == 0 ){
            uint64_t rightsize = checksize(par->addr[ind + 1] );
            if (rightsize >= B/2){
                return {1,ind, rightsize,1};
            }
            else{
                return {0, ind, rightsize,1};
            }
        }
        else if(ind==B || par->ele[ind]==0){
            uint64_t leftsize = checksize(par->addr[ind -1] );
            if (leftsize >= B/2){
                return {1,ind -1, leftsize,0};
            }
            else{
                return {0, ind -1, leftsize,0};
            }
        }
        else{
            //if (ind !=0 && ind != B && par->addr[ind -1] !=NULL &&par->addr[ind + 1]!=NULL){
            uint64_t leftsize = checksize(par->addr[ind -1] );
            uint64_t rightsize = checksize(par->addr[ind + 1] );
            if (leftsize >= B/2){
                return {1,ind -1, leftsize,0};
            }
            else if (rightsize >= B/2){
                return {1,ind, rightsize, 1};
            }
            else{
                return {0, ind -1, leftsize, 0};
            }

        
        }
        
    }
    std::vector<uint64_t> rangesearch(uint64_t x, uint64_t y){
            Node * t = root;
            std::vector<uint64_t> v;
            while(!t->isleaf){
                t= gotochild(t, x);
            }
            
            int i = 0;
            for(i=0;i<= B-1; i++ ){
                if (t->ele[i] < x)
                    continue;
                break;
            }
            while(t!=NULL&&t->ele[i] <= y){
                v.push_back(t->ele[i]);
                i++;
                if (i> B-1 || t->ele[i] == 0){
                        t = t->addr[1];
                        i = 0;
                }
            }
            return v;   
    }


 
    //Insert with no lock but yes concurrency
    std::pair<Node *, uint64_t> gotochild_NL(Node * t, uint64_t x){
        for(int i=0;i<B;i++){
            if(t->ele[i]==0){
                return {t->addr[i],i};
            }
            if(x< t->ele[i]){
                t= t->addr[i];
                return {t,i}; 
            }
        }
        t= t->addr[B];
        return {t,B};
    }
    void insert_NL(uint64_t x,uint64_t y){
        while(1){
            Node * t =  root, *par =NULL;
            Node * ori_root = t;
            Node *croot = new Node(t);
            t= croot;
            while(!t->isleaf){
                par = t;
                std::pair<Node *, uint64_t> p=gotochild_NL(t, x);
                t= p.first;
                Node *nt = new Node(t); 
                t= nt;
                par->addr[p.second] = nt;
            }
            insert1(x,y,croot);
            if(ori_root == root){
                root= croot;
                break;
            }
        }
    }
    void remove_NL(uint64_t x){
        while(1){
            Node * t =  root, *par =NULL;
            Node * ori_root = t;
            Node *croot = new Node(t);
            t= croot;
            while(!t->isleaf){
                par = t;
                std::pair<Node *, uint64_t> p=gotochild_NL(t, x);
                t= p.first;
                Node *nt = new Node(t); 
                t= nt;
                par->addr[p.second] = nt;
            }

            remove(x,croot);
            if(ori_root == root){
                root= croot;
                break;
            }
        }
    }

    };
    

    /**
     * BPlusTreeDram::init -- initialize bplus tree
     */
    int BPlusTreeNoLock::init(const char *path) {
        root = new Node(BPLUSTREE_DEGREE,1);
        B = BPLUSTREE_DEGREE;
        return 1;
    }

    /**
     * BPlusTreeDram::scan -- perform range query
     */
    int BPlusTreeNoLock::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        rangesearch(key, key + len);
        return 1;
    }

    /**
     * BPlusTreeDram::read -- read 'value' of 'key' from btree and place it into '&result'
     */
    int BPlusTreeNoLock::read(const uint64_t key, void *&result) {
        bool res = query(key);
        result = (void *) res;
        return 1;
    }

    /**
     * BPlusTreeDram::update -- update 'value' of 'key' into btree, will insert the 'value' if 'key' not exists
     */
    int BPlusTreeNoLock::update(const uint64_t key, void *value) {
        remove_NL(key);
        return 1;
    }

    /**
     * BPlusTreeDram::insert -- inserts <key, value> pair into bplus tree, will update the 'value' if 'key' already exists
     */
    int BPlusTreeNoLock::insert(const uint64_t key, void *value) {
        insert_NL(key, key);
        
        return 1;
    }

}   //ycsbc