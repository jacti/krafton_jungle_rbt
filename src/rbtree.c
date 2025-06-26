#include "rbtree.h"
#include <stdbool.h>
#include <stdlib.h>

static node_t _nil = {
    .color = RBTREE_BLACK,
    .key = 0,
    .parent = NULL,
    .left = NULL,
    .right = NULL};

const static node_t *NIL = &_nil;

__attribute__((constructor)) // GCC 전용: main 전에 실행됨
static void
init_nil_node()
{
    _nil.parent = &_nil;
    _nil.left = &_nil;
    _nil.right = &_nil;
}

rbtree *new_rbtree(void)
{
    rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));
    t->nil = NIL;

    // root에 nil 정의
    t->root = NIL;
    return t;
}

// rbtree 원소를 재귀로 제거
static void _delete_rbtree(node_t *root)
{
    if (root->left != NIL)
        _delete_rbtree(root->left);
    if (root->right != NIL)
        _delete_rbtree(root->right);
    if (root != NIL)
        free(root);
}

void delete_rbtree(rbtree *t)
{
    _delete_rbtree(t->root);
    // free(NIL);
    free(t);
}

typedef enum
{
    LEFT,
    RIGHT
} direction_t;

static void _setChild(node_t *parent, node_t *child, direction_t isRight)
{
    if (child != NIL)
    {
        child->parent = parent;
    }
    if (parent == NIL)
        return;
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
회전하고 색을 유지하기 위해 회전하는 두색을 바꿔줌
입력 : 부모노드, 회전 방향
*/
static void _rotate(node_t *parent, direction_t isRight, rbtree * t)
{
    node_t *grandParent = parent->parent;
    node_t *newParent = _getChild(parent, !isRight);
    node_t *beta = _getChild(newParent, isRight);

    // 색 교환
    color_t parentColor = parent->color;
    parent->color = newParent->color;
    newParent->color = parentColor;

    // 내부 회전
    _setChild(parent, beta, !isRight);
    _setChild(newParent, parent, isRight);

    // 조부모와 새 부모를 연결
    // !조부모가 NIL이면 루트니 t->root로 변경
    _setChild(grandParent, newParent, (grandParent->right == parent));
    if(newParent->parent == NIL){
        t->root = newParent;
    }
}

node_t *rbtree_insert(rbtree *t, const key_t key)
{
    // 새 노드를 만들고 초기화 (red, NIL)
    node_t *newNode = malloc(sizeof(node_t));
    newNode->key = key;
    newNode->color = RBTREE_RED;
    newNode->left = NIL;
    newNode->right = NIL;
    newNode->parent = NIL;

    // root가 NIL이면 새 노드를 루트로 입력하고 종료
    if (t->root == NIL)
    {
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

    cur = newNode;
    node_t *uncle;
    direction_t parentDirection, curDirection;
    // 부모가 레드여서 이중 레드일 동안 반복해서 수정
    while (cur->parent->color == RBTREE_RED)
    {
        parent = cur->parent;
        parentDirection = (parent->parent->right == parent);
        uncle = _getChild(parent->parent, !parentDirection);

        // CASE 1 : uncle이 red일 경우 -> grand parent ->red; parent & uncle -> black;
        if (uncle->color == RBTREE_RED)
        {
            parent->color = RBTREE_BLACK;
            uncle->color = RBTREE_BLACK;
            parent->parent->color = RBTREE_RED;
            cur = parent->parent;
            continue;
        }

        curDirection = (parent->right == cur);
        node_t *grandParent = parent->parent;

        // CASE 2 : parent direction 과 cur direction이 다른 경우 -> 다이아몬드 모양 -> 쭉 펴줌
        if (curDirection != parentDirection)
        {
            _rotate(parent, !curDirection, t);
        }

        // CASE 3 : parent direction 과 cur direction이 같을 경우 -> 한칸 내리고 색칠
        _rotate(grandParent, !parentDirection, t);
        // grandParent->color = RBTREE_RED;
        // grandParent->parent->color = RBTREE_BLACK;
        cur = grandParent->parent;
        break;
    }

    // cur 가 root가 됐으면 부모가 NIL일 것이므로 루트 변경
    if (cur->parent == NIL)
    {
        t->root = cur;
        cur->color = RBTREE_BLACK;
    }

    return newNode;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{
    node_t *cur = t->root;
    while (cur != NIL)
    {
        if (cur->key == key)
        {
            return cur;
        }
        cur = key < cur->key ? cur->left : cur->right;
    }
    return NULL;
}

static node_t *_rbtree_min(const node_t *root)
{
    node_t *cur = root;
    while (cur->left != NIL)
    {
        cur = cur->left;
    }
    return cur;
}

node_t *rbtree_min(const rbtree *t)
{
    return _rbtree_min(t->root);
}

static node_t *_rbtree_max(const node_t *root)
{
    node_t *cur = root;
    while (cur->right != NIL)
    {
        cur = cur->right;
    }
    return cur;
}

node_t *rbtree_max(const rbtree *t)
{
    if (t->root == NIL)
    {
        return NULL;
    }
    return _rbtree_max(t->root);
}

// *u 위치를 *v로 대체
static void _transplant(node_t *u, node_t *v)
{
    _setChild(u->parent, v, (u->parent->right == u));
    _setChild(v, u->left, LEFT);
    _setChild(v, u->right, RIGHT);
}

int rbtree_erase(rbtree *t, node_t *p)
{
    if(p == t->root && p->left == NIL && p->right == NIL){
        t->root = NIL;
        free(p);
        return 0;
    }
    /* step 1 : BST 삭제 매 구현
        색을 유지해줘야할 replaceColor 찾아서 저장
    */
    color_t replaceColor = p->color;
    node_t * replacer, *cur, *parent;
    if (p->left != NIL && p->right != NIL)
    {
        replacer = _rbtree_min(p->right);
        if(replacer != p->right){
            _setChild(replacer->parent, replacer->right, LEFT);
        }
        else {
            _setChild(p,replacer->right,RIGHT);
        }
        cur = replacer ->right;
        parent = cur == NIL ? replacer : cur->parent;
        replaceColor = replacer->color;
        replacer->color = p->color;
        _transplant(p, replacer);
    }
    else
    {
        replacer = _getChild(p, (p->right != NIL));
        cur = replacer;
        parent = cur->parent;
        if(p->left == NIL && p->right == NIL) {
            parent = p->parent;
        }
        _setChild(p->parent, replacer, (p->parent->right == p));
    }
    if(p == t->root){
        t->root = replacer;
    }
    // STEP 2 : 없어진 색이 black이면 FIX가 필요
    if(replaceColor == RBTREE_BLACK){
        // CASE double-black :
        while(cur->color == RBTREE_BLACK && t->root != cur){
            direction_t curDirection = (parent->right == cur);
            node_t *brother = _getChild(parent,!curDirection);
            // CASE 1 : 형제가 빨강 (부모는 검정)
            //  조치  이후 CASE 2, 3, 4 중 하나로 변환 됨
            if(brother->color == RBTREE_RED){
                _rotate(parent, curDirection, t);
                brother = _getChild(parent,!curDirection);
                _nil.color = RBTREE_BLACK;
            }

            // NOTE : 이후부터는 형제가 검정
            // CASE 2 : 형제의 두 자식이 모두 검정
            if(brother->left->color == RBTREE_BLACK && brother->right->color == RBTREE_BLACK){
                brother->color = RBTREE_RED;
                cur = parent;
                parent = cur->parent;
                _nil.color = RBTREE_BLACK;
                continue;
            }

            // CASE 3 : 형재의 내 쪽 자식이 빨강, 반대 쪽 자식은 검정
            else if(_getChild(brother, curDirection)->color == RBTREE_RED, _getChild(brother, !curDirection) == RBTREE_BLACK){
                _rotate(brother, !curDirection, t);
                brother = brother->parent;
                _nil.color = RBTREE_BLACK;
                // NOTE : CASE 4로 바뀜
            }

            // CASE 4 : 형제의 반대 자식이 빨강
            if(_getChild(brother,!curDirection)->color == RBTREE_RED){

                _getChild(brother,!curDirection)->color = RBTREE_BLACK;
                _rotate(parent, curDirection, t);
                _nil.color = RBTREE_BLACK;
                break;
            }
        }

        // CASE red-black : 교체한 내가 RED이면 검정으로 칠하고 종료
        if(cur->color == RBTREE_RED){
            cur->color = RBTREE_BLACK;
        }
        t->root->color = RBTREE_BLACK;
    }

    free(p);
    return 0;
}

int rbtree_inorder(key_t *arr, node_t * cur, int index, const size_t n){
    if(cur == NIL || index >= n){
        return index;
    }
    index = rbtree_inorder(arr,cur->left,index, n);
    arr[index] = cur->key;
    return rbtree_inorder(arr,cur->right,index+1, n);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
    int index = rbtree_inorder(arr, t->root, 0, n);
    if(index != n){
        return 1;
    }
    return 0;
}
