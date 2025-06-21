// visualize_rbtree.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbtree.h>

//선 정의
static void ensure_dir(const char *filename);
static void export_node(FILE *fp, node_t *n, node_t *nil);
void rbtree_visualize(rbtree *tree, const char *dotfile, const char *imgfile);


/*
 ______    ____       ____        ______   
/\__  _\  /\  _`\    /\  _`\     /\__  _\  
\/_/\ \/  \ \ \L\_\  \ \,\L\_\   \/_/\ \/  
   \ \ \   \ \  _\L   \/_\__ \      \ \ \  
    \ \ \   \ \ \L\ \   /\ \L\ \     \ \ \ 
     \ \_\   \ \____/   \ `\____\     \ \_\
      \/_/    \/___/     \/_____/      \/_/
                                           
main에 테스트 함수 작성
*/
int main()
{

    rbtree *t = new_rbtree();
    char dot_file_buffer[100];  //dot file 이름
    char img_file_buffer[100];  //img file 이름

    //0부터 10까지 원소를 추가하는 테스트
    for(int i=0; i<10;i++){
        rbtree_insert(t,i);
        //파일 이름 for 문으로 생성
        sprintf(dot_file_buffer, "out/result/serial_d_%d.dot",i);
        sprintf(img_file_buffer, "out/result/serial_i_%d.png",i);
        rbtree_visualize(t,dot_file_buffer,img_file_buffer);
    }

    //10개의 랜덤 원소를 넣는 테스트
    for(int i=0; i<10;i++){
        rbtree_insert(t,rand()%100);
        //파일 이름 for 문으로 생성
        sprintf(dot_file_buffer, "out/result/rand_d_%d.dot",i);
        sprintf(img_file_buffer, "out/result/rand_i_%d.png",i);
        rbtree_visualize(t,dot_file_buffer,img_file_buffer);
    }

}




// 디렉터리 확인 및 생성 (mkdir -p 기능)
static void ensure_dir(const char *filename)
{
    char *path = strdup(filename);
    if (!path)
        return;
    char *slash = strrchr(path, '/');
    if (slash)
    {
        *slash = '\0';
        char cmd[512];
        snprintf(cmd, sizeof(cmd), "mkdir -p '%s'", path);
        system(cmd);
    }
    free(path);
}

// 재귀적으로 각 노드를 dot 포맷으로 출력
static void export_node(FILE *fp, node_t *n, node_t *nil)
{
    if (n == nil)
        return;

    const char *col = (n->color == RBTREE_RED) ? "red" : "black";
    fprintf(fp,
            "  \"%p\" [label=\"%d\", shape=circle, style=filled, color=%s, fillcolor=%s, fontcolor=white];\n",
            (void *)n, n->key, col, col);

    if (n->left != nil)
    {
        fprintf(fp, "  \"%p\" -> \"%p\";\n", (void *)n, (void *)n->left);
        export_node(fp, n->left, nil);
    }
    else
    {
        fprintf(fp,
                "  null%pL [shape=point, width=0.1];\n"
                "  \"%p\" -> null%pL;\n",
                (void *)n, (void *)n, (void *)n);
    }

    if (n->right != nil)
    {
        fprintf(fp, "  \"%p\" -> \"%p\";\n", (void *)n, (void *)n->right);
        export_node(fp, n->right, nil);
    }
    else
    {
        fprintf(fp,
                "  null%pR [shape=point, width=0.1];\n"
                "  \"%p\" -> null%pR;\n",
                (void *)n, (void *)n, (void *)n);
    }
}

// dot 파일 생성 및 Graphviz 호출
void rbtree_visualize(rbtree *tree, const char *dotfile, const char *imgfile)
{
    // 디렉터리 생성
    ensure_dir(dotfile);
    ensure_dir(imgfile);

    node_t *nil;
    #ifdef SENTINEL
        nil = tree->nil;
    #else
        nil = null;
    #endif

    FILE *fp = fopen(dotfile, "w");
    if (!fp)
    {
        perror("fopen");
        return;
    }
    fprintf(fp,
            "digraph RBTree {\n"
            "  node [fontname=\"Arial\"];\n");
    if (tree->root != tree->nil)
        export_node(fp, tree->root, nil);
    fprintf(fp, "}\n");
    fclose(fp);

    // dot 툴로 PNG 생성
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "dot -Tpng '%s' -o '%s'", dotfile, imgfile);
    system(cmd);
}
