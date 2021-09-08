#include "rbtree.h"

#include <malloc.h>

void delete_node(node_t *);
node_t *rbtree_find_value(node_t *, const key_t);

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(sizeof(rbtree), 1);
  return p;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  delete_node(t->root);
  free(t);
}

void delete_node(node_t *p) {
  if (p) {
    delete_node(p->left);
    delete_node(p->right);
    free(p);
  }
}

void left_rotate(rbtree *t, node_t *x) {
  node_t *y = x->right;
  x->right = y->left;
  if (y->left != NULL) {
    y->left->parent = x;
  }
  y->parent = x->parent;
  if (x->parent == NULL) {
    t->root = y;
  } else if (x == x->parent->left) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }
  y->left = x;
  x->parent = y;
}

void right_rotate(rbtree *t, node_t *y) {
  node_t *x = y->left;
  y->left = x->right;
  if (x->right != NULL) {
    x->right->parent = x;
  }
  x->parent = y->parent;
  if (y->parent == NULL) {
    t->root = x;
  } else if (y == y->parent->right) {
    y->parent->right = x;
  } else {
    y->parent->left = x;
  }
  x->right = y;
  y->parent = x;
}

void rbtree_insert_fixup(rbtree *t, node_t *z) {
  while (z->parent && z->parent->parent && z->parent->color == RBTREE_RED) {
    if (z->parent == z->parent->parent->left) {
      node_t *y = z->parent->parent->right;
      if (y && y->color == RBTREE_RED) {  // z의 삼촌 y가 적색인 경우
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      } else {
        if (z == z->parent->right) {  // z의 삼촌 y가 흑색이며 z가 오른쪽 자식인 경우
          z = z->parent;
          left_rotate(t, z);
        }
        if (z->parent) {
          z->parent->color = RBTREE_BLACK;
          if (z->parent->parent) {
            z->parent->parent->color = RBTREE_RED;
            right_rotate(t, z->parent->parent);
          }
        }
      }
    } else {
      node_t *y = z->parent->parent->left;
      if (y && y->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      } else {
          if (z == z->parent->left) {
            z = z->parent;
            right_rotate(t, z);
          }
        if (z->parent) {
          z->parent->color = RBTREE_BLACK;
          if (z->parent->parent) {
            z->parent->parent->color = RBTREE_RED;
            left_rotate(t, z->parent->parent);
          }
        }
      }
    }
  }
  // printf("%d \n\n", t->root->key);
  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  node_t *z = calloc(sizeof(node_t), 1);
  z->key = key;
  node_t *y = NULL;
  node_t *x = t->root;
  while (x != NULL) {
    y = x;
    if (z->key < x->key) {
      x = x->left;
    } else {
      x = x->right;
    }
  }
  z->parent = y;
  if (y == NULL) {
    t->root = z;
  } else if (z->key < y->key) {
    y->left = z;
  } else {
    y->right = z;
  }
  z->color = RBTREE_RED;
  rbtree_insert_fixup(t, z);
  return t->root;
}

/* 탐색 */
node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  return rbtree_find_value(t->root, key);
}

node_t *rbtree_find_value(node_t *p, const key_t key) {
  if (p == NULL || p->key == key) {
    return p;  // 매개변수가 const node_t *p일 때는 왜 warning일까?
  } else if (key > p->key) {
    return rbtree_find_value(p->right, key);
  } else {
    return rbtree_find_value(p->left, key);
  }
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  node_t *x = t->root;
  while (x->left != NULL) {
    x = x->left;
  }
  return x;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement 
  node_t *x = t->root;
  while (x->right != NULL) {
    x = x->right;
  }
  return x;
}

void rb_transplant(rbtree *t, node_t *u, node_t *v) {
  if (u->parent == NULL) {
    t->root = v;
  } else if (u == u->parent->left) {
    u->parent->left = v;
  } else {
    u->parent->right = v;
  }
  if (v != NULL) {
    v->parent = u->parent;
  }
}

node_t *subtree_min(node_t *p) {
  while (p->left != NULL) {
    p = p->left;
  }
  return p;
}

void rbtree_erase_fixup(rbtree *t, node_t *x) {
  while (x != t->root && x->color == RBTREE_BLACK) {
    if (x == x->parent->left) {
      node_t *w = x->parent->right;
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      } else if (w->right->color == RBTREE_BLACK) {
        w->left->color = RBTREE_BLACK;
        w->color = RBTREE_RED;
        right_rotate(t, w);
        w = x->parent->right;
      }
      w->color = x->parent->color;
      x->parent->color = RBTREE_BLACK;
      w->right->color = RBTREE_BLACK;
      left_rotate(t, x->parent);
      x = t->root;
    } else {
      node_t *w = x->parent->left;
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      } else if (w->left->color == RBTREE_BLACK) {
        w->right->color = RBTREE_BLACK;
        w->color = RBTREE_RED;
        left_rotate(t, w);
        w = x->parent->left;
      }
      w->color = x->parent->color;
      x->parent->color = RBTREE_BLACK;
      w->left->color = RBTREE_BLACK;
      right_rotate(t, x->parent);
      x = t->root;
    }
  }
  if (x) {
    x->color = RBTREE_BLACK;
  }
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  node_t *y = p;
  color_t y_original_color = y->color;
  node_t *x;
  if (p->left == NULL) {
    x = p->right;
    rb_transplant(t, p, p->right);
  } else if (p->right == NULL) {
    x = p->left;
    rb_transplant(t, p, p->left);
  } else {
    y = subtree_min(p->right);
    y_original_color = y->color;
    x = y->right;
    if (y->parent == p) {
      x->parent = y;
    } else {
      rb_transplant(t, y, y->right);
      y->right = p->right;
      y->right->parent = y;
    }
    rb_transplant(t, p, y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }
  if (y_original_color == RBTREE_BLACK) {
    rbtree_erase_fixup(t, x);
  }
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}

// void print_inorder(node_t *root) {
//   if (root == NULL)
//     return ;
//   print_inorder(root -> left);
//   printf("%d   %d\n", root -> key, root -> color);
//   print_inorder(root -> right);
// }