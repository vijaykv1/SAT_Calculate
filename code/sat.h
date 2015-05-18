/*
 * Equivalence Checker for netlists
 *
 *
 * Name 1: Varun Vijaykumar 
 * Matriculation Number 1: 391960
 * Date of Completion : /05/2015
 *
 */

#ifndef _SAT_H_DECLARE_
#define _SAT_H_DECLARE_

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <cstdlib>

using namespace std;

typedef enum
{
	AND,
	OR,
	INV,
	XOR,
	ZERO,
	ONE,
	EQUAL, // adding this here for the sake of adding inputs Mapping in the case of MITER 
	UNDEFINED
} GateType;

typedef struct
{
	GateType type;
	vector<int> nets;
} Gate;

typedef vector<Gate> GateList;

#define fileReadERROR -1 
#define argReadERROR -1
#define fileReadSuccess 0
#define SUCCESS 0
#define FAILURE -1

// declarations for variables used.
int netCount1, netCount2,MAX_NET_ID = 0;

vector<string> inputs1, outputs1, inputs2, outputs2; // input of first netlist, outputs of second netlist , ... etc 

vector<int > miterOutputs;

map<string, int> map1, map2,map_modified2;

GateList gates1, gates2,miterGateList;

vector<vector<int> > cnf;

//declarations of the functions used

int readFile(string filename, int & netCount, vector<string> & inputs, vector<string> & outputs, map<string, int> & map, GateList & gates);
int readFiles(string filename1, string filename2);

void printData(int & netCount, vector<string> & inputs, vector<string> & outputs, map<string, int> & map, GateList & gates);
void printDataForNetlist(int netlistNumber);
void printCNF(vector< vector<int> > cnf);

int buildMiterCriterion();

void cnfMapper(Gate gType);  
int buildcnf(vector< vector<int> > cnf );

void DP(vector< vector<int> > cnf); // Davis Putnam Algorithm
void setZeroLiteral(int literal); 
void setOneLiteral(int literal); 

#endif  //#ifndef _SAT_H_DECLARE_