#include <stdio.h>
#include <stdlib.h>
#define MAX_NUM 500// maximum array size
#include<math.h>
//definitions
//define array of structures containing the user's inputs
typedef struct
{
    double Q;
    double cin;
    double c0;
    double tf;
    double V;
    double dt;
    int open;

} USERINPUT;
//function prototypes
void getUserInput(USERINPUT *);
void UsingEulersMethod(USERINPUT, double *, double *);
void AnalyticalMethod(USERINPUT, double *);
int useSave(USERINPUT *, USERINPUT *);
void manualInput(USERINPUT *);
void exportToFile(FILE **, USERINPUT *, USERINPUT *);
//main
int main()
{
    USERINPUT values;
    getUserInput(&values);
    double arrayEuler[MAX_NUM];// array for storing the values of the concentration using Euler's mehtod.
    double arrayAnalytical[MAX_NUM];//array for storing the values of the concentration using analytical method.
    double ListOfTimeValues[MAX_NUM];// Also, declare a list of times array
    UsingEulersMethod(values, arrayEuler, ListOfTimeValues);// passes the structure variable and the array for time and concentration using Euler's method
    AnalyticalMethod(values, arrayAnalytical);// passes the array for analytical method and the structure variable.
    int i, n;
    n=(int)ceil((values.tf)/(values.dt));// rounds up the value of n and casts it to an int type.
    printf("These are the values that were input:\n");
    printf("rate flow: %lf, inflow concentration: %lf, intial concentration: %lf\n", values.Q, values.cin, values.c0);
    printf("final time: %lf, volume: %lf, delta-t: %lf\n", values.tf, values.V, values.dt);
    printf("\n");
   printf("Time(min)\tConcentration(Euler)(mg/m^3)\tConcentration(Analytical)(mg/m^3)\n");
   for(i=0; i<=n; i++)
       printf("%lf\t\t%lf\t\t%lf\n", ListOfTimeValues[i], arrayEuler[i], arrayAnalytical[i]);// prints the values in the array for plotting.
    return 0;
}

/*------------------------------------------------------
Function: getUserInput
Parameters: values: address of user-defined values declared in main
Description: function opens stream to file and fills an array of USERINPUT
              type structs with information, if it exists. Array indices with
              valid information are printed, and the user is asked if they want
              to use previous values. If so, these values are given to values
              pointer ad the function returns. Otherwise, they are asked to
              input values manually. They are then asked if they want to export
              these new values. If yes, they are put into the array of structures
              which is then written to a binary file.
-----------------------------------------------------------*/

void getUserInput(USERINPUT *values)
{
    //declarations
    USERINPUT var[5];
    FILE *fp = fopen("savedata.bin", "rb");
    int i, n, useSlot;
    //check for existing savefile
    if (fp != NULL)
    {
        rewind(fp);
        for (i=0; i < 5 ; i++)//set the savefile values to the array var[]
        {
            fscanf(fp, "%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n", &var[i].Q, &var[i].cin, &var[i].c0, &var[i].tf, &var[i].V, &var[i].dt);
        }

        for (i = 0; i < 5; i++)  //verify if any array is "open" and change that structure's "open" token, then print its contents
        {
            if (var[i].Q!=0 && var[i].cin!=0 && var[i].tf!=0 && var[i].dt!=0 && var[i].V != 0)
                var[i].open = 0;
            else
                var[i].open = 1;
            if (var[i].open == 0)//print contents of array; "open" if there's nothing there
            {
                printf("***********************************************\n");
                printf("Saved set: %d\n", i);
                printf("Flow: %lf\n", var[i].Q);
                printf("Inflow concentration: %lf\n", var[i].cin);
                printf("Initial reactor concentration: %lf\n", var[i].c0);
                printf("Time interval: [0, %lf]\n", var[i].tf);
                printf("Time interval increments: %lf\n", var[i].dt);
                printf("Reactor volume: %lf\n", var[i].V);
                printf("***********************************************\n\n");
            }
            else
                printf("Slot [%d] open\n\n", i);
        }
        useSlot = useSave(var, values);//decide whether or not to use save or manual input
        if (useSlot == 1)
          return;//return if previously saved values are used

    }
    manualInput(values);
    exportToFile(&fp, var, values);
    return;//return

}
/*-----------------------------------------------------------------------------
    Function: UsingEulersMethod
    Parameters:
            var: a copy of the structure variable.
                Members used- tf: the final time for the study of the reactor.
                              dt: the delta t variable.
                              Q: the flow rate.
                              cin: the inflow substance concentration.
                              c0: the intial concetration.
                              V: the volume of the reactor.
            ArrayEuler: reference to the array for saving concentration values using Euler's method.
            ListOfTimeValues: reference to array for saving time values.
    Description: Fills in the array with (tf/dt) points of time and concentration values using
                 Euler's mehtod.
--------------------------------------------------------------------------------*/
void UsingEulersMethod(USERINPUT values, double *ArrayEuler, double *ListOfTimeValues)
{
    double ci, cprev;// ci is the concentation at time t and cprev is the concentration at a previous time t-1.
    int n;// the value for the number of sub-intervals
    n= (int)ceil((values.tf)/(values.dt));// rounds up the value of the n and casts it to an int type.

    int i;
    cprev=values.c0;// assigns the initial concentration of the substance to cprev.
    double t=0;// a variable for time
    for(i=0; i<=n; i++)
    {
        ci= cprev+(((values.Q*values.cin)-(values.Q*cprev))/(values.V))*(values.dt);// this gets the value for the concentartion of the substance at time t, using the concentration of the substance at time t-1.
        ArrayEuler[i]= ci;//assign concentration values to the array for storing concentration using Euler's method.
        ListOfTimeValues[i]=t;//assign time values to the array for storing time values.
        t=t+values.dt;// increments the time t.
        cprev=ci;// updates the value of cprev.
    }
}
/*-----------------------------------------------------------------------------
    Function: AnalyticalMethod
    Parameters:
            var: a copy of the structure variable values.
                Members used- tf: the final time for the study of the reactor.
                              dt: the delta t variable.
                              Q: the flow rate.
                              cin: the inflow substance concentration.
                              c0: the intial concetration of the substance.
                              V: the volume of the reactor.
            ArrayAnalytical: reference to the array for storing concentration values using analytical method.
    Description: Fills in the array with (tf/dt) points of concentration values using
                 the analytical solution.
--------------------------------------------------------------------------------*/
void AnalyticalMethod(USERINPUT values, double *ArrayAnalytical)
{
    int i, n;
    double ct;// the concentration of the substance in the reactor at time t.
    double t;// time variable.
    t=0.0;
    n= (int) ceil((values.tf)/(values.dt));
    for(i=0; i<=n; i++)
    {
        ct= (values.cin)*(1-exp(-((values.Q)/(values.V))*t)) + (values.c0)*(exp(-((values.Q)/(values.V))*t));// this calculates the concentration of the substance in the reactor at time t.
        ArrayAnalytical[i]=ct;// assigns the value of concentration at time t to ArrayAnalytical.
        t=t+values.dt;// increments the time t.
    }
}
/*----------------------------------------------------------------
Function: useSave()
Parameters: var[5] : address of array containing values loaded from binary FILE
            values: address of values declared in main
Description: Function takes user input, deciding whether to use previously
              saved values, which have been printed in calling function. Returns
              1 or 0, which will further instruct calling function on whether to continue
              or to return to main.
-----------------------------------------------------------------*/
int useSave(USERINPUT var[5], USERINPUT *values)
{
{
  int useSave = 2, i;
  while (useSave != 0 && useSave != 1)
  {
      printf("Would you like to use one of these values? 1:yes 0:no \n"); //ask if they want to use saved values
      scanf("%d", &useSave);
  }
  if (useSave == 1)
  {
      do{
      printf("Which save slot would you like to use?\n");//if yes, set var[i] for whichever i is chosen to  the parameter address and RETURN (switch tree? in a new function?)
      scanf("%d", &i);
    } while (var[i].open == 1);
      values->Q = var[i].Q;
      values->cin = var[i].cin;
      values->c0 = var[i].c0;
      values->tf = var[i].tf;
      values->dt = var[i].dt;
      values->V = var[i].V;
      return 1;
  }
  else
    return 0;
}
}
/*---------------------------------------------------------
Function: manualInput
Parameters: values: address of values declared in main Function
Desctiption: Function assigns address of values to hold user input. These values
            are checked to see if they are positive and if the number of increments
            used will be within the max alloted number.
-------------------------------------------------------------*/
void manualInput(USERINPUT *values)
{
  int check;
  do
  {
      check = 0;
      printf("Please input the values you would like to use:\n"); //ask user for inputs, set to Values. only accept values => 0
      printf("Flow (Q): \n");
      scanf("%lf", &values->Q);
      printf("Concentration of inflow (cin): \n");
      scanf("%lf", &values->cin);
      printf("Initial reactor concentration (c0): \n");
      scanf("%lf", &values->c0);
      printf("End of time period observed (tf): \n");
      scanf("%lf", &values->tf);
      printf("Time increment (dt): \n");
      scanf("%lf", &values->dt);
      printf("Reactor volume (V): \n");
      scanf("%lf", &values->V);
      if(((values->tf)/(values->dt)) > MAX_NUM)
          printf("Time step too small for final time (%lf)\n", values->dt);
      else
        check++;
      if (values->Q <= 0 || values->V <= 0 || values->tf <=0 || values->dt <= 0)
        printf("Flow, time period, time increment and reactor volume must be >0\n");
      else
        check++;

  }
  while(check < 2);
  return;
}

/*-----------------------------------------------------------
Function: exportToFile
Parameters: **stream: address of pointer to stream, used to write to FILE
            var[5]: array containing previously saved data. Written to stream
            values: pointer to values stored in main.
Description: This function takes the values the user has written into *values and asks if
            they want to save them to the file. If yes, they are asked which "slot" they wish
            to overwrite. That index i of the array var[i] is overwritten with that information.
            Then the array var[], which has the new inputs and the previously loaded ones, is
            written to the stream.
--------------------------------------------------------------*/
void exportToFile(FILE **stream, USERINPUT var[5], USERINPUT *values)
{
  int useSave = 2, i;
  while (useSave != 1 && useSave!=0)
  {
      printf("Would you like to save these numbers? 1-yes 0-no\n");//ask user if they want to save numbers
      scanf("%d", &useSave);
  }
  if (useSave == 1)//if yes
  {
      useSave = 5; //ask which index i they want to overwrite
      while (useSave != 0 && useSave != 1 && useSave != 2 && useSave != 3 && useSave != 4)
      {
          printf("Which slot would you like to save in: \n");
          scanf("%d", &useSave);
      }
      var[useSave].Q = values->Q;//sets the Values into that index at var[useSave]
      var[useSave].cin = values->cin;
      var[useSave].c0 = values->c0;
      var[useSave].tf = values->tf;
      var[useSave].dt = values->dt;
      var[useSave].V = values->V;




  *stream = fopen("savedata.bin", "w");//open stream to savefile with mode write binary
  for (i = 0; i < 5; i++)
    {
      fprintf(*stream, "%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n", var[i].Q, var[i].cin, var[i].c0, var[i].tf, var[i].V, var[i].dt);//write all structures from var[i] into savefile (formatiing?)
    }
  }
  fclose(*stream);//close stream

  return;
}
