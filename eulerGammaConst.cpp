#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <mpi.h>

#define EULER_GAMMA_CONST 0.57721566490153286060651209008240243

int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);

  int numOfProcs, procID;
  double start_t, end_t;
  MPI_Comm_size(MPI_COMM_WORLD, &numOfProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &procID);

  int numOfSteps = 0; //how many addends Leibniz series will have

  //process 0 gets required elements of Leibniz Series
  if (!procID)
  {
    printf("Enter n: ");
    fflush(stdout);
    while (numOfSteps < 3)
    {
      scanf("%d", &numOfSteps);
    }
  }
  start_t = MPI_Wtime();
  MPI_Bcast(&numOfSteps, 1, MPI_INT, 0, MPI_COMM_WORLD);

  //Divides elements to calculate between processes, rest of divisoin is added to last process
  int count = numOfSteps / numOfProcs;
  int start = count * procID;
  int end = count * procID + count;
  if (numOfSteps % numOfProcs != 0)
  {
    if (procID == numOfProcs - 1)
      end += (numOfSteps % numOfProcs);
  }

  double partialSum = 0, sumOfSeries = 0;
  double temp = 0;
  int i = 0;

  //printf("\nProcess %d calculates %d series elements, start = %d end = %d\n", procID, end - start, start, end);

  if (start == 0)
    start = 1;
  for (i = start; i < end; ++i)
  {
    partialSum += 1. / (double)i;
  }
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Reduce(&partialSum, &sumOfSeries, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if (!procID)
  {
    double result = sumOfSeries - log((double)numOfSteps);
    printf("\nEuler Gamma Constant:   %.16f\n", result);
    printf("\nAbsolute Error: %.16f\n", (EULER_GAMMA_CONST - result));
    printf("\nRelative error: %.16f\n", ((EULER_GAMMA_CONST - result) / EULER_GAMMA_CONST));
  }

  MPI_Finalize();

  if (!procID)
  {
    end_t  = MPI_Wtime();
    printf("\n TIME OF EXECUTION %.16lf\n", end_t - start_t);
  }
}
