
#ifndef SPAN_TREE_HPP
#define SPAN_TREE_HPP


#include "gen_ex.hpp"

/* span_tree is a binary search tree (implemented as AVL tree here) that sorts data that is associated with some value (where the value is a double)

Upon lookup for any value, the span_tree will return the data associated with the next lowest and next highest stored data.

The benefit of a span_tree over just a sorted vector is that the span tree is easily expanded
*/

template<typename DATA_T>
class span_tree
{
private:
    struct node
    {
        double key;
        DATA_T* data;
        unsigned char height;
        node* left;
        node* right;

        node(double k) { key = k; left = right = 0; height = 1; }
        node() { left = right = 0; height = 1; }
    };

    unsigned char height(node* p)
    {
        return p?p->height:0;
    }

    int bfactor(node* p)
    {
        return height(p->right)-height(p->left);
    }

    void fixheight(node* p)
    {
        unsigned char hl = height(p->left);
        unsigned char hr = height(p->right);
        p->height = (hl>hr?hl:hr)+1;
    }

    node* rotateright(node* p)
    {
        node* q = p->left;
        p->left = q->right;
        q->right = p;
        fixheight(p);
        fixheight(q);
        return q;
    }

    node* rotateleft(node* q)
    {
        node* p = q->right;
        q->right = p->left;
        p->left = q;
        fixheight(q);
        fixheight(p);
        return p;
    }

    node* balance(node* p) // balancing the p node
    {
        fixheight(p);
        if( bfactor(p)==2 )
        {
            if( bfactor(p->right) < 0 )
                p->right = rotateright(p->right);
            return rotateleft(p);
        }
        if( bfactor(p)==-2 )
        {
            if( bfactor(p->left) > 0  )
                p->left = rotateleft(p->left);
            return rotateright(p);
        }
        return p; // balancing is not required
    }

    node* insert(node* p, node* n) // insert node n in a tree with p root
    {
        if( !p ) return n;
        if( n->key < p->key )
            p->left = insert(p->left, n);
        else
            p->right = insert(p->right, n);
        return balance(p);
    }

    void clear_node(node* p)
    //clear the node so that it can be deleted
    {
        if(not p) return;
        if(p->data) delete p->data;

        clear_node(p->left);
        clear_node(p->right);

        if(p->left) delete p->left;
        if(p->right) delete p->right;
    }

    node* root;

public:

    span_tree()
    {
        root=0;
    }

    ~span_tree()
    {
        if(root)
        {
            clear_node(root);
            delete root;
        }
    }

    void insert(double key, DATA_T* data)
    // tree will take control of data, and delete it when tree goes out of scope
    {
        node* p=new node;
        p->key=key;
        p->data=data;

        if(not root)
        {
            root=p;
        }
        else
        {
            insert(root, p);
        }
    }

    template <class... args_t>
    DATA_T* emplace(double key, args_t...args)
    // construct the data from arguments
    {
        node* p=new node;
        p->key=key;
        p->data=data(args...);
        auto ret=p->data;

        if(not root)
        {
            root=p;
        }
        else
        {
            insert(root, p);
        }
        return ret;
    }

    struct span_result
    {
        double left_key;
        double right_key;
        DATA_T* left_data;
        DATA_T* right_data;
    };

    span_result lookup(double key)
    {
        double current_left_key;
        double current_right_key;
        DATA_T* current_left_data=0;
        DATA_T* current_right_data=0;

        node* next_node=root;
        while(next_node)
        {
            if(key < next_node->key)
            {
                current_right_key=next_node->key;
                current_right_data=next_node->data;
                next_node=next_node->left;
            }
            else
            {
                current_left_key=next_node->key;
                current_left_data=next_node->data;
                next_node=next_node->right;
            }
        }

        if(not current_left_data)
        {
            throw gen_exception("value ", key, " is below smallest value in tree: ", current_right_key);
        }
        if(not current_right_data)
        {
            throw gen_exception("value ", key, " is above largest value in tree: ", current_left_key);
        }

        span_result ret;
        ret.left_key=current_left_key;
        ret.right_key=current_right_key;
        ret.left_data=current_left_data;
        ret.right_data=current_right_data;
        return ret;
    }

};


#endif // SPAN_TREE_HPP
