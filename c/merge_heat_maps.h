#ifndef MERGE_H
#define MERGE_H

#include "heat.h"

//THE NEW CONTRIBUTING HEAT MAP DOEES NOT GET DELETED IN THIS FUNCTION AFTER THE MAPS ARE MERGED
heat_t * merge_heat_maps(heat_t * o_heat, heat_t * n_heat){
	int new_count=o_heat->count+ n_heat->count;
	int m= o_heat->height;
	int n= o_heat->width;
	int i,j;
	for(i=0; i<m; i++){
		for(j=0; j<n; j++){
			o_heat->map.ridgeending[i*n+j]=(o_heat->count/new_count)*o_heat->map.ridgeending[i*n+j]
			+(n_heat->count/new_count)*n_heat->map.ridgeending[i*n+j];
			o_heat->map.bifurcation[i*n+j]=(o_heat->count/new_count)*o_heat->map.bifurcation[i*n+j]
			+(n_heat->count/new_count)*n_heat->map.bifurcation[i*n+j];
			o_heat->map.crossing[i*n+j]=(o_heat->count/new_count)*o_heat->map.crossing[i*n+j]
			+(n_heat->count/new_count)*n_heat->map.crossing[i*n+j];
		}
	}
	o_heat->count=new_count;
	return o_heat;
}

#endif
