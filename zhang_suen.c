//
//  zhang_suen.c
//  113DTesting
//
//  Created by s110773 on 9/4/15.
//  Copyright (c) 2015 EE113DB. All rights reserved.
//
//RUSS: I started the C implenetation of the Z-S algorithm based on your Matlab Code, tried to
//keep all the variable names/ structure as close to the same as possible, lmk if you have questions

#include <stdio.h>
#include <stdlib.h>

//SUBCOND_ONE Determines if the first Zhang-Suen subcondition is true
int SUBCOND_ONE(int i , int j ,int rMax, int* img){//RUSS: rMax is to access the array, pointer shit
    int s=0;
    int a=A(img,i,j,rMax);
    int b=/*TODO: RUSS:function B*/;

    if (2<=b && b<=6 && a==1) {
        if((*(img+(i-1)*rMax+j)) *(*(img+(i*rMax)+(j+1))) * (*(img+((i+1)*rMax+j)))==0){
            if ( (*(img+(i*rMax)+(j+1))) * (*(img+(i+1)*rMax+j)) * (*(img+i*rMax+(j-1)))==0 ){
                s=1;
            }
        }
    }
    return s;
}

//SUBCOND_TWO Determines if the second Zhang-Suen subcondition is true
int SUBCOND_TWO(int i , int j ,int rMax, int* img){
    int s=0;
    int a=A(img,i,j,rMax);
    int b=/*TODO: RUSS:function B*/;
    if (2<=b && b<=6 && a==1) {
        if((*(img+(i-1)*rMax+j)) *(*(img+(i*rMax)+(j+1))) * (*(img+i*rMax+(j-1)))==0){
            if ( (*(img+(i-1)*rMax+j)) * (*(img+(i+1)*rMax+j)) * (*(img+i*rMax+(j-1)))==0 ){
                s=1;
            }
        }
    }
    return s;
}

//A Gets the Zhang-Suen A function value for a pixel img(i,j)
//NOTE:RUSS: apprently C99 has an implicit function called A(), might wanna consider renaming
int A(int * img, int i, int j, int rMax){
    int a=0;
    //% P2 -> P3
    if(*(img+(i-1)*rMax+j)==0 && *(img+(i-1)*rMax+(j+1))==1){
        a++;
    }
    //% P3 -> P4
    if(*(img+(i-1)*rMax+(j+1))==0 && *(img+i*rMax+(j+1))==1){
        a++;
    }
    //% P4 -> P5
    if(*(img+(i)*rMax+(j+1))==0 && *(img+(i+1)*rMax+(j+1))==1){
        a++;
    }
    //% P5 -> P6
    if(*(img+(i+1)*rMax+(j+1))==0 && *(img+(i+1)*rMax+(j))==1){
        a++;
    }
    //% P6 -> P7
    if(*(img+(i+1)*rMax+(j))==0 && *(img+(i+1)*rMax+(j-1))==1){
        a++;
    }
    //% P7 -> P8
    if(*(img+(i+1)*rMax+(j-1))==0 && *(img+(i)*rMax+(j-1))==1){
        a++;
    }
    //% P8 -> P9
    if(*(img+(i)*rMax+(j-1))==0 && *(img+(i-1)*rMax+(j-1))==1){
        a++;
    }
    //% P9 -> P2
    if(*(img+(i-1)*rMax+(j-1))==0 && *(img+(i-1)*rMax+(j))==1){
        a++;
    }
    return a;
}

int B(int* img,int i, int j,int rMax)
{
   int b=0;
    //TODO: Russ: Implement function B
    return b;
}



int * zhang_suen(int m, int n, int * img){
    int * thin = (int*)malloc(m*n*sizeof(int));
    
    //thin = img;
    int a,b;
    for(a=0; a<n; a++){
        for(b=0; b<m; b++){
            *(thin+(a*m)+b)=*(img+(a*m)+b);
        }
    }
    
    int did_change=1;
    
    //Note: pts_to_remove will be a linked list in C? or can just use an array
    //RUSS: I started to implement this in an array, but can be changed if linked list ends up being better
    while(did_change==1){
        int p=1;
        did_change=0;
        int * pts_to_remove= (int*)malloc(m*n*sizeof(int)*2);
        //RUSS: not sure what the 2 is for, but included it anyways for now
        int i,j;
        //RUSS:ZERO OUT THE MATRIX
        for(i=0;i<2;i++){
            for(j=0;j<m*n;j++){
                *(pts_to_remove+ i*(m*n)+j)=0;
            }
        }
        
        //Do the first subiteration
        
        for(i=1;i<m-1;i++){
            for (j=1; j<n-1; j++){
                if (*(thin+(i*n)+j)==1 &&/*TODO: RUSS:SUBCONDONE FUNC*/) {
                    //TODO: UPDATE pts_to_remove
                }
            }
        }
        //TODO:    % Remove points found in first subiteration

        //TODO:    % Do the second subiteration

        //TODO:    % Remove points found in second subiteration
    
        //TODO: UPDATE THIN
        
        free(pts_to_remove);//RUSS:free here?? (just put this here for now as a reminder)
    }
    //TODO RUSS:DO I NEED TO FREE *img(input) ??
    return thin;
}
