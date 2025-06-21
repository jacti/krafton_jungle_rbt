#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    rbtree *t = new_rbtree();
    char dot_file_buffer[100];
    char img_file_buffer[100];
    for(int i=0; i<20;i++){
        rbtree_insert(t,i);
        sprintf(dot_file_buffer, "out/result/dot_%d.dot",i);
        sprintf(img_file_buffer, "out/result/img_%d.png",i);
        rbtree_visualize(t,dot_file_buffer,img_file_buffer);
    }

}