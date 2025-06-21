// visualize_rbtree.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rbtree.h"

// 디렉터리 확인 및 생성 (mkdir -p 기능)
static void ensure_dir(const char *filename) {
    char *path = strdup(filename);
    if (!path) return;
    char *slash = strrchr(path, '/');
    if (slash) {
        *slash = '\0';
        char cmd[512];
        snprintf(cmd, sizeof(cmd), "mkdir -p '%s'", path);
        system(cmd);
    }
    free(path);
}

// 재귀적으로 각 노드를 dot 포맷으로 출력
static void export_node(FILE *fp, node_t *n, node_t *nil) {
    
    if (n == nil) return;

    const char *col = (n->color == RBTREE_RED) ? "red" : "black";
    fprintf(fp,
        "  \"%p\" [label=\"%d\", shape=circle, style=filled, color=%s, fillcolor=%s, fontcolor=white];\n",
        (void*)n, n->key, col, col);

    if (n->left != nil) {
        fprintf(fp, "  \"%p\" -> \"%p\";\n", (void*)n, (void*)n->left);
        export_node(fp, n->left, nil);
    } else {
        fprintf(fp,
            "  null%pL [shape=point, width=0.1];\n"
            "  \"%p\" -> null%pL;\n",
            (void*)n, (void*)n, (void*)n);
    }

    if (n->right != nil) {
        fprintf(fp, "  \"%p\" -> \"%p\";\n", (void*)n, (void*)n->right);
        export_node(fp, n->right, nil);
    } else {
        fprintf(fp,
            "  null%pR [shape=point, width=0.1];\n"
            "  \"%p\" -> null%pR;\n",
            (void*)n, (void*)n, (void*)n);
    }
}

// dot 파일 생성 및 Graphviz 호출
void rbtree_visualize(rbtree *tree, const char *dotfile, const char *imgfile) {
    // 디렉터리 생성
    ensure_dir(dotfile);
    ensure_dir(imgfile);

    FILE *fp = fopen(dotfile, "w");
    if (!fp) {
        perror("fopen");
        return;
    }
    fprintf(fp,
        "digraph RBTree {\n"
        "  node [fontname=\"Arial\"];\n");
    if (tree->root != tree->nil)
        export_node(fp, tree->root, tree->nil);
    fprintf(fp, "}\n");
    fclose(fp);

    // dot 툴로 PNG 생성
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "dot -Tpng '%s' -o '%s'", dotfile, imgfile);
    system(cmd);
}
