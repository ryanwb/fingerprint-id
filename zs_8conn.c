//
//  zs_8conn.c
//  113DTesting
//
//  Created by s110773 on 16/4/15.
//  Copyright (c) 2015 EE113DB. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

//B Gets the Zhang-Suen B function value for a pixel img(i,j)
int B(int i, int j, int * img, int rMax){
    int b=0;
    b+= *(img+(i-1)*rMax+(j));
    b+= *(img+(i-1)*rMax+(j+1));
    b+= *(img+(i)*rMax+(j+1));
    b+= *(img+(i+1)*rMax+(j+1));
    b+= *(img+(i+1)*rMax+(j));
    b+= *(img+(i+1)*rMax+(j-1));
    b+= *(img+(i)*rMax+(j-1));
    b+= *(img+(i-1)*rMax+(j-1));
    return b;
}

int flood_count_recursive(int i, int j, int * img, int x, int y,int rMax, int r, int c);

int flood_count(int i, int j, int * img, int x, int y,int rMax, int r, int c){
    //y and x are current positions
    //i and j are center-of-frame positions
    int n;
    if(!(i-1<=y && y<= i+1)){
        n=0;
        return n;
    }
    if(!(j-1<=x && x<=j+1)){
        n=0;
        return n;
    }
    if(*(img+y*rMax+x)==0){
        n=0;
        return n;
    }
    int * nimg=(int*)malloc(r*c*sizeof(int));
    int l,m;
    for (l=0; l<r; l++) {
        for (m=0; m<c; m++) {
            *(nimg+l*c+m)=*(img+l*c+m);
        }
    }
    *(nimg+y*rMax+x)=0;
    n=flood_count_recursive(i, j, nimg, x, y, rMax, r, c);
    free(nimg);
    return n;
}

//recursive wrapper for the flood count function
int flood_count_recursive(int i, int j, int * img, int x, int y,int rMax, int r, int c){
    int n=1+ flood_count(i, j, img, x-1, y-1, rMax, r, c)+flood_count(i, j, img, x, y-1, rMax, r, c)
    +flood_count(i, j, img, x+1, y-1, rMax, r, c)+flood_count(i, j, img, x-1, y, rMax, r, c)
    +flood_count(i, j, img, x+1, y, rMax, r, c)+flood_count(i, j, img, x-1, y+1, rMax, r, c)
    +flood_count(i, j, img, x, y+1, rMax, r, c)+flood_count(i, j, img, x+1, y+1, rMax, r, c);
    return n;
}


int is_8conn(int i, int j, int * img, int rMax, int a, int b){
    int r;
    int n_black=0;
    int x,y;
    for(y=i-1;y<i+2;y++)
    {
        for (x=j-1; x<j+2; x++) {
            if(*(img+y*rMax+x)==1){
                n_black++;
            }
        }
    }
    
    for(y=i-1;y<i+2;y++)
    {
        for (x=j-1; x<j+2; x++) {
            if(*(img+y*rMax+x)==1){
                int n=flood_count(i, j, img, x, y, rMax, a, b);//a & b??
                if(n_black==n){
                    r=1;
                    return r;
                }
                else{
                    r=0;
                    return r;
                }
            }
        }
    }
    
    return r;
}

int * zs_8conn(int m, int n, int * in){
    int * out = (int*)malloc(m*n*sizeof(int));
    int a,b;
    for(a=0; a<n; a++){
        for(b=0; b<m; b++){
            *(out+(a*m)+b)=*(in+(a*m)+b);//out=in
        }
    }
    
    int i,j;
    for(i=1;i<m-1;i++){
        for (j=1; j<n-1; j++){
            if (*(out+n*i+j)==1){
                if(B(i, j, out, n)>2){
                    /* % if removing the point breaks 8-connectivity,
                     % don't actually remove the point*/
                    *(out+n*i+j)=0;
                    if(is_8conn(i, j, out, n, m, n)){ //NOTE: Not sure if the m, n are right parameters. need to test. I think needed for matrix copying in flood_count
                        *(out+n*i+j)=1;
                    }
                }
            }
        }
    }
    free(in);
    return out;
}
