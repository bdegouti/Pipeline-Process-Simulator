#include <algorithm> 
  
  struct Node;
  typedef struct Node* Tree;


  int height(const Tree T);

  struct Node
  {
    char*   item;      // Information at this node
    int   ht;        // height of this node
    struct Node* left;      // The left subtree
    struct Node* right;     // The right subtree

    Node(char* it, struct Node* lft, struct Node* rgt)
    {
      item  = it;
      left  = lft;
      right = rgt;
      ht    = 1 + std::max(height(lft), height(rgt));
    }
  };

  //==========================================
  //               height
  //==========================================
  // height(T) returns the height of tree T.
  //
  // Requirement: If T is nonempty, then the
  // ht field has already been set correctly
  // T.
  //==========================================

  int height(const Tree T)
  {
    if(T == NULL)
    {
      return 0;
    }
    else
    {
      return T->ht;
    }
  }

  //==========================================
  //               installHeight
  //==========================================
  // installHeight(T) sets T->ht to the
  // height of T.
  //
  // Requirements:
  //   (1) T is not empty.
  //   (2) The ht field has already been set
  //       correctly in every node of each
  //       subtree of T.
  //==========================================

  void installHeight(Tree T)
  {
    T->ht = 1 + std::max(height(T->left), height(T->right));
  }
    //==========================================
  //              singleRotateLeft
  //==========================================
  // singleRotateLeft(T) performs a single
  // rotation from right to left at the
  // root of T.
  //==========================================

  void singleRotateLeft(Tree& T)
  {
    Tree r   = T->right;
    T->right = r->left;
    installHeight(T);

    r->left  = T;
    T        = r;
    installHeight(T);
  }

  //==========================================
  //              singleRotateRight
  //==========================================
  // singleRotateRight(T) performs a single
  // rotation from left to right at the
  // root of T.
  //==========================================

  void singleRotateRight(Tree& T)
  {
    Tree L   = T->left;
    T->left  = L->right;
    installHeight(T);

    L->right = T;
    T        = L;
    installHeight(T);
  }

  //==========================================
  //              doubleRotateLeft
  //==========================================
  // doubleRotateLeft(T) performs a double
  // rotation from right to left at the
  // root of T.
  //==========================================

  void doubleRotateLeft(Tree& T)
  {
    singleRotateRight(T->right);
    singleRotateLeft(T);
  }

  //==========================================
  //              doubleRotateRight
  //==========================================
  // doubleRotateRight(T) performs a double
  // rotation from left to right at the
  // root of T.
  //==========================================

  void doubleRotateRight(Tree& T)
  {
    singleRotateLeft(T->left);
    singleRotateRight(T);
  }

  //==========================================
  //              rotateLeft
  //==========================================
  // rotateLeft(T) performs a rotation from
  // from right to left at the root of T, 
  // choosing a single or double rotation.
  //==========================================

  void rotateLeft(Tree& T)
  {
    Tree r   = T->right;
    int  zag = height(r->left);
    int  zig = height(r->right);

    if(zig > zag)
    {
      singleRotateLeft(T);
    }
    else
    {
      doubleRotateLeft(T);
    }
  }

  //==========================================
  //              rotateRight
  //==========================================
  // rotateRight(T) performs a rotation from
  // from left to right at the root of T, 
  // choosing a single or double rotation.
  //==========================================

  void rotateRight(Tree& T)
  {
    Tree L  = T->left;
    int  zig = height(L->left);
    int  zag = height(L->right);

    if(zig > zag)
    {
      singleRotateRight(T);
    }
    else
    {
      doubleRotateRight(T);
    }
  }

  //==========================================
  //              rebalance
  //==========================================
  // rebalance(T) does the following.
  //
  //   (1) Perform a rotation at T if required.
  //
  //   (2) Set the ht field of T correctly,
  //       regardless of whether or not a
  //       rotation is done.
  //
  // Requirement: T must not be empty.
  //==========================================

  void rebalance(Tree& T)
  {
    int hl = height(T->left);
    int hr = height(T->right);

    if(hr > hl + 1)
    {
      rotateLeft(T);
    }
    else if(hl > hr + 1)
    {
      rotateRight(T);
    }
    else
    {
      installHeight(T);
    }
  }

    //=================================================
  //                insert
  //=================================================
  // insert(x,T) inserts x into binary search tree T.
  // If x is already a member of T, it does nothing.
  //
  // This function rebalances T after the insertion
  // if necessary.  It requires that T is
  // height-balanced when insert is called.
  //=================================================

  void insert(char* x, Tree& T)
  {
    if(T == NULL)
    {
      T = new Node(x, NULL, NULL);
    }
    else if(x < T->item)
    {
      insert(x, T->left);
      rebalance(T);
    }
    else if(x > T->item)
    {
      insert(x, T->right);
      rebalance(T);
    }
  }

  int search(char* x, Tree& T){
    if(T == NULL)
    {
      return 0;
    }
    else if(x == T->item){
      return 1;
    }
    else if(x < T->item)
    {
      search(x, T->left);
    }
    else if(x > T->item)
    {
      search(x, T->right);
    }
  }

