 /*

time tree is a way to keep objects sorted by time, which is represented by a double

used red-black tree based on:

 <http://nathanbelue.blogspot.com/2012/04/red-black-trees.html>
*/

#include <cstdlib>
#include <iostream>
#include <utility>
#include <memory>

#ifndef TIME_TREE_HPP
#define TIME_TREE_HPP

// The type of data to store
//typedef int value_type;

// A constant reference to the type of data to store
//typedef const value_type & const_reference;


//// this time-tree may be slow for small number of objects. N<100??  Maybe should make a sorted-linked list for that case
// add two classes:  sorted_time_list and unsorted_time_list


template< typename DATA_T>
class time_tree
{
    /*
    * For representing colors
    */
    enum color_type
    {
       RED,
       BLACK
    };

    /*
    * Red-black tree node
    */
    struct rb_node
    {
       double time;
       DATA_T* data_ptr; //we own this data!

       color_type color;
       rb_node * parent;
       rb_node * left;
       rb_node * right;
    };
    typedef rb_node * node_ptr;



   /*
    * Data
    */
   // The root of the tree
   node_ptr _root;
   node_ptr first;


   /*
    * Typedefs
    */
   // A link typedef
   typedef node_ptr * link_type;

protected:


  /*
   * Helper functions
   */
  // A handy function for getting a node's color
  // It even works for imaginary leaf nodes!
  color_type _node_color( node_ptr node )
  {
      if( node )
          return node->color;
      return BLACK;
  }

  // Get a node's sibling
  node_ptr _node_sibling( node_ptr node )
  {
      node_ptr par = node->parent;
      node_ptr sib = par->left;
      if( sib == node )
          sib = par->right;
      return sib;
  }

  // Return the leftmost node of a subtree
  node_ptr _node_leftmost( node_ptr node )
  {
      while( node->left )
          node = node->left;
      return node;
  }

  // Convert a node pointer to a link
  // You must pass it something like node->left.
  // Passing it just node or just left won't work.
  link_type _make_link( node_ptr & node )
  {
      return & node;
  }

  // Get a node's parent's link to that node
  link_type _get_parent_link( node_ptr node )
  {
      node_ptr parent = node->parent;
      if( ! parent )
          return _make_link( _root );
      if( parent->left == node )
          return _make_link( parent->left );
      //if( parent->right == node )
          return _make_link( parent->right );
  }

  // Get a link's destination
  node_ptr _link_dest( link_type link )
  {
      return *link;
  }

  // Set a link's destination
  void _link_set_dest( link_type link, node_ptr dest )
  {
      *link = dest;
  }

  // Get a link's origin
  node_ptr _link_orig( link_type link )
  {
      return _link_dest(link) -> parent;
  }


  // Clear out the data in a subtree
  void _clear( node_ptr subtree )
  {
      if( subtree ) {
          _clear( subtree->left );
          _clear( subtree->right );
          delete subtree->data_ptr;
          delete subtree;
      }
  }


  // Rotate about a node counterclockwise
  void _rotate_counterclockwise( link_type link )
  {
      node_ptr node = _link_dest(link);
      node_ptr right = node->right;
      node_ptr rleft = right->left;

      _link_set_dest( link, right );
      right->parent = node->parent;

      right->left = node;
      node->parent = right;

      node->right = rleft;
      if( rleft )
          rleft->parent = node;
  }

  // Rotate about a node clockwise
  void _rotate_clockwise( link_type link )
  {
      node_ptr node = _link_dest( link );
      node_ptr left = node->left;
      node_ptr lright = left->right;

      _link_set_dest( link, left );
      left->parent = node->parent;

      left->right = node;
      node->parent = left;

      node->left = lright;
      if( lright )
          lright->parent = node;
  }


  // Figure out where to insert a value in the tree
  // Returns a pair, where first is the link (nullptr if the
  // value is already in the tree) and second is the origin
  // of the link.
  std::pair<link_type, node_ptr>
  _get_insert_link( const double& value)
  {
      link_type where = _make_link(_root);
      node_ptr origin = nullptr;

      while( _link_dest(where) ) {
          origin = _link_dest(where);
          if( value < origin->time )
              where = _make_link( origin->left );
          else if( value >= origin->time )
              where = _make_link( origin->right );
          else {
              where = nullptr;
              break;
          }
      }

      return std::make_pair( where, origin );
  }


  // Balance the tree after insertion
  // This function handles the harder cases.
  void _insert_harder_balancing( node_ptr node )
  {
      node_ptr parent = node->parent;
      node_ptr sibling = _node_sibling( node );

      // Initial case 1: node black, par black, sib red
      if( _node_color(sibling) == RED ) {
          sibling->color = BLACK;
          parent->color = RED;

          // If parent's parent is red
          if( _node_color(parent->parent) == RED ) {
              parent->parent->color = BLACK;
              _insert_harder_balancing( parent->parent );
          }
      }

      // Initial case 2: node black, par black, sib black
      else {
          parent->color = RED;
          if( node == parent->left ) {
              if( _node_color(node->right) == RED ) {
                  node->color = RED;
                  node->right->color = BLACK;
                  _rotate_counterclockwise(
                      _make_link(parent->left) );
              }
              _rotate_clockwise(
                  _get_parent_link(parent) );
          }
          else {
              if( _node_color(node->left) == RED ) {
                  node->color = RED;
                  node->left->color = BLACK;
                  _rotate_clockwise(
                      _make_link(parent->right) );
              }
              _rotate_counterclockwise(
                  _get_parent_link(parent) );
          }
      }
  }

  // Balance the tree after insertion
  // This function only handles the easy cases.  The harder
  // cases are handed off to another function.
  void _insert_balance( node_ptr node )
  {
      // We just inserted at the root?
      if( ! node->parent ) {
          node->color = BLACK;
          return; // done
      }

      // We just inserted a red as a black's child?
      if( _node_color(node->parent) == BLACK )
          return; // done

      // Otherwise...  we have two reds in a row
      node->parent->color = BLACK;
      _insert_harder_balancing( node->parent );
      _root->color = BLACK;
  }


  // [Possibly] replace and remove a node
  // Returns the actual node removed from the tree
  node_ptr _replace_and_remove_node( node_ptr node )
  {
      node_ptr rep = nullptr;

      if( node->left && node->right ) {
          rep = _node_leftmost( node->right );
          std::swap( rep->time, node->time );
          std::swap( rep->data_ptr, node->data_ptr );
          node = rep;
          rep = nullptr;
      }

      if( node->left )
          rep = node->left;
      else if( node->right )
          rep = node->right;

      _link_set_dest( _get_parent_link(node), rep );
      if( rep )
          rep->parent = node->parent;

      return node;
  }

  // Harder balancing stuff
  // Handles cases where parent is red
  void _erase_harder_balancing_red_parent( node_ptr sibling )
  {
      // Use default rotations and sib->left|right
      typedef void (time_tree::*rot_func)( link_type );
      rot_func rot_clock = & time_tree::_rotate_clockwise;
      rot_func rot_counter= & time_tree::_rotate_counterclockwise;
      node_ptr sib_left = sibling->left;
      node_ptr sib_right = sibling->right;

      // But if sibling is on the left, turn them around.
      if( sibling == sibling->parent->left ) {
          std::swap( rot_clock, rot_counter );
          std::swap( sib_left, sib_right );
      }

      // One more variable
      node_ptr parent = sibling->parent;

      // Parent is red, by the way
      // Sibling must be black.

      // Case 6: both of sibling's children are black
      if( _node_color(sib_left) == BLACK
          && _node_color(sib_right) == BLACK ) {
          (this->*rot_counter)( _get_parent_link(parent) );
      }

      // Case 7: sib_left is red, sib_right is black
      else if( _node_color(sib_left) == RED
          && _node_color(sib_right) == BLACK ) {
          parent->color = BLACK;
          (this->*rot_clock)( _get_parent_link(sibling) );
          (this->*rot_counter)( _get_parent_link(parent) );
      }

      // Case 8: sib_left is black, sib_right is red
      else if( _node_color(sib_left) == BLACK
          && _node_color(sib_right) == RED ) {
          (this->*rot_counter)( _get_parent_link(parent) );
      }

      // Case 9: both of sibling's children are red
      else {
          parent->color = BLACK;
          (this->*rot_clock)( _get_parent_link(sibling) );
          (this->*rot_counter)( _get_parent_link(parent) );
      }
  }

  // Harder balancing stuff
  void _erase_harder_balancing( node_ptr sibling )
  {
      // Use default rotations and sib->left|right
      typedef void (time_tree::*rot_func)( link_type );
      rot_func rot_clock = & time_tree::_rotate_clockwise;
      rot_func rot_counter= & time_tree::_rotate_counterclockwise;
      node_ptr sib_left = sibling->left;
      node_ptr sib_right = sibling->right;

      // But if sibling is on the left, turn them around.
      if( sibling == sibling->parent->left ) {
          std::swap( rot_clock, rot_counter );
          std::swap( sib_left, sib_right );
      }

      // One more variable
      node_ptr parent = sibling->parent;

      // Case 1-5: parent is black
      if( _node_color(parent) == BLACK ) {

          // Case 1-4: sibling is black
          if( _node_color(sibling) == BLACK ) {

              // Case 1: sibling's children are both black
              if( _node_color(sib_left) == BLACK
                  && _node_color(sib_right) == BLACK ) {
                  sibling->color = RED;
                  if( parent->parent ) {
                      _erase_harder_balancing(
                          _node_sibling(parent) );
                  }
              }

              // Case 2: sib_left is red, sib_right is black
              else if( _node_color(sib_left) == RED
                  && _node_color(sib_right) == BLACK ) {
                  sib_left->color = BLACK;
                  (this->*rot_clock)(
                      _get_parent_link(sibling) );
                  (this->*rot_counter)(
                      _get_parent_link(parent) );
              }

              // Case 3: sib_left is black, sib_right is red
              else if( _node_color(sib_left) == BLACK
                  && _node_color(sib_right) == RED ) {
                  sib_right->color = BLACK;
                  (this->*rot_counter)(
                      _get_parent_link(parent) );
              }

              // Case 4: sibling's children are both red
              else {
                  sib_left->color = BLACK;
                  (this->*rot_clock)(
                      _get_parent_link(sibling) );
                  (this->*rot_counter)(
                      _get_parent_link(parent) );
              }

          }

          // Case 5: sibling is red
          else {
              parent->color = RED;
              sibling->color = BLACK;
              (this->*rot_counter)( _get_parent_link(parent) );
              _erase_harder_balancing_red_parent( sib_left );
          }

      }

      // Case 6-9: parent is red
      else {
          _erase_harder_balancing_red_parent( sibling );
      }
  }

  // Balance the tree after erasing
  // Node is the node that was removed
  void _erase_balance( node_ptr node )
  {
      // If we just deleted a red node, we're done
      if( _node_color(node) == RED )
          return;

      // If we deleted a black node, but it has a red child
      if( node->left || node->right ) {
          if( node->left )
              node->left->color = BLACK;
          else
              node->right->color = BLACK;
          return;
      }

      // If we just deleted the root, we're done
      if( ! node->parent )
          return;

      // Otherwise, we have a black node with no children
      node_ptr sib = node->parent->left;
      if( ! sib )
          sib = node->parent->right;
      _erase_harder_balancing( sib );
  }


  // Check helper function
  // Returns the black height of a subtree
  int _check( node_ptr subtree )
  {
      // Imaginary leaf?  black height is 1
      if( ! subtree )
          return 1;

      node_ptr left = subtree->left;
      node_ptr right = subtree->right;

      // Black height of both sides must be the same
      int left_height = _check( left );
      int right_height = _check( right );
      if( left_height != right_height )
          throw "black imbalance!";

      // No two reds in a row
      if( _node_color(subtree) == RED ) {
          if( _node_color(left) == RED
              || _node_color(right) == RED )
              throw "two reds in a row!";
      }

      // We're black, the height is [left|right]_height + 1
      else
          ++ left_height;

      // Make sure that the children's parents are correct
      if( left && left->parent != subtree
          || right && right->parent != subtree )
          throw "parent pointer wrong!";

      // Return our height
      return left_height;
  }


public:

  /*
   * Interface functions
   */


  // Constructor
  time_tree() : _root(nullptr), first(nullptr)
  {
  }


  // Destructor
  ~time_tree()
  {
      _clear( _root );
  }


  // Clear out the data in the tree
  void clear()
  {
    if(_root)
    {
      _clear( _root );
      _root = nullptr;
    }
  }

//return data that is first in the tree, and remove it from the tree.
//the tree relinquishes ownership of the object
DATA_T* pop_first()
{
    if(not first)
    {
        return nullptr;
    }
    else
    {
        DATA_T* ret=first->data_ptr;
        node_ptr node=first;

        if(first->right)
        {
            first=first->right;
            while(first->left) first=first->left;
        }
        else
        {
            first=first->parent; //may be null
        }

        node = _replace_and_remove_node( node );
        _erase_balance( node );
        delete node;


        return ret;
    }
}

//return the data that is first in the tree without removing it
//tree keeps ownershp of the object
DATA_T* get_first()
{
    if(not first)
    {
        return nullptr;
    }
    else
    {
        return first->data_ptr;
    }
}

//return the time that is first in the tree without removing it
double get_first_time()
{
    if(not first)
    {
        return 0;
    }
    else
    {
        return first->time;
    }
}

/*
  // Find data in the tree
  // Returns nullptr if not found; otherwise returns a pointer to the data
  DATA_T* find( double time_value )
  {
      node_ptr node = _root;
      while( node ) {
          if( time_value < node->time )
              node = node->left;
          else if( time_value >= node->time )
              node = node->right;
          else
              break;
      }

      return node->data_ptr;
  }*/


  // Insert data into the tree
  void insert( double time_value, DATA_T* data_ptr )
  {
      // We need both the link and the parent
      std::pair<link_type, node_ptr> where;
      where = _get_insert_link( time_value );
      if( ! where.first )
      {
        throw gen_exception("could not insert new node");
      }

      // Create the node
      node_ptr node = new rb_node;
      node->time = time_value;
      node->data_ptr = data_ptr;
      node->color = RED;
      node->left = node->right = nullptr;
      node->parent = where.second;

      if(first==nullptr or (time_value < (first->time)))
      {
        first=node;
      }

      // Attach it to the tree
      _link_set_dest( where.first, node );

      // Balance
      _insert_balance( node );
  }

    //make a new object with arguments, place it in the time tree
    //return pointer to new object, but time tree maintains ownership of the object
    template< typename... args_T >
    DATA_T* emplace(double time, args_T&& ...args)
    {
        DATA_T* new_data_ptr=new DATA_T(args...);
        insert(time, new_data_ptr);
        return new_data_ptr;
    }
/*
  // Erase data from the tree
  void erase( double value )
  {
      node_ptr node = find( value );
      if( ! node )
          return;

      node = _replace_and_remove_node( node );

      _erase_balance( node );

      delete note->data_ptr;
      delete node;
  }*/


  // Make sure that the tree is valid
  // Throws an error if it isn't.
  void check()
  {
      if( _node_color(_root) == RED )
          throw "root is red!";

      _check( _root );
  }

};

#endif
