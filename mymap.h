#pragma once

#include <iostream>
#include <sstream>
#include <vector>


using namespace std;

template<typename keyType, typename valueType>
class mymap {
 private:
    struct NODE {
        keyType key;  // used to build BST
        valueType value;  // stored data for the map
        NODE* left;  // links to left child
        NODE* right;  // links to right child
        int nL;  // number of nodes in left subtree
        int nR;  // number of nodes in right subtree
        bool isThreaded;
    };
    NODE* root;  // pointer to root node of the BST
    int size;  // # of key/value pairs in the mymap

    //
    // iterator:
    // This iterator is used so that mymap will work with a foreach loop.
    //
    struct iterator {
     private:
        NODE* curr;  // points to current in-order node for begin/end

     public:
        iterator(NODE* node) {
            curr = node;
        }

        keyType operator *() {
            return curr -> key;
        }

        bool operator ==(const iterator& rhs) {
            return curr == rhs.curr;
        }

        bool operator !=(const iterator& rhs) {
            return curr != rhs.curr;
        }

        bool isDefault() {
            return !curr;
        }

        //
        // operator++:
        //
        // This function should advance curr to the next in-order node.
        // O(logN)
        //
        iterator operator++() {
            if (curr->isThreaded) {
                curr = curr->right;
            } else{
                curr = curr->right;
                while (curr->left != nullptr) {
                    curr=curr->left;
                }
            }
            return iterator(curr);
        }
    };


 public:
    /* mymap
     *
     * sets the root pointer to null and size to zero
     */
    mymap() {
        root = nullptr;
        size = 0;
    }


    /* copytree
     *
     * Helper function Copies over the tree from the parameter by using preorder
     * traversal. Uses recursion and returns the new node added.
     */
    NODE* copytree(NODE* other){
        if (other == nullptr) {
            return nullptr;
        } else{
            NODE* newnode = new NODE();
            newnode->key = other->key;
            newnode->value = other->value;
            newnode->isThreaded = other->isThreaded;
            newnode->nL = other->nL;
            newnode->nR = other->nR;

            if (newnode->isThreaded) {
                newnode->right = other->right;
                newnode->left = other->left;
            } else {
                newnode->left = copytree(other->left);
                newnode->right = copytree(other->right);
            }
            return newnode;
        }
    }


    /* mymap copy constructor
     *
     * Calls the recusive helper function and sets the size value
     */
    mymap(const mymap& other) {
        root = copytree(other.root);
        size = other.size;
    }
        
    
    /* mymap copy operator
     *
     * First calls the clear tree functionCalls the recusive helper function
     * and sets the size value and returns the current pointer
     */
    mymap& operator=(const mymap& other) {
        cleartree(root);
        size = other.size;
        root = copytree(other.root);
        return *this;
    }


    /* cleartree
     *
     * deletes all the nodes using postorder recursive traversal
     */
    void cleartree(NODE* curr){
        if (curr == nullptr) {
            delete curr;
            return;
        }

        if (curr->isThreaded) {
            delete curr;
            return;
        } else {
            cleartree(curr->left);
            cleartree(curr->right);
            delete curr;
        }
    }


    /* clear
     *
     * Calls the clear tree helper functon and sets the root and size to defult
     * values
     */
    void clear() {
        cleartree(root);
        root = nullptr;
        size = 0;
    }

    /* ~mymap
     *
     * Calls the clear tree helper functon and sets the root and size to defult
     * values
     */
    ~mymap() {
        cleartree(root);
        root = nullptr;
        size = 0;
    }


    /* isbalanced
     *
     * Checks to see if the current node is balanced by using a algorithm, return
     * true if it is and false if it is not.
     */
    bool isbalanced(NODE* node){
        if (!(max(node->nL, node->nR) <= 2 * min(node->nL, node->nR) + 1)) {
            return false;
        }
        return true;
    }


    /* createNewNode
     *
     * Helper function to create a new node and add it to the tree
     */
    void createNewNode(keyType key, valueType value, NODE* prev){
        NODE* newnode = new NODE();
        newnode->left = nullptr;
        newnode->right = nullptr;
        newnode->key = key;
        newnode->value = value;
        newnode->isThreaded=true;
        newnode->nL = 0;
        newnode->nR = 0;

        if(prev == nullptr){
            root = newnode;
        }
        else if(key < prev->key){
            newnode->right = prev;
            prev->left = newnode;
        }
        else if(key>prev->key){
            newnode->right = prev->right;
            prev->isThreaded = false;
            prev->right = newnode;
        }
        changeNumNodes(key);
        size++;
    }


    /* rebalanceTree
     *
     * Helper function to rebalance tree. Adds all the nodes to be balanced to a vector
     * and calls the recusive rebalance function
     */
    void rebalanceTree(keyType key, vector<NODE*> toBeBalanced, vector<NODE*> allNodes){
        NODE* temp = searchforkey(key);
        
        if(temp!=nullptr){
            NODE* parenttemp = searchforParentkey(key);
            addtovector(temp, toBeBalanced);
            addentire(root, allNodes);

            NODE* newsubtreeroot =  _rebalance(0, int(toBeBalanced.size()-1), toBeBalanced, allNodes, parenttemp);

            if(parenttemp==nullptr){
                root = newsubtreeroot;
            }
            else{
                if(newsubtreeroot->key < parenttemp->key){
                    parenttemp->left = newsubtreeroot;

                }
                else{
                    parenttemp->right = newsubtreeroot;
                }
            }
        }
    }


    /* put
     *
     * This is the main put function that calls other helper function.
     */
    void put(keyType key, valueType value) {
        NODE* curr = root;
        NODE* prev = nullptr;
        vector<NODE*> toBeBalanced;
        vector<NODE*> allNodes;

        while (curr!=nullptr) {
            if(key == curr->key){
                curr->value = value;
                return;
            } else if (key < curr->key) {
                prev = curr;
                curr = curr->left;
            } else {
                if (curr->isThreaded) {
                    prev = curr;
                    curr = nullptr;
                  } else {
                      prev = curr;
                      curr = curr->right;
                  }
            }
        }
        createNewNode(key, value, prev);
        rebalanceTree(key, toBeBalanced, allNodes);
    }


    /* changeNumNodes
     *
     * Changes the count for the number of left and right nodes for the necessary nodes
     * does this by finding the inserted noded and increamenting as it goes along
     */
    void changeNumNodes(keyType key){
        NODE* curr = root;
        while (curr != nullptr) {
            if (key == curr->key) {
                return ;
            } else if (key<curr->key) {
                curr->nL++;
                curr = curr->left;
            } else {
                if (!curr->isThreaded) {
                    curr->nR++;
                    curr = curr->right;
                }
            }
        }
    }


    /* searchforkey
     *
     * Searches for a key using pre order traversal
     */
    NODE* searchforkey(keyType key){
        NODE* curr = root;
        while (curr != nullptr) {
            if (!isbalanced(curr)) {
                return curr;
            } else {
                if (key == curr->key) {
                    return nullptr;
                } else if (key<curr->key) {
                    curr = curr->left;
                } else{
                    if (!curr->isThreaded) {
                        curr = curr->right;
                    }
                }
            }
        }
        return nullptr;
    }
    
    /* searchforParentkey
     *
     * Searches for a parent node using pre order traversal
     */
    NODE* searchforParentkey(keyType key){
        NODE* curr = root;
        NODE* prev = nullptr;

        while (curr != nullptr) {
            if (!isbalanced(curr)) {
                return prev;
            } else {
                if (key == curr->key) {
                    prev = curr;
                    return nullptr;
                } else if (key<curr->key) {
                    prev = curr;
                    curr = curr->left;
                } else {
                    if (!curr->isThreaded) {
                        prev = curr;
                        curr = curr->right;
                    }
                }
            }
        }
        return nullptr;
    }
    
    
    /* _rebalance
     *
     * This is a recursive function that uses post order to rebalance the tree from the
     * vector of nodes.
     */
    NODE* _rebalance(int left, int right, vector<NODE*>& tobebalanced, vector<NODE*>& allNodes, NODE* prev){
        if (left>right) { return nullptr; }
        
        int mid = (left + right) / 2;
        NODE* newroot = tobebalanced[mid];
        newroot->left = _rebalance(left, mid-1, tobebalanced, allNodes, prev);
        newroot->nL = mid-left;
        newroot->right = _rebalance(mid+1, right, tobebalanced, allNodes, prev);
        newroot->nR = right-mid;
        
        if (newroot->right == nullptr) {
            if (mid+1 < tobebalanced.size()) {
                newroot->right = tobebalanced[mid+1];
                newroot->isThreaded = true;
            } else {
                if (allNodes[allNodes.size()-1]->key == newroot->key) {
                    newroot->right = nullptr;
                } else {
                    for(int i = 0; i < allNodes.size(); i++){
                        if(allNodes[i]->key == newroot->key){
                            newroot->right = allNodes[i+1];
                        }
                    }
                }
                newroot->isThreaded = true;
            }
        } else {
            newroot->isThreaded = false;
        }
        return newroot;
    }


    /* addtovector
     *
     * This function gets all the nodes that needs to be added based on the root
     * parameter given
     */
    void addtovector(NODE* imbalanced, vector<NODE*>& toBeBalanced){
        if (imbalanced == NULL) {return;}
        imbalanced->nL = 0;
        imbalanced->nR = 0;

        addtovector(imbalanced->left, toBeBalanced);
        toBeBalanced.push_back(imbalanced);
        if (!imbalanced->isThreaded) {
            addtovector(imbalanced->right, toBeBalanced);
        }
    }

    /* addentire
     *
     * This function adds all the nodes into a vector
     */
    void addentire(NODE* imbalanced, vector<NODE*>& toBeBalanced){
        if (imbalanced == NULL){return;}

        addentire(imbalanced->left, toBeBalanced);
        toBeBalanced.push_back(imbalanced);
        if (!imbalanced->isThreaded) {
            addentire(imbalanced->right, toBeBalanced);
        }
    }


    /* contains
     *
     * This function checks to see if the key is in the BST. If it is then
     * return true. If not return false.
     */
    bool contains(keyType key) {
        NODE* curr = root;
        while (curr != nullptr) {
            if (key == curr->key) {
                return true;
            } else if (key < curr->key) {
                curr = curr->left;
            } else {
                if (!curr->isThreaded) {
                    curr = curr->right;
                } else {
                    curr = nullptr;
                }
            }
        }
        return false;
    }


    /* get
     *
     * This function returns the value at a given key. If the key is not int the
     * BST then return the valuetype();
     */
    valueType get(keyType key) {
        NODE* curr = root;
        while (curr != nullptr) {
            if (key == curr->key) {
                return curr->value;
            } else if (key < curr->key) {
                curr = curr->left;
            } else {
                if (!curr->isThreaded) {
                    curr = curr->right;
                } else {
                    curr = nullptr;
                }
            }
        }
        return valueType();
    }

    
    /* operator[]
     *
     * This function returns the value at a given key. If the key does not exist
     * then add it to the BST with val as valuetype()
     */
    valueType operator[](keyType key) {
        NODE* curr = leftMost(root);
        while (curr != nullptr) {
            if (curr->key == key) {
                return curr->value;
            }
            if (curr->isThreaded) {
                curr = curr->right;
            } else {
                curr = leftMost(curr->right);
            }
        }

        put(key, valueType());
        return  valueType();
    }
    
    
    /* Size
     *
     * Returns the size
     */
    int Size() {
        return size;
    }


    /* begin
     *
     * Returns the leftmost node in the BST
     */
    iterator begin() {
        NODE* curr = root;
        while (curr->left != nullptr) {
            curr = curr->left;
        }
        return iterator(curr);  // TODO: Update this return.
    }


    /* end
     *
     * Returns the rightmost node in the BST
     */
    iterator end() {
        NODE* curr = root;
        while (curr->right != nullptr){
            curr = curr->right;
        }
        return iterator(curr);
    }


    /* leftMost
     *
     * Returns the leftMost node in the BST
     */
   NODE* leftMost(NODE* n){
       if (n == nullptr) {
           return nullptr;
       }
       while (n->left != NULL) {
           n = n->left;
       }
        return n;
    }
    
  
    /* _buildString
     *
     * Ouputs the BST as a string using inorder traversal recursion
     */
   void _buildString(NODE* node, ostream& out) {
       if (node == nullptr){
           return;
       }
       _buildString(node->left, out);
       out << "key: " << node->key << " value: " << node->value << "\n";
       if (!node->isThreaded) {
           _buildString(node->right, out);
       }
    }
    
    /* toString
     *
     * calls the helper function to output BST as a string
     */
    string toString() {
        stringstream ss;
        _buildString(root, ss);
        return ss.str();
    }


    /* toVector
     *
     * Converts the BST into a vector of pairs
     */
    vector<pair<keyType, valueType> > toVector() {
        
        NODE* curr = leftMost(root);
        vector<pair<keyType, valueType>> map;
        
        while (curr != nullptr) {
            map.push_back(make_pair(curr->key, curr->value));

            if (curr->isThreaded) {
                curr = curr->right;
            } else {
                curr = leftMost(curr->right);
            }
        }
        return map;
    }


    /* _buildBalanceString
     *
     * Outputs the key and its left and right node counts using preorder traversal
     */
    void _buildBalanceString(NODE* node, ostream& out) {
        if(node == nullptr){
            return;
        }
        out << "key: " << node->key << ", nL: " << node->nL << ", nR: " << node->nR << "\n";
        
        _buildBalanceString(node->left, out);
        if(!node->isThreaded){
            _buildBalanceString(node->right, out);
        }
     }


    /* checkBalance
     *
     * Calls helper function to output the balance count as a string
     */
    string checkBalance() {
        stringstream ss;
        _buildBalanceString(root, ss);
        return ss.str();
    }
};
