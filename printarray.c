    #include <math.h>
    #include <stdio.h>
    
    void main(){
      	int x;
      	int y=0;
      	printf("const int arrTemp[256]={");
      	for(x=0;x<255;x++){
            float T20=20.0;
            float R20=11500.0;
            float Rext=46500.0;
       		float M=5.01-x/255.0*5.01;
            float RN=Rext*M/(5.01-M);
            float T0=T20+273.15;
            float H=3988.0+(T0*log(RN/R20));
            float T=(T0*3988.0/H)-273.15;
            int t=T*10;
            printf("%d,",t);
            y++;
            if (y==8) {
            y=0;
            printf(" \\\n");
            }
      }

      printf("0};\n");
    }