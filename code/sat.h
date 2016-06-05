
/*********************************************************************
 ** File : sat.h
 ** Description: SAT Equivalence Checker using Davis Putnam Algorithm.
 ** Author: Varun Vijaykumar <varun.vijaykumar@s2014.tu-chemnitz.de>
 ** Date : 23rd June 2015
 *********************************************************************/

#ifndef __SAT_H_DECLARE__
#define __SAT_H_DECLARE__

// includes
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>

// defines
#define fileReadERROR -1
#define argReadERROR -1
#define fileReadSuccess 0
#define SUCCESS 0
#define FAILURE -1

// namespaces
using namespace std;

typedef enum { AND, OR, INV, XOR, ZERO, ONE, EQUAL, UNDEFINED } GateType;

typedef struct {
  GateType type;
  vector<int> nets;
} Gate;

typedef vector<Gate> GateList;
int netCount1, netCount2, MAX_NET_ID = 0;
vector<string> inputs1, outputs1, inputs2, outputs2;
vector<int> miterOutputs;
map<string, int> map1, map2, map_modified2;
GateList gates1, gates2, miterGateList;
vector<vector<int>> cnf;
map<int, int> assignedValues;
vector<int> miterLiterals;

int buildMiterCriterion();
int buildcnf(vector<vector<int>> cnf);

int readFile(string filename, int &netCount, vector<string> &inputs,
             vector<string> &outputs, map<string, int> &map, GateList &gates);

int readFiles(string filename1, string filename2);
bool unitClauseOccurance(vector<vector<int>> cnf, int &Literal);
bool pureLiteralOccurance(vector<vector<int>> cnf, int &Literal);
bool emptyClauseCheck(vector<vector<int>> cnf);
void cnfMapper(Gate gType);
void printCNF(vector<vector<int>> cnf);
void printDataForNetlist(int netlistNumber);
void davisPutnamAlgo(vector<vector<int>> cnf, vector<int> literalList);

void printData(int &netCount, vector<string> &inputs, vector<string> &outputs,
               map<string, int> &map, GateList &gates);

void setZeroLiteral(int Literal, vector<vector<int>> cnf,
                    vector<vector<int>> &cnfNew, vector<int> literalList,
                    vector<int> &literalListNew);

void setOneLiteral(int Literal, vector<vector<int>> cnf,
                   vector<vector<int>> &cnfNew, vector<int> literalList,
                   vector<int> &literalListNew);

#endif //#ifndef __SAT_H_DECLARE__
