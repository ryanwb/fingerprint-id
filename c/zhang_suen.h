#include <stdio.h>
#include <stdlib.h>

// zs_A Gets the Zhang-Suen A function value for a pixel img(i,j)
int zs_A(unsigned char* img, int i, int j, int rMax) {
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

int zs_B(unsigned char* img,int i, int j,int rMax) {
    int b=0;
    b=b+ *(img+(i-1)*rMax+(j));
    b=b+ *(img+(i-1)*rMax+(j+1));
    b=b+ *(img+(i)*rMax+(j+1));
    b=b+ *(img+(i+1)*rMax+(j+1));
    b=b+ *(img+(i+1)*rMax+(j));
    b=b+ *(img+(i+1)*rMax+(j-1));
    b=b+ *(img+(i)*rMax+(j-1));
    b=b+ *(img+(i-1)*rMax+(j-1));
    
    return b;
}

//SUBCOND_ONE Determines if the first Zhang-Suen subcondition is true
int SUBCOND_ONE(int i , int j ,int rMax, unsigned char* img) {//RUSS: rMax is to access the array
    int s=0;
    int a=zs_A(img,i,j,rMax);
    int b=zs_B(img,i,j,rMax);
    
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
int SUBCOND_TWO(int i , int j ,int rMax, unsigned char* img) {
    int s=0;
    int a=zs_A(img,i,j,rMax);
    int b=zs_B(img,i,j,rMax);
    if (2 <= b && b <= 6 && a == 1) {
        if((*(img+(i-1)*rMax+j)) *(*(img+(i*rMax)+(j+1))) * (*(img+i*rMax+(j-1)))==0){
            if ( (*(img+(i-1)*rMax+j)) * (*(img+(i+1)*rMax+j)) * (*(img+i*rMax+(j-1)))==0 ){
                s=1;
            }
        }
    }
    return s;
}

unsigned char* zhang_suen(int m, int n, unsigned char* img) {
    unsigned char* thin = (unsigned char*)malloc(m*n*sizeof(unsigned char));
    
    //thin = img;
    int a,b;
    for(a=0; a<n; a++){
        for(b=0; b<m; b++){
            *(thin+(a*m)+b)=*(img+(a*m)+b);
        }
    }
    
    int did_change = 1;    
    //Note: pts_to_remove will be a linked list in C? or can just use an array
    //RUSS: I started to implement this in an array, but can be changed if linked list ends up being better
    while(did_change== 1){
        int p = 1;
        did_change = 0;
        int * pts_to_remove= (int*)malloc(m*n*sizeof(int)*2);
        int i,j;
        //RUSS:ZERO OUT THE MATRIX
        for(i=0;i<m*n;i++){
            for(j=0;j<2;j++){
                *(pts_to_remove+ i*2+j) = 0; //Note: Not sure if this loop dimensions is correct
            }
        }
        
        //Do the first subiteration
        
        for(i=1;i<m-1;i++){
            for (j=1; j<n-1; j++){
                if (*(thin+(i*n)+j)==1 && SUBCOND_ONE(i, j, n, thin)== 1) {
                    *(pts_to_remove+p*2) = i;
                    *(pts_to_remove+p*2+1) = j;
                    did_change=1;
                    p++;
                }
            }
        }
        //Remove points found in the first subiteration
        for(i=0; i<p-1;i++){
            int k = pts_to_remove[p*2];
            int l = pts_to_remove[p*2+1];
            thin[k*2 + l] = 0;
        }
        
        p=1;
        
        //Do the second subiteration
        for(i=1;i<m-1;i++){
            for(j=1;j<n-1;j++){
                if (*(thin+(i*n)+j)==1 && SUBCOND_TWO(i, j, n, thin)== 1) {
                    *(pts_to_remove+p*2) = i;
                    *(pts_to_remove+p*2+1) = j;
                    did_change = 1;
                    p++;
                }
            }
        }
        
        //:Remove points found in second subiteration
        for(i=0; i<p-1;i++){
            int k = pts_to_remove[p*2];
            int l = pts_to_remove[p*2+1];
            thin[k*2 + l] = 0;
            
        }
        free(pts_to_remove);
    }
    return thin;
}
