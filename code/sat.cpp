/*********************************************************************
 ** File : sat.cpp
 ** Description: SAT Equivalence Checker using Davis Putnam Algorithm.
 ** Author: Varun Vijaykumar <varun.vijaykumar@s2014.tu-chemnitz.de>
 ** Date : 23rd June 2015
 *********************************************************************/
#include "sat.h"
/********************* File Reading Functions ******************************/

////////////////////////////////////////////////////////////////////////////
// Reads netlist from the files that has been given as input to function.
////////////////////////////////////////////////////////////////////////////
int readFile(string filename, int &netCount, vector<string> &inputs,
             vector<string> &outputs, map<string, int> &map, GateList &gates) {
  ifstream file(filename.c_str());
  if (!file.is_open()) {
    return FAILURE;
  }
  string curLine;
  // net count
  getline(file, curLine);
  netCount = atoi(curLine.c_str());
  // inputs
  getline(file, curLine);
  stringstream inputsStream(curLine);
  string buf;
  while (inputsStream >> buf) {
    inputs.push_back(buf);
  }
  // outputs
  getline(file, curLine);
  stringstream outputsStream(curLine);
  while (outputsStream >> buf) {
    outputs.push_back(buf);
  }
  // mapping
  for (size_t i = 0; i < inputs1.size() + outputs1.size(); i++) {
    getline(file, curLine);
    stringstream mappingStream(curLine);
    mappingStream >> buf;
    int curNet = atoi(buf.c_str());
    mappingStream >> buf;
    map[buf] = curNet;
  }
  // empty line
  getline(file, curLine);
  if (curLine.length() > 1) {
    return -1;
  }
  // gates
  while (getline(file, curLine)) {
    stringstream gateStream(curLine);
    gateStream >> buf;
    Gate curGate;
    curGate.type =
        (buf == "and" ? AND : buf == "or"
                                  ? OR
                                  : buf == "inv"
                                        ? INV
                                        : buf == "xor"
                                              ? XOR
                                              : buf == "zero"
                                                    ? ZERO
                                                    : buf == "one" ? ONE
                                                                   : UNDEFINED);
    if (curGate.type == UNDEFINED) {
      return FAILURE;
    }
    while (gateStream >> buf) {
      int curNet = atoi(buf.c_str());
      curGate.nets.push_back(curNet);
    }
    gates.push_back(curGate);
  }
  return 0;
} // readFile

////////////////////////////////////////////////////////////////////////////
// Wrapper Function for the read files function.
////////////////////////////////////////////////////////////////////////////
int readFiles(string filename1, string filename2) {
  if (readFile(filename1, netCount1, inputs1, outputs1, map1, gates1) != 0) {
    return -1;
  }
  if (readFile(filename2, netCount2, inputs2, outputs2, map2, gates2) != 0) {
    return -1;
  }
  return 0;
} // readFiles

/********************* Printing Functions ***********************************/

////////////////////////////////////////////////////////////////////////////
// Prints the data on terminal for the circuit supplied in the netlist file.
////////////////////////////////////////////////////////////////////////////
void printData(int &netCount, vector<string> &inputs, vector<string> &outputs,
               map<string, int> &map, GateList &gates) {
  cout << "Net count: " << netCount << "\n\n";
  cout << "Inputs:\n";
  for (size_t i = 0; i < inputs.size(); i++) {
    cout << inputs[i] << "\n";
  }
  cout << "\n";
  cout << "Outputs:\n";
  for (size_t i = 0; i < outputs.size(); i++) {
    cout << outputs[i] << "\n";
  }
  cout << "\n";
  cout << "Mapping (input/output port to net number):\n";
  for (size_t i = 0; i < inputs.size(); i++) {
    cout << inputs[i] << " -> " << map[inputs[i]] << "\n";
  }
  for (size_t i = 0; i < outputs.size(); i++) {
    cout << outputs[i] << " -> " << map[outputs[i]] << "\n";
  }
  cout << "\n";
  cout << "Gates:\n";
  for (size_t i = 0; i < gates.size(); i++) {
    Gate &curGate = gates[i];
    cout << (curGate.type == AND
                 ? "AND"
                 : curGate.type == OR
                       ? "OR"
                       : curGate.type == INV
                             ? "INV"
                             : curGate.type == XOR
                                   ? "XOR"
                                   : curGate.type == ZERO
                                         ? "ZERO"
                                         : curGate.type == ONE ? "ONE"
                                                               : "ERROR");
    cout << ": ";
    for (size_t j = 0; j < curGate.nets.size(); j++) {
      cout << curGate.nets[j] << " ";
    }
    cout << "\n";
  }
  cout << "\n";
} // printData

////////////////////////////////////////////////////////////////////////////
// Wrapper Function that deals with directing the netlist that needs displaying.
////////////////////////////////////////////////////////////////////////////
void printDataForNetlist(int netlistNumber) {
  if (netlistNumber == 1) {
    printData(netCount1, inputs1, outputs1, map1, gates1);
  } else if (netlistNumber == 2) {
    printData(netCount2, inputs2, outputs2, map2, gates2);
  } else {
    cout << "Invalid netlist number " << netlistNumber << " (must be 1 or 2)\n";
  }
} // printDataForNetlist

////////////////////////////////////////////////////////////////////////////
// Prints the CNFs that has been created by the cnf creation Algorithm or
// the Davis Putnam Algorithm processing unit.
////////////////////////////////////////////////////////////////////////////
void printCNF(vector<vector<int>> cnf) {
  cout << "*************************************************" << endl;
  cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ " << endl;
  cout << "CNF Size is : " << cnf.size() << endl;
  cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ " << endl;

  for (int i = 0; i < cnf.size(); i++) {
    vector<int> clauses = cnf[i];

    for (int j = 0; j < clauses.size(); j++) {
      if (j != 0) {
        cout << " v ";

      } else {
        cout << " (";
      }
      cout << clauses[j];
    }
    cout << ") " << endl;
  }
  cout << endl;
  cout << "*************************************************" << endl;
} // printCNF

/********************  Processing Algorithms  *****************************/

////////////////////////////////////////////////////////////////////////////
// Reads structures which have been created via read functions and then creates
// the respective miter Circuit for them.
////////////////////////////////////////////////////////////////////////////
int buildMiterCriterion() {
  int returnCode = FAILURE;

  for (int i = 0; i < inputs2.size(); i++) {
    map_modified2[inputs2[i]] = map2[inputs2[i]] + netCount1;
  }

  for (int i = 0; i < outputs2.size(); i++) {
    map_modified2[outputs2[i]] = map2[outputs2[i]] + netCount1;
  }

  for (int i = 0; i < gates2.size(); i++) {
    for (int j = 0; j < gates2[i].nets.size(); j++)
      gates2[i].nets[j] = gates2[i].nets[j] + netCount1;
  }

  map<string, int> miterMap;

  int outputs_circuit1 = outputs1.size();
  int outputs_circuit2 = outputs2.size();
  int newOutputValue = netCount1 + netCount2 + 1;

  if (outputs_circuit1 == outputs_circuit2) {
    if (((outputs_circuit1 + outputs_circuit2) % 2) == 0) {
      vector<int> miterIntermediateOutputs;

      for (int i = 0; i < outputs_circuit1; i++) {
        Gate miterGate;
        miterGate.type = XOR;
        miterGate.nets.push_back(map1[outputs1[i]]);
        miterGate.nets.push_back(map_modified2[outputs2[i]]);
        miterGate.nets.push_back(newOutputValue);
        miterGateList.push_back(miterGate);
        miterIntermediateOutputs.push_back(newOutputValue);
        newOutputValue++;
      }

      int finalMiterOutput =
          netCount1 + netCount2 + miterIntermediateOutputs.size();

      if (miterIntermediateOutputs.size() > 1) {
        Gate miterGate;
        miterGate.type = OR;

        for (int i = 0; i < miterIntermediateOutputs.size(); i++) {
          miterGate.nets.push_back(miterIntermediateOutputs[i]);
        }

        miterGate.nets.push_back(finalMiterOutput + 1);
        miterGateList.push_back(miterGate);

        miterOutputs.push_back(finalMiterOutput + 1);
        cout << "miter built successfully ! " << endl;
        returnCode = SUCCESS;
      } else {
        miterOutputs.push_back(miterIntermediateOutputs[0]);
        cout << "Miter built successfully !!" << endl;
        returnCode = SUCCESS;
      }
    }
  } else {
    cout << "Miter Building Failed,Something went really wrong !!" << endl;
    returnCode = FAILURE;
  }

  for (int i = 1; i <= miterOutputs[0]; i++) {
    miterLiterals.push_back(i);
    assignedValues.insert(std::pair<int, int>(i, -1));
  }

  for (int i = 0; i < assignedValues.size(); i++) {
    cout << i + 1 << ":" << assignedValues[i + 1] << endl;
  }

  for (int i = 0; i < miterLiterals.size(); i++) {
    cout << miterLiterals[i] << " ";
  }
  cout << endl;

  return returnCode;
} // buildMiterCriterion

////////////////////////////////////////////////////////////////////////////
// Builds the CNF based on the Miter circuits that has been generated.
////////////////////////////////////////////////////////////////////////////
int buildcnf(vector<vector<int>> cnf) {
  int returnCode = FAILURE;

  if (buildMiterCriterion() == SUCCESS) {
    Gate InputGate;
    InputGate.type = EQUAL;
    cnfMapper(InputGate);

    for (int i = 0; i < gates1.size(); i++) {
      cnfMapper(gates1[i]);
    }

    for (int i = 0; i < gates2.size(); i++) {
      cnfMapper(gates2[i]);
    }

    for (int i = 0; i < miterGateList.size(); i++) {
      cnfMapper(miterGateList[i]);
    }

    returnCode = SUCCESS;
  } else {
    cout << "Miter Criterion building Failed ... returning Failure ! " << endl;
  }

  return returnCode;
} // buildcnf

////////////////////////////////////////////////////////////////////////////
// CNF mapping according to the gates that are present in the miter circuit.
////////////////////////////////////////////////////////////////////////////
void cnfMapper(Gate gType) {
  switch (gType.type) {
  case OR:
    if (gType.nets.size() > 3) {
      vector<int> cnfClause;
      for (int i = 0; i < (gType.nets.size() - 1); i++) {
        cnfClause.push_back(gType.nets[i]);
      }
      cnf.push_back(cnfClause);
    } else {
      vector<int> cnfClause1, cnfClause2, cnfClause3;

      cnfClause1.push_back(-(gType.nets[0]));
      cnfClause1.push_back((gType.nets[2]));
      cnfClause2.push_back(-(gType.nets[1]));
      cnfClause2.push_back(gType.nets[2]);
      cnfClause3.push_back(gType.nets[0]);
      cnfClause3.push_back(gType.nets[1]);
      cnfClause3.push_back(-(gType.nets[2]));

      cnf.push_back(cnfClause1);
      cnf.push_back(cnfClause2);
      cnf.push_back(cnfClause3);
    }
    break;

  case AND: {
    vector<int> cnfClause1, cnfClause2, cnfClause3;

    cnfClause1.push_back(gType.nets[0]);
    cnfClause1.push_back(-(gType.nets[2]));
    cnfClause2.push_back(gType.nets[1]);
    cnfClause2.push_back(-(gType.nets[2]));
    cnfClause3.push_back(-(gType.nets[0]));
    cnfClause3.push_back(-(gType.nets[1]));
    cnfClause3.push_back(gType.nets[2]);

    cnf.push_back(cnfClause1);
    cnf.push_back(cnfClause2);
    cnf.push_back(cnfClause3);

    break;
  }

  case INV: {
    vector<int> cnfClause1, cnfClause2, cnfClause3;

    cnfClause1.push_back(gType.nets[0]);
    cnfClause1.push_back(gType.nets[1]);
    cnfClause2.push_back(-(gType.nets[0]));
    cnfClause2.push_back(-(gType.nets[1]));

    cnf.push_back(cnfClause1);
    cnf.push_back(cnfClause2);

    break;
  }

  case XOR: {
    vector<int> cnfClause1, cnfClause2, cnfClause3, cnfClause4;

    cnfClause1.push_back(gType.nets[0]);
    cnfClause1.push_back(gType.nets[1]);
    cnfClause1.push_back(-(gType.nets[2]));
    cnfClause2.push_back(-(gType.nets[0]));
    cnfClause2.push_back(-(gType.nets[1]));
    cnfClause2.push_back(-(gType.nets[2]));
    cnfClause3.push_back(-(gType.nets[0]));
    cnfClause3.push_back(gType.nets[1]);
    cnfClause3.push_back(gType.nets[2]);
    cnfClause4.push_back(gType.nets[0]);
    cnfClause4.push_back(-(gType.nets[1]));
    cnfClause4.push_back(gType.nets[2]);

    cnf.push_back(cnfClause1);
    cnf.push_back(cnfClause2);
    cnf.push_back(cnfClause3);
    cnf.push_back(cnfClause4);

    break;
  }

  case ZERO: // dead code
    break;

  case ONE: // dead code
    break;

  case EQUAL:
    if (inputs1.size() == inputs2.size()) {
      for (int i = 0; i < inputs1.size(); i++) {
        vector<int> clause1, clause2;

        for (int j = 0; j < inputs1.size(); j++) {
          if (inputs1[i] == inputs2[j]) {
            clause1.push_back(-(map1[inputs1[i]]));
            clause1.push_back(map_modified2[inputs2[j]]);
            clause2.push_back(map1[inputs1[i]]);
            clause2.push_back(-(map_modified2[inputs2[j]]));

            cnf.push_back(clause1);
            cnf.push_back(clause2);
          }
        }
      }
    }
    break;

  case UNDEFINED:
  default:
    cout << "NO GATE detected ... NO CNF DUMPING" << endl;
    break;
  }
} // cnfMapper

////////////////////////////////////////////////////////////////////////////
// Main Davis Putnam Engine Function. (Recursive Functionality Enabled here)
////////////////////////////////////////////////////////////////////////////
void davisPutnamAlgo(vector<vector<int>> cnf, std::vector<int> literalsList) {
  int literal_extracted;
  vector<vector<int>> cnfNew;
  vector<int> literalmaint;

  printCNF(cnf);

  while (unitClauseOccurance(cnf, literal_extracted) ||
         pureLiteralOccurance(cnf, literal_extracted)) {
    if (unitClauseOccurance(cnf, literal_extracted)) {
      cout << "-- Unit Clause Reduction --" << endl;
      if (literal_extracted > 0) {
        setOneLiteral(literal_extracted, cnf, cnfNew, literalsList,
                      literalmaint);
      } else {
        setZeroLiteral(literal_extracted, cnf, cnfNew, literalsList,
                       literalmaint);
      }
      cnf = cnfNew;
      literalsList = literalmaint;
      cout << "-- End of Unit Clause Reduction -- " << endl << endl;
    } else if (pureLiteralOccurance(cnf, literal_extracted)) {
      cout << "-- Pure Literal Reduction -- " << endl;
      if (literal_extracted > 0) {
        setOneLiteral(literal_extracted, cnf, cnfNew, literalsList,
                      literalmaint);
      } else // negetive value
      {
        setZeroLiteral(literal_extracted, cnf, cnfNew, literalsList,
                       literalmaint);
      }
      cnf = cnfNew;
      literalsList = literalmaint;
      cout << "-- End of Pure Literal Reduction -- " << endl << endl;
    } else {
      cout << " should not be entering here ... check code !! " << endl;
    }
  }

  if (cnf.size() == 0) {
    cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++ " << endl;
    cout << "************** CIRCUIT IS NOT EQUIVALENT !! ******** " << endl;
    cout << "************** Counter Solution Proposed *********** " << endl;

    cout << "INPUTS (To both Circuits via Miter Input Feeding) : " << endl;
    for (int i = 0; i < inputs1.size(); i++) {
      cout << inputs1[i] << ":" << assignedValues[map1[inputs1[i]]] << endl;
    }

    cout << "OUTPUTS (Circuit 1): " << endl;
    for (int k = 0; k < outputs1.size(); k++) {
      cout << outputs1[k] << ":" << assignedValues[map1[outputs1[k]]];
    }

    cout << endl << "OUTPUTS (Circuit 2): " << endl;
    for (int j = 0; j < outputs2.size(); j++) {
      cout << outputs2[j] << ":" << assignedValues[map_modified2[outputs2[j]]];
    }

    cout << endl << "++++++++++++++++++++++++++++++++++++++++++++ " << endl;
    exit(0);
  } else if (emptyClauseCheck(cnf)) {
    return;
  } else {
    if (literalsList.size() != 0) {
      int literal_selected = literalsList.back();

      literalsList.pop_back();

      vector<vector<int>> cnf0, cnf1;
      vector<int> literalBackTrack1, literalBackTrack2;

      if (literal_selected != miterOutputs[0]) {
        setZeroLiteral(literal_selected, cnf, cnf0, literalsList,
                       literalBackTrack1);
        davisPutnamAlgo(cnf0, literalBackTrack1);
      }

      setOneLiteral(literal_selected, cnf, cnf1, literalsList,
                    literalBackTrack2);
      davisPutnamAlgo(cnf1, literalBackTrack2);

      assignedValues[literal_selected] = -1;
      literalsList.push_back(literal_selected);
      return;
    }
  }
} // davisPutnamAlgo

////////////////////////////////////////////////////////////////////////////
// Helper Function for Davis Putnam Engine.
// This will scan the CNF created for a unitClause (eg : (1), (4) , etc ...)
////////////////////////////////////////////////////////////////////////////
bool unitClauseOccurance(vector<vector<int>> cnf, int &literal) {
  bool returnCode = false;

  for (int i = 0; i < cnf.size(); i++) {
    vector<int> clause = cnf[i];

    if ((clause.size() == 1) && (clause[0] != 0)) {
      literal = clause[0];
      returnCode = true;
    } else {
      literal = 0;
      returnCode = false;
    }
  }
  return returnCode;
} // unitClauseOccurance

////////////////////////////////////////////////////////////////////////////
// Helper Function for the Davis Putnam Algorithm.
// This will scan the CNF for a commonly occuring Literal throughout.
////////////////////////////////////////////////////////////////////////////
bool pureLiteralOccurance(vector<vector<int>> cnf, int &literal) {
  bool returnCode = false;
  int size_CNF = cnf.size();

  if (cnf.size() != 0) {
    for (int checkElement = 1; checkElement <= miterOutputs[0];
         checkElement++) {
      int positiveElement = 0;
      int negetiveElement = 0;

      for (int i = 0; i < cnf.size(); i++) {
        if (find(cnf[i].begin(), cnf[i].end(), checkElement) != cnf[i].end()) {
          positiveElement++;
        } else if (find(cnf[i].begin(), cnf[i].end(), (-checkElement)) !=
                   cnf[i].end()) {
          negetiveElement++;
        } else {
          vector<int> clause = cnf[i];
          int lastElement = clause.size();

          if (clause.size() != 0) {
            // checking the end element as well
            if (clause[lastElement - 1] == checkElement) {
              positiveElement++;
            } else if (clause[lastElement] == (-checkElement)) {
              negetiveElement++;
            }
          }
        }
      }

      if (((negetiveElement == size_CNF) && (positiveElement == 0)) ||
          ((positiveElement == size_CNF) && (negetiveElement == 0))) {
        literal = checkElement;
        returnCode = true;
        break;
      } else {
        literal = 0;
      }
    }
  }

  return returnCode;
} // pureLiteralOccurance

////////////////////////////////////////////////////////////////////////////
// Helper Function For Davis Putnam Engine.
// This will help in scanning the CNF for occurance of (0)
////////////////////////////////////////////////////////////////////////////
bool emptyClauseCheck(vector<vector<int>> cnf) {
  bool returnCode = false;

  for (int i = 0; i < cnf.size(); i++) {
    vector<int> clause = cnf[i];
    if ((clause.size() == 1) && (clause[0] == 0)) {
      returnCode = true;
      break;
    }
  }
  return returnCode;
} // emptyClauseCheck

/***************************** HEART FUNCTIONS *****************************/

////////////////////////////////////////////////////////////////////////////
// Helper Function to Davis Putnam Function.
// Helps in setting a Literal to 1.
// Special Condition: if the Literal has a negetive sign then a 0 is set.
////////////////////////////////////////////////////////////////////////////
void setOneLiteral(int Literal, vector<vector<int>> cnf,
                   std::vector<std::vector<int>> &cnfNew,
                   vector<int> literalList, vector<int> &literalListNew) {
  literalListNew = literalList;

  if ((literalListNew.size() != 0) &&
      ((find(literalListNew.begin(), literalListNew.end(), Literal) !=
        literalListNew.end()) ||
       (literalListNew[literalList.size()] == Literal))) {
    literalListNew.erase(
        remove(literalListNew.begin(), literalListNew.end(), Literal),
        literalListNew.end());
  }

  if (Literal < 0) {
    Literal = -Literal;
  }

  if (assignedValues.find(Literal) == assignedValues.end()) {
    assignedValues.insert(std::pair<int, int>(Literal, 1));
  } else {
    assignedValues[Literal] = 1;

    for (int i = 0; i < assignedValues.size(); i++) {
      if ((i + 1) == Literal) {
        cout << i + 1 << ":" << assignedValues[i + 1] << " <--- Changed here "
             << endl;
      } else {
        cout << i + 1 << ":" << assignedValues[i + 1] << endl;
      }
    }
  }

  cnfNew = cnf;
  for (int i = (cnfNew.size() - 1); i >= 0; i--) {
    for (int j = (cnfNew[i].size() - 1); j >= 0; j--) {
      if (cnfNew[i][j] == Literal) {
        cnfNew.erase(cnfNew.begin() + i);
      } else if ((cnfNew[i][j] == (-Literal)) && (cnfNew[i].size() > 1)) {
        cnfNew[i].erase(cnfNew[i].begin() + j);
      } else if (cnfNew[i][j] == (-Literal)) {
        cnfNew[i][j] = 0;
      }
    }
  }
} // setOneLiteral

////////////////////////////////////////////////////////////////////////////
// Helper Function to Davis Putnam Function.
// Helps in setting a Literal to 0.
// Special Condition: if the Literal has a negetive sign then a 1 is set.
////////////////////////////////////////////////////////////////////////////
void setZeroLiteral(int Literal, vector<vector<int>> cnf,
                    vector<vector<int>> &cnfNew, vector<int> literalList,
                    vector<int> &literalListNew) {
  literalListNew = literalList;
  if ((literalListNew.size() != 0) &&
      ((find(literalListNew.begin(), literalListNew.end(), Literal) !=
        literalListNew.end()) ||
       (literalListNew[literalListNew.size()] == Literal))) {
    literalListNew.erase(
        remove(literalListNew.begin(), literalListNew.end(), Literal),
        literalListNew.end());
  }

  if (Literal < 0) {
    Literal = -Literal;
  }

  if (assignedValues.find(Literal) == assignedValues.end()) {
    assignedValues.insert(std::pair<int, int>(Literal, 0));
  } else {
    assignedValues[Literal] = 0;

    for (int i = 1; i < assignedValues.size(); i++) {
      if ((i + 1) == Literal) {
        cout << i + 1 << ":" << assignedValues[i + 1]
             << "  <----- Changed here " << endl;
      } else {
        cout << i + 1 << ":" << assignedValues[i + 1] << endl;
      }
    }
  }

  cnfNew = cnf;
  for (int i = (cnfNew.size() - 1); i >= 0; i--) {
    for (int j = (cnfNew[i].size() - 1); j >= 0; j--) {
      if (cnfNew[i][j] == (-Literal)) {
        cnfNew.erase(cnfNew.begin() + i);
      } else if ((cnfNew[i][j] == Literal) && (cnfNew[i].size() > 1)) {
        cnfNew[i].erase(cnfNew[i].begin() + j);
      } else if (cnfNew[i][j] == Literal) {
        cnfNew[i][j] = 0;
      }
    }
  }
} // setZeroLiteral

/************************** MAIN FUNCTION **********************************/

////////////////////////////////////////////////////////////////////////////
// Main Control Function.
// Performs Calls for
// 1) Circuitry Reading from the .net file
// 2) Miter Circuitry Creation.
// 3) Davis Putnam Engine Activation
// 4) Display of result (if necessary , i.e. depends on the end point)
////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
  if (argc != 3) {
    cerr << "Wrong argument count!\n";
    return argReadERROR;
  }

  if (readFiles(argv[1], argv[2]) != 0) {
    cerr << "Error while reading files!\n";
    return fileReadERROR;
  }

  cout << "Netlist 1:\n==========\n";
  printDataForNetlist(1);
  cout << "\nNetlist 2:\n==========\n";
  printDataForNetlist(2);

  if (buildcnf(cnf) != 0) {
    cout << "CNF building Failed ... Marking whole SAT Algorithm as failure ";
    return FAILURE;
  } else {
    cout << "###################################################" << endl;
    cout << "CNF : ";
    printCNF(cnf);
    cout << "#######################################################" << endl;
    cout << "CNF build successful ... Now applying Davis Putnam Algorithm"
         << endl;
    davisPutnamAlgo(cnf, miterLiterals);
    cout << "***************** FINAL RESULT ************************** "
         << endl;
    cout << "THE GIVEN CIRCUITS ARE EQUIVALENT !! " << endl;
    cout << "********************************************************* "
         << endl;
  }
  return SUCCESS;
} // main
