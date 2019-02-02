#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <mpi.h>

#define EULER_GAMMA_CONST 0.57721566490153286060651209008240243

int main(int argc, char **argv)
{
  int procID;
  int numOfProcs = 4; //used instead 'p' variable. easier to refactor and to understand in the future
  int numOfSteps = 0; //used instead 'n' variable
  double start_t, end_t;
  
  MPI_Init(&numOfProcs, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &procID);
  MPI_Comm_size(MPI_COMM_WORLD, &numOfProcs);

  if (!procID)
  {
    printf("Enter n : ");
    fflush(stdout);
    while (numOfSteps < 3)
    {
      scanf("%d", &numOfSteps);
    }
  
  }
  start_t = MPI_Wtime();
  MPI_Bcast(&numOfSteps, 1, MPI_INT, 0, MPI_COMM_WORLD);

  int count = numOfSteps / numOfProcs;
  int start = count * procID;
  int end = count * procID + count;

  //if numOfSteps is not divisable on numOfProcs -> add modulo result to last Process
  if (numOfSteps % numOfProcs != 0 )
  {
    if (procID == numOfProcs - 1)
      end += (numOfSteps % numOfProcs);
  }

  double partialSum = 0, sumOfSeries = 0;
  double temp = 0;
  int i;
  if(procID == 0) start = 1; //using i
  for (i = start; i <= end; ++i)
  {
    partialSum += 1. / (double)i;
  }

  MPI_Reduce(&partialSum, &sumOfSeries, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if (!procID)
  {
    double result = sumOfSeries - log((double)numOfSteps);
    printf("\nEuler Gamma Constant:   %.16f\n", result);
    printf("Absolute Error: %.16f\n", (EULER_GAMMA_CONST - result));
    printf("Relative error: %.16f\n", ((EULER_GAMMA_CONST - result) / EULER_GAMMA_CONST));
  }
  end_t = MPI_Wtime();
  MPI_Finalize();

  if(procID == 0) printf("Computation time  = %lf\n", end_t - start_t);

}