/***************************************************************************//**
 * @file 
 * @brief determines if a batch of routers should be accepted or rejected
 *
 * @mainpage Program 3 - Unit Testing
 *
 * @section course_section Course Information
 *
 * @authors Christian Fattig
 *
 * @date May 1, 2015
 *
 * @par Instructor:
 *         Dr. Qiao
 *
 * @par Course:
 *         CSC 150 - Section 3 - 2:00 pm
 *
 * @par Location:
 *         McLaury - Room 205
 *
 * @section program_section Program Information
 *
 * @details This program fills a 2D array with data from an input file which 
 * contains list of 5 columns of numbers used to determine if a batch of 100
 * or less routers can be accepted or rejected. The program uses a series of 
 * arithmatic equations to determine a number for the 5 categories being tested
 * and then compares the numbers to pre-determined constants to see if they pass
 * inspection or not.
 *
 * At this point, the program creates an output file and sends the data to this
 * determining if the batch of routers passed or failed each individual category
 * and whether it ultimately passed or failed.
 *
 * @section compile_section Compiling and Usage
 *
 * @par Compiling Instructions:
 *      None
 *
 * @par Usage:
   @verbatim
   c:\> prog3.exe
   d:\> c:\bin\prog3.exe
   @endverbatim
 *
 * @section todo_bugs_modification_section Todo, Bugs, and Modifications
 *
 * @bug None
 *
 * @todo None
 *
 * @par Modifications and Development Timeline:
   @verbatim
   Date          Modification
   ------------  ---------------------------------------------------------------
   Apr 15, 2015  Created an outline for the program.
   Apr 18, 2015  Started building basic structure for the program.
   Apr 26, 2015  Built functions and finished most of program.
   Apr 27, 2015  Finished program (except for doxygen).
   Apr 30, 2015  Finished doxygen.
   @endverbatim
 *
 ******************************************************************************/


//***libraries***//
#include<iostream>
#include<fstream>
#include<cmath>
#include<iomanip>
#include<cstring>

using namespace std;

/*******************************************************************************
 *             Constant Variables, defines and Enums
 ******************************************************************************/
const double MAX_VOLTAGE_VARIATION = 0.2;//Max variation allowed for IV
const double MAX_GAIN_VARIATION = 1.5;//Max variation allowed for A1 and A2
const double MAX_PERCENTAGE = 1.0;//Max percentage allowed for MT24 and MT5

/*******************************************************************************
 *                         Function Prototypes
 ******************************************************************************/
int read_measurements(ifstream &fin, double data[][5], int &lotnumber);
double calc_A_stdev(double data[][5], int rows);
double calc_G_stdev(double data[][5], int rows, int column);
double calc_percentage(double data[][5], int rows, int column);
int calc_pass_fail(double IV, double A1, double A2, double MT24, double MT5);
void create_report(ofstream &fout, int result, int lotnumber, double IV
				   , double A1, double A2, double MT24, double MT5);

/***************************************************************************//**
 * @author Christian Fattig
 *
 * @par Description:
 * This function recieves and opens an input file and gives an error message
 * if the file cannot be opened. It also opens a an output file.
 * This function calls a series of other functions and when finished, it
 * closes both files.
 *
 * @param[in] argc - the number of arguments from the command prompt.
 * @param[in] argv - a 2d array of characters containing the arguments.
 *
 * @returns 0 program ran successfully
 * @returns -1  an error occured
 ******************************************************************************/
int main(int argc, char *argv[])
{
	double data[100][5];// A 2D array that stores data from an input file
	char name[50]="report_";// An array use to create a desired output file name
	int result = 0;// Determines if batch should be accepted or rejected
	int rows = 0;// Number of rows in the batch
	int lotnumber = 0;// The lotnumber of the batch
	double IV = 0.0;// Determined IV value for batch
	double A1 = 0.0;// Determined A1 value for batch
	double A2 = 0.0;// Determined A2 value for batch
	double MT24 = 0.0;// Determined MT24 value for batch
	double MT5 = 0.0;// Determined MT5 value for batch


	strcat_s(name, argv[2]);// Used to appened name for output file

	// Tests to see if there are 3 command arguments
	if(argc!=2)
	{
		cout<<"Error, structure should be as follows: ";
		cout<<"prog3.exe sample-pass.txt"<<endl;
		return -1;
	}

	ifstream fin;// Input file declaration
	ofstream fout;// Output file declaration
	fin.open(argv[1]);// Opens input file
	fout.open(argv[2]);// Opens output file

	// Tests to see if a file does not open
	if(!fin)
	{
		cout<<"Error, 1 or more files did not open."<<endl;
		fin.close();
		fout.close();
		return 0;
	}

	rows = read_measurements(fin, data, lotnumber);// Fills 2D array
	IV = calc_A_stdev(data, rows);// Calculates IV
	A1 = calc_G_stdev(data, rows, 1);// Calculates A1
	A2 = calc_G_stdev(data, rows, 2);// Calculates A2
	MT24 = calc_percentage(data, rows, 3);// Calculates MT24
	MT5 = calc_percentage(data, rows, 4);// Calculates MT5
	result = calc_pass_fail(IV,A1,A2,MT24,MT5);// Calculates result (pass/fail)
	fout<< fixed << showpoint << setprecision(4);
	create_report(fout, result, lotnumber, IV, A1, A2, MT24, MT5);
	// Fills the output file

	fin.close();// Closes input file
	fout.close();// Closes output file

	return 0;
}


/**************************************************************************//** 
 * @author Christian Fattig
 * 
 * @par Description: 
 * This function recieves an open input file and uses it to fill a 2D array
 * as well as determining the lot number.
 * 
 * @param[in]      fin - an opened input file
 * @param[in,out]  data - a 2D array filled with info from the input file
 * @param[in,out]  lotnumber - the number assigned to the router batch
 * 
 * @returns rows The number of rows in the input file
 *****************************************************************************/
int read_measurements(ifstream &fin, double data[][5], int &lotnumber)
{
	int rows=0;// Number of rows in file
	fin >> lotnumber;// Inputs lotnumber from file
	while(rows<100&&!fin.eof())// Fills the 2D array with data from the file
	{
		fin >> data[rows][0];
		fin >> data[rows][1];
		fin >> data[rows][2];
		fin >> data[rows][3];
		fin >> data[rows][4];
		rows++;

	}

	return rows;
}


/**************************************************************************//** 
 * @author Christian Fattig
 * 
 * @par Description: 
 * This function uses an arithmatic equation and values from the 2D array to 
 * find IV which is then returned to the main function.
 * 
 * @param[in,out]  data - a 2D array filled with info from the input file
 * @param[in]      rows - the number of rows in the 2D array
 * 
 * @returns answer The value which is IV
 *****************************************************************************/
double calc_A_stdev(double data[][5], int rows)
{
	double average = 0.0;// The average of all the numbers in the column
	double sum = 0.0;// The sum of all the numbers in the column
	double answer = 0.0;// The variable used to store IV

	for(int i=0;i<rows;i++)// Finds the sum
	{
		average = 0.0;
		average = data[i][0];
		sum += average;
	}
	average = sum / rows;// Calculates the average
	sum = 0.0;
	for(int i=0;i<rows;i++)// Equation used to find IV
	{
		answer = 0.0;
		answer = pow( data[i][0] - average , 2 );
		sum += answer;
	}
	answer = 0.0;
	answer = sqrt(sum / rows);

	return answer;
}


/**************************************************************************//** 
 * @author Christian Fattig
 * 
 * @par Description: 
 * This function uses an arithmatic equation and values from the 2D array to 
 * find A1 and A2 which are then returned to the main function.
 * 
 * @param[in,out]  data - a 2D array filled with info from the input file
 * @param[in]      rows - the number of rows in the 2D array
 * @param[in]      column - the column of numbers being used
 * 
 * @returns answer The value which is A1 or A2
 *****************************************************************************/
double calc_G_stdev(double data[][5], int rows, int column)
{
	double product = 1.0;// Total product of the numbers in the column
	double answer = 0.0;// Variable that stores A1 or A2
	double sum = 0.0;// Calculates the sum of values

	for(int i=0;i<rows;i++)// Finds total product
	{
		product = product * data[i][column];
	}
	answer = pow( product , ( 1.0 / rows ) );// Finds geometric mean
	for(int i=0;i<rows;i++)// Equation used to calculate A1 or A2
	{
		product = pow( log10(data[i][column] / answer), 2.0 );
		sum = sum + product;
	}
	answer = 0.0;
	answer = pow( 10.0 , sqrt(sum / rows) );
	
	return answer;
}


/**************************************************************************//** 
 * @author Christian Fattig
 * 
 * @par Description: 
 * This function calculates the percent of 0's in the column by adding all 
 * the 0's, dividing by the total number of numbers in the column and then
 * multiplying by 100.
 * 
 * @param[in,out]  data - a 2D array filled with info from the input file
 * @param[in]      rows - the number of rows in the 2D array
 * @param[in]      column - the column of numbers being used
 * 
 * @returns answer The value which is MT24 and MT5
 *****************************************************************************/
double calc_percentage(double data[][5], int rows, int column)
{
	double answer = 0.0;// Holds MT24 or MT5
	double sum = 0.0;// Sum of all 0's in column
	for(int i=0;i<rows;i++)// Used to find sum of 0's
	{
		answer = data[i][column];
		if(answer==0)
			sum++;
	}
	answer = ( sum / rows ) * 100;// Calculates percent of 0's

	return answer;
}


/**************************************************************************//** 
 * @author Christian Fattig
 * 
 * @par Description: 
 * This function compares values found for each column to the max variances 
 * allowed to see determine if the batch should be accepted or rejected.
 * 
 * @param[in]      IV - value found for the first column
 * @param[in]      A1 - value found for the second column
 * @param[in]      A2 - value found for the third column
 * @param[in]      MT24 - value found for the fourth column
 * @param[in]      MT5 - value found for the fifth column
 * 
 * @returns -1 batch should be rejected
 * @returns 0  batch should be accepted
 *****************************************************************************/
int calc_pass_fail(double IV, double A1, double A2, double MT24, double MT5)
{
	// Compares values to the max variances allowed
	if(IV>MAX_VOLTAGE_VARIATION||A1>MAX_GAIN_VARIATION||A2>MAX_GAIN_VARIATION
		||MT24>MAX_PERCENTAGE||MT5>MAX_PERCENTAGE)
		return -1;
	else
		return 0;
}


/**************************************************************************//** 
 * @author Christian Fattig
 * 
 * @par Description: 
 * This function uses information previously determined and writes it to an
 * output file where it is shown what catagories have passes and which of 
 * them have failed. It also shows whether the lot is to be accepted or
 * whether it should be rejected.
 * 
 * @param[in,out]  fout - output file where results are written
 * @param[in]      result - determines if batch should be accepted or rejected
 * @param[in]      lotnumber - holds the number assigned to a certain batch
 * @param[in]      IV - value found for the first column
 * @param[in]      A1 - value found for the second column
 * @param[in]      A2 - value found for the third column
 * @param[in]      MT24 - value found for the fourth column
 * @param[in]      MT5 - value found for the fifth column
 * 
 * @returns none
 *****************************************************************************/
void create_report(ofstream &fout,int result,int lotnumber, double IV
				   , double A1, double A2, double MT24, double MT5)
{
	char action[10];// Used to display action taken (accept/reject or pass/fail)
	

	fout <<"Lot ID: "<<lotnumber<<endl<<endl;

	if(IV<MAX_VOLTAGE_VARIATION)// IV results
		strcpy_s(action,"Pass");
	else
		strcpy_s(action,"Fail");


	fout<<"Standard Deviation of IV:  "<<IV<<"  "<<action<<endl;

	if(A1<MAX_GAIN_VARIATION)// A1 results
		strcpy_s(action,"Pass");
	else
		strcpy_s(action,"Fail");

	fout<<"Standard Deviation of A1:  "<<A1<<"  "<<action<<endl;

	if(A2<MAX_GAIN_VARIATION)// A2 results
		strcpy_s(action,"Pass");
	else
		strcpy_s(action,"Fail");

	fout<<"Standard Deviation of A2:  "<<A2<<"  "<<action<<endl;

	if(MT24<MAX_PERCENTAGE)// MT24 results
		strcpy_s(action,"Pass");
	else
		strcpy_s(action,"Fail");

	fout<<"Percentage of failed MT24: "<<MT24<<"  "<<action<<endl;

	if(MT5<MAX_PERCENTAGE)// MT5 results
		strcpy_s(action,"Pass");
	else
		strcpy_s(action,"Fail");

	fout<<"percentage of failed MT5:  "<<MT5<<"  "<<action<<endl<<endl;

	if(result==0)// Action taken (accept/reject)
		strcpy_s(action,"Accept");
	else
		strcpy_s(action,"Reject");

	fout<<"Action: "<<action;
}