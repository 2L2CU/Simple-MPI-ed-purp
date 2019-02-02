#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <mpi.h>



int main(int argc, char **argv)
{
  int procID;
  int numOfProcs = 4; //used instead 'p' variable. easier to refactor and to understand in the future
  int numOfSteps = 0; //used instead 'n' variable
  
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numOfProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &procID);

  //process 0 gets required elements of Leibniz Series
  if (!procID)
  { 
    printf("Number of numOfSteps: ");
    fflush(stdout);
    while (numOfSteps < 3)
    {
      scanf("%d", &numOfSteps);
    }
  }

  MPI_Bcast(&numOfSteps, 1, MPI_INT, 0, MPI_COMM_WORLD);

  int count = numOfSteps / numOfProcs;
  int start = count * procID;
  int end = count * procID + count;

  //deals with division of series elements calculations between processes
  if (numOfSteps % numOfProcs != 0) 
  {
    if (procID == numOfProcs -1 ) end += (numOfSteps % numOfProcs); 
  }

  double processSum = 0, sumOfSeries = 0;
  sleep(0.1);
  printf("\nProcess %d calculates %d series elements, start = %d end = %d\n", procID, end - start, start, end);
  for (int i = start; i < end; ++i)
  {

    processSum += pow(-1, i) / (2 * i + 1);
  }

  MPI_Reduce(&processSum, &sumOfSeries, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if (!procID)
  {
    printf("PI:   %.16f\n", sumOfSeries *4.0); //Leibniz Series results with PI/4 
    printf("Absolute Error: %.16f\n", M_PI_4 - sumOfSeries);
    printf("Relative error: %.16f\n", (M_PI_4 - sumOfSeries)/M_PI_4);
  }

  MPI_Finalize();
}