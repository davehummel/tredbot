#include <Arduino.h>
 #include "quadfit.h"

void setup(){
	Serial.begin(115200);
	Serial1.begin(460800);

    QuadFit<double> qfit;
    qfit.setSampleCount(6);

	 //determine how many points used
        int size = 6;
       
        //Allocate memory for data[size][2]
        //data[3][0] -> x4, data[3][1] -> y4
        double **data;
        data = new double*[size];
        qfit.xData = new double[size];
        qfit.yData = new double[size];

        for(int i=0; i < size; i++)
                data[i] = new double[2];
 
        // sumx3 = (x0)^3 + (x1)^3 + ... + (x{size})^3
        double sumx4 = 0, sumx3 = 0, sumx2 = 0, sumx1 = 0, det;
       
        // Allocate memory for X^T
        double **transpose;
        transpose = new double*[3];
        for(int i=0; i < 3; i++)
                transpose[i] = new double[size];
 
        //Allocate memory for (X^T *X)^-1 * X^T
        double **stuff;
        stuff = new double*[3];
        for(int i=0; i < 3; i++)
                stuff[i] = new double[size];
 
        // y = a[0]x^2 + a[1]x + a[2]
        double a[3] = {0};
 
        //input data, do some precalculations at the same time to avoid making more loops
        for (int i=0; i < size; i++)
        {
      
                         data[i][0] = i;
                         data[i][1] = 1+i*i -3*i;
                         qfit.xData[i] = i;
                         qfit.yData[i] = 207;
                        //these computations only happen with x values
                     
                                //develops the sum values needed for inverse
                                sumx4 += data[i][0]*data[i][0]*data[i][0]*data[i][0];
                                sumx3 += data[i][0]*data[i][0]*data[i][0];
                                sumx2 += data[i][0]*data[i][0];
                                sumx1 += data[i][0];
                               
                                //develops transpose matrix
                                transpose[2][i] = 1;
                                transpose[1][i] = data[i][0];
                                transpose[0][i] = data[i][0]*data[i][0];
                     
                
        }
       
//After solving all the math
        //determinate
        det = (sumx4*sumx2*size) + (sumx3*sumx1*sumx2) + (sumx2*sumx3*sumx1) - (sumx2*sumx2*sumx2) - (sumx1*sumx1*sumx4) - (size*sumx3*sumx3);
        Serial1.print("sumx1 orig=");
        Serial1.println(sumx1);
        Serial1.print("sumx2 orig=");
        Serial1.println(sumx2);
        Serial1.print("sumx3 orig=");
        Serial1.println(sumx3);
        Serial1.print("sumx4 orig=");
        Serial1.println(sumx4);
        Serial1.print("det orig=");
        Serial1.println(det);

        qfit.processXData();
 
        //precalculated the inverse matrix to avoid numerical methods which take time or lose accuracy, NOTE: does not include division of determinate
        double inverse[3][3] = {
                {size*sumx2 - sumx1*sumx1, -(size*sumx3 - sumx1*sumx2), sumx1*sumx3-sumx2*sumx2},
                {-(size*sumx3-sumx2*sumx1), size*sumx4-sumx2*sumx2, -(sumx1*sumx4-sumx3*sumx2)},
                {sumx1*sumx3 - sumx2*sumx2, -(sumx1*sumx4 - sumx2*sumx3), sumx2*sumx4 - sumx3*sumx3}
        };

        Serial1.println(inverse[0][1]);
        Serial1.println(inverse[1][2]);
        //This is matrix multiplication for this particular pair of matrices
        for (int i=0; i < 3; i++)
        {
                for (int j=0; j < size; j++)
                {
                        stuff[i][j] = inverse[i][0]*transpose[0][j] + inverse[i][1]*transpose[1][j] + inverse[i][2]*transpose[2][j];
                }
        }

               Serial1.println(stuff[0][1]);
 
        //This is the final matrix multiplication that outputs a 1x3 matrix with our curve parameters
        for (int i=0; i < 3; i++)
        {
                for (int j=0; j < size; j++)
                {
                        a[i] += stuff[i][j]*data[j][1];
                }
                //dont forget to divide by determinate
                a[i] /= det;
        }
 
       // cout << endl << "a: " << a[0] << " b: " << a[1] << " c: " << a[2] << endl;
       Serial1.print("a:");
       Serial1.print(a[0]);
       Serial1.print("b:");
       Serial1.print(a[1]);
       Serial1.print("c:");
       Serial1.println(a[2]);

       qfit.solve();

       Serial1.print("a:");
       Serial1.print(qfit.c0);
       Serial1.print("b:");
       Serial1.print(qfit.c1);
       Serial1.print("c:");
       Serial1.println(qfit.c2);



              qfit.solve();

       Serial1.print("a:");
       Serial1.print(qfit.c0);
       Serial1.print("b:");
       Serial1.print(qfit.c1);
       Serial1.print("c:");
       Serial1.println(qfit.c2);
    

}


void loop(){

        
}



