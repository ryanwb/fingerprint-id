#ifndef ZS_8CONN_H
#define ZS_8CONN_H

#include <stdio.h>
#include <stdlib.h>
#include "zhang_suen.h" // for zs_B function

int flood_count_recursive(int i, int j, unsigned char * nimg, int x, int y, int m, int n){
    //y and x are current positions
    //i and j are center-of-frame positions
    int z;
    if(!(i-1<=y && y<= i+1)){
        z=0;
        return z;
    }
    if(!(j-1<=x && x<=j+1)){
        z=0;
        return z;
    }
    if(*(nimg+y*n+x)==0){
        z=0;
        return z;
    }

    *(nimg+y*n+x)=0;
    z = 1 + flood_count_recursive(i, j, nimg, x-1, y-1, m, n)+flood_count_recursive(i, j, nimg, x, y-1, m, n)
    +flood_count_recursive(i, j, nimg, x+1, y-1, m, n)+flood_count_recursive(i, j, nimg, x-1, y, m, n)
    +flood_count_recursive(i, j, nimg, x+1, y, m, n)+flood_count_recursive(i, j, nimg, x-1, y+1, m, n)
    +flood_count_recursive(i, j, nimg, x, y+1, m, n)+flood_count_recursive(i, j, nimg, x+1, y+1, m, n);
    return z;
}

int flood_count(int i, int j, unsigned char * img, int x, int y, int m, int n){
    unsigned char * nimg=(unsigned char*)malloc(m*n*sizeof(unsigned char));
    int a, b;
    for (a=0; a<m; a++) {
        for (b=0; b<n; b++) {
            *(nimg+a*n+b)=*(img+a*n+b);
        }
    }
    flood_count_recursive(i, j, nimg, x, y, m, n);
    free(nimg);
    return n;
}


int is_8conn(int i, int j, unsigned char * img, int m, int n) {
    int r;
    int n_black=0;
    int x,y;
    for(y=i-1;y<i+2;y++)
    {
        for (x=j-1; x<j+2; x++) {
            if(*(img+y*n+x)==1){
                n_black++;
            }
        }
    }
    
    for(y=i-1;y<i+2;y++)
    {
        for (x=j-1; x<j+2; x++) {
            if(*(img+y*n+x)==1){
                int n=flood_count(i, j, img, x, y, m, n);
                if(n_black==n){
                    return 1;
                }
                else{
                    return 0;
                }
            }
        }
    }
    
    return -1;
}

unsigned char * zs_8conn(int m, int n, unsigned char * in){
    unsigned char * out = (unsigned char*)malloc(m*n*sizeof(unsigned char));
    int a;
    for(a=0; a < m*n; a++)
        out[a] = in[a]; // out = in
    
    int i,j;
    for(i=1;i<m-1;i++){
        for (j=1; j<n-1; j++){
            if (*(out+n*i+j)==1){
                if(zs_B(out, i, j, n)>2){
                    /* % if removing the point breaks 8-connectivity,
                     % don't actually remove the point*/
                    *(out+n*i+j)=0;
                    if(is_8conn(i, j, out, m, n) == 0){
                        *(out+n*i+j)=1;
                    }
                }
            }
        }
    }
    return out;
}

#endif
