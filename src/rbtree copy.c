#include "rbtree.h"

#include <stdlib.h>

#define NIL t->nil

rbtree *new_rbtree(void)
{
  rbtree *t = (rbtree *)malloc(sizeof(rbtree));
  // NIL 정의
  NIL = (node_t *)malloc(sizeof(node_t));
  NIL->color = RBTREE_BLACK;
  NIL->left = NIL;
  NIL->right = NIL;
  NIL->parent = NIL;
  NIL->key = -1;

  // root에 nil 정의
  t->root = NIL;
  return t;
}

// rbtree 원소를 재귀로 제거
static void _delete_rbtree(node_t *root, node_t *nil)
{
  if (root->left != nil)
    _delete_rbtree(root->left, nil);
  if (root->right != nil)
    _delete_rbtree(root->right, nil);
  if (root != nil)
    free(root);
}

void delete_rbtree(rbtree *t)
{
  _delete_rbtree(t->root, NIL);
  free(NIL);
  free(t);
}

typedef enum
{
  LEFT,
  RIGHT
} direction_t;

static void _setChild(node_t *parent, node_t *child, direction_t isRight)
{
  if(child->parent != child)
  {
    child->parent = parent;
  }
  if (isRight)
  {
    parent->right = child;
  }
  else
  {
    parent->left = child;
  }
}

static node_t *_getChild(node_t *parent, direction_t isRight)
{
  if (isRight)
  {
    return parent->right;
  }
  else
  {
    return parent->left;
  }
}

/*
노드를 회전하는 함수
입력 : 부모노드, 회전 방향
*/
static void _rotate(node_t *parent, direction_t isRight)
{
  node_t *grandParent = parent->parent, *newParent, *beta;

  // 회전 방향에 맞추어 new parent 지정
  // 내부 회전
  newParent = _getChild(parent, 1 ^ isRight);
  beta = _getChild(newParent, isRight);
  _setChild(parent, beta, 1 ^ isRight);
  _setChild(newParent, parent, isRight);

  // 조부모와 새 부모를 연결
  _setChild(grandParent,newParent,(grandParent->right == parent));
}

node_t *rbtree_insert(rbtree *t, const key_t key)
{
  // 새 노드를 만들고 초기화 (red, NIL)
  node_t *newNode = malloc(sizeof(node_t));
  newNode->key = key;
  newNode->color = RBTREE_RED;
  newNode->left = NIL;
  newNode->right = NIL;

  // root가 NIL이면 새 노드를 루트로 입력하고 종료
  if (t->root == NIL)
  {
    newNode->parent = NIL;
    newNode->color = RBTREE_BLACK;
    t->root = newNode;
    return t->root;
  }

  // BST처럼 새 노드가 삽입 될 위치를 찾음
  node_t *parent = NIL, *cur = t->root;
  while (cur != NIL)
  {
    parent = cur;
    cur = newNode->key < cur->key ? cur->left : cur->right;
  }
  _setChild(parent, newNode, (parent->key <= newNode->key));

  // 규칙에 맞게 수정
  cur = newNode;
  node_t *uncle;
  direction_t parentDirection, curDirection;
  // 부모가 레드여서 이중 레드일 동안 반복해서 수정
  while (cur->parent->color == RBTREE_RED)
  {
    parent = cur->parent;
    parentDirection = (parent->parent->right == parent);
    uncle = _getChild(parent->parent,1 ^ parentDirection);

    // CASE 1 : uncle이 red일 경우 -> grand parent ->red; parent & uncle -> black;
    if(uncle->color == RBTREE_RED){
      parent->color == RBTREE_BLACK;
      uncle->color == RBTREE_BLACK;
      parent->parent->color == RBTREE_RED;
      cur = parent->parent;
      continue;
    }

    // CASE 2 & 3 : uncle이 black -> cur를 black으로 grand parent를 red로 
    // parent를 기준으로 회전 , grand parent를 기준으로 반대 회전
    curDirection = (parent->right == cur);
    node_t *grandParent = parent->parent;
    grandParent->color = RBTREE_RED;
    if(curDirection != parentDirection){
      _rotate(parent,1 ^ curDirection);
    }
    _rotate(grandParent, 1 ^ parentDirection);
    grandParent->parent->color = RBTREE_BLACK;
    cur = grandParent->parent;
    break;
  }

  //cur 가 root가 됐으면 부모가 NIL일 것이므로 루트 변경
  if(cur->parent == NIL){
    t->root = cur;
  }

  return newNode;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{
  node_t *cur = t->root;
  while (cur != NIL)
  {
    if(cur->key == key){
      return cur;
    }
    cur = key < cur->key ? cur->left : cur->right;
  }
  return NULL;
}

static node_t *_rbtree_min(const node_t *root, const node_t *nil){
  node_t* cur = root;
  while(cur->left != nil){
    cur=cur->left;
  }
  return cur;
}

node_t *rbtree_min(const rbtree *t)
{
  return _rbtree_min(t->root,NIL);
}

static node_t *_rbtree_max(const node_t *root, const node_t *nil){
  node_t* cur = root;
  while(cur->right != nil){
    cur=cur->right;
  }
  return cur;
}

node_t *rbtree_max(const rbtree *t)
{
  return _rbtree_max(t->root, NIL);
}

// 트리 교체 정의
static void _transplant(rbtree *t, node_t *u, node_t *v){
  if(t->root == u){
    t->root == v;
  }
  else {
    direction_t uDirection = (u->parent->right == u);
    _setChild(u->parent,v,uDirection);
  }
}

// 트리 색 수정
static void _rbtree_fix_up(rbtree *t, node_t *x){

}

int rbtree_erase(rbtree *t, node_t *p)
{
  color_t origin_color = p->color;
  node_t *modify;
  
  // 삭제할 노드를 대체할 후보자 찾기
  node_t *replacement;
  if(p->left == NIL){
    replacement = p->right;
    modify = p->right;
  } else if (p->right == NIL){
    replacement = p->left;
    modify = p->left;
  } else {
    // 양 쪽 자식이 다 있으면 오른쪽에서 가장 작은 값 가져오기
    replacement = _rbtree_min(p->right, NIL);
    modify = replacement->right;
    //replacement가 대체되기 전 색 저장
    origin_color = replacement->color;
    if(replacement != p->right){
      //replacement 오른쪽 자식으로 replacement 대체
      _transplant(t,replacement,modify);
      replacement->right = p->right;
      p->right->parent = replacement;
    }
    //replacement 왼쪽에 연결
    replacement->left = p->left;
    p->left->parent = replacement;
  }
  _transplant(t,p,replacement);
  replacement->color = p->color;

  if(origin_color == RBTREE_BLACK){
    _rbtree_fix_up(t,modify);
  }

  free(p);
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  // TODO: implement to_array
  return 0;
}
