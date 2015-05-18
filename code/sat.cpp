#include "sat.h" // moving all the Variables to the header files 

//File Reading Functions.
int readFile(string filename, int & netCount, vector<string> & inputs, vector<string> & outputs, map<string, int> & map, GateList & gates)
{
	ifstream file(filename.c_str());
	if (! file.is_open())
	{
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
	while (inputsStream >> buf)
	{
		inputs.push_back(buf);
	}
	// outputs
	getline(file, curLine);
	stringstream outputsStream(curLine);
	while (outputsStream >> buf)
	{
		outputs.push_back(buf);
	}
	// mapping
	for (size_t i=0; i<inputs1.size() + outputs1.size(); i++)
	{
		getline(file, curLine);
		stringstream mappingStream(curLine);
		mappingStream >> buf;
		int curNet = atoi(buf.c_str());
		mappingStream >> buf;
		map[buf] = curNet;
	}
	// empty line
	getline(file, curLine);
	if (curLine.length() > 1)
	{
		return -1;
	}
	// gates
	while (getline(file, curLine))
	{
		stringstream gateStream(curLine);
		gateStream >> buf;
		Gate curGate;
		curGate.type = (buf == "and" ? AND : buf == "or" ? OR : buf == "inv" ? INV : buf == "xor" ? XOR : buf == "zero" ? ZERO : buf == "one" ? ONE : UNDEFINED);
		if (curGate.type == UNDEFINED)
		{
			return FAILURE;
		}
		while (gateStream >> buf)
		{
			int curNet = atoi(buf.c_str());
			curGate.nets.push_back(curNet);
		}
		gates.push_back(curGate);
	}
	return 0;
}

int readFiles(string filename1, string filename2)
{
	if (readFile(filename1, netCount1, inputs1, outputs1, map1, gates1) != 0)
	{
		return -1;
	}
	if (readFile(filename2, netCount2, inputs2, outputs2, map2, gates2) != 0)
	{
		return -1;
	}
	return 0;
}

//Printing Functions.
void printData(int & netCount, vector<string> & inputs, vector<string> & outputs, map<string, int> & map, GateList & gates)
{
	cout << "Net count: " << netCount << "\n\n";
	cout << "Inputs:\n";
	for (size_t i=0; i<inputs.size(); i++)
	{
		cout << inputs[i] << "\n";
	}
	cout << "\n";
	cout << "Outputs:\n";
	for (size_t i=0; i<outputs.size(); i++)
	{
		cout << outputs[i] << "\n";
	}
	cout << "\n";
	cout << "Mapping (input/output port to net number):\n";
	for (size_t i=0; i<inputs.size(); i++)
	{
		cout << inputs[i] << " -> "<< map[inputs[i]] << "\n";
	}
	for (size_t i=0; i<outputs.size(); i++)
	{
		cout << outputs[i] << " -> "<< map[outputs[i]] << "\n";
	}
	cout << "\n";
	cout << "Gates:\n";
	for (size_t i=0; i<gates.size(); i++)
	{
		Gate & curGate = gates[i];
		cout << (curGate.type == AND ? "AND" : curGate.type == OR ? "OR" : curGate.type == INV ? "INV" : curGate.type == XOR ? "XOR" : curGate.type == ZERO ? "ZERO" : curGate.type == ONE ? "ONE" : "ERROR");
		cout << ": ";
		for (size_t j=0; j<curGate.nets.size(); j++)
		{
			cout << curGate.nets[j] << " ";
		}
		cout << "\n";
	}
	cout << "\n";
}

void printDataForNetlist(int netlistNumber)
{
	if (netlistNumber == 1)
	{
		printData(netCount1, inputs1, outputs1, map1, gates1);
	}
	else if (netlistNumber == 2)
	{
		printData(netCount2, inputs2, outputs2, map2, gates2);
	}
	else
	{
		cout << "Invalid netlist number " << netlistNumber << " (must be 1 or 2)\n";
	}
}

void printCNF(vector< vector<int> > cnf)
{
	for (int i = 0 ; i < cnf.size() ; i++)
	{
		vector <int> clauses = cnf[i]; // dump the first clause from the list of clauses ... 

		for(int j = 0 ; j < clauses.size() ; j++)
		{
			if (j != 0)
			{
				cout << "v" ; // append Or Operator

			}
			else
			{
				cout << " {" ; 
			}
			cout << "("<<clauses[j]<<")"; 
		}
		cout << "} " ; // end of the clause printed here and may or may not be the start of the next clause 
	}
	cout << endl; // end of the whole CNF Function 
}


//Processing Algorithms.
int buildMiterCriterion()
{
	// create a new vector list + Map for the purpose of remapping the ports and their corresponding gates <as we are going to create a new set of boolean functions>

	int returnCode = FAILURE; 

	cout << "Commencing Miter Circuit Building process !! " << endl; 

	for (int i = 0 ; i < inputs2.size() ; i++)
	{
		map_modified2[inputs2[i]] = map2[inputs2[i]] + netCount1 ; // creating a local map 
		//map2[inputs2[i]] = map2[inputs2[i]] + netCount1;  // replace all the content in the second map with an offset ! 
	}

	// now that we have re-alloc all the inputs , lets re-alloc all the outputs ..
	for (int i=0; i < outputs2.size() ; i++)
	{
		map_modified2[outputs2[i]] = map2[outputs2[i]] + netCount1 ;  
	}

	// Performed reallocs for all the ports .. Now we can start the Miter criterions! 
	//for this we need to create a new Map for the newly created circuits 
	// According to the Miter Procedure, we need to 
	// 1) Join all the inputs of both the circuits
	// 2) join all the outputs to the overall circuit to a XOR gate(s) <2 at a time >
	// 3) Join all the XOR outputs to an OR gate ! <might give a hack here to speed up the algorithm overall>
   
    map <string,int > miterMap; // optional now , lets confirm

    //optimizing by accessing the value only once ... might need this for the case of calculations below: 
    int outputs_circuit1 = outputs1.size(); 
    int outputs_circuit2 = outputs2.size(); 
    int newOutputValue = netCount1+netCount2; 

    //outputs are to be routed to the XOR gates.
    if (outputs_circuit1 == outputs_circuit2) // FIXME .. Revise this condition check .. 
    {
    	int outdiff = 0 ; 
    	// if the number of gates are even for both the circuits , then we have a jackpot , just create an new in out combination
    	if ((outputs_circuit1 + outputs_circuit2) % 2)
    	{
    		if (outputs_circuit1 == outputs_circuit2)
    		{

    			int outdiff = 0;
    			vector<int> miterIntermediateOutputs;
    			for (int i = 0 ; i < outputs_circuit1 ; i++ )
    			{
    				Gate miterGate;
    				miterGate.type = XOR;
    				miterGate.nets[0] = map1[outputs1[i]];
    				miterGate.nets[1] = map_modified2[outputs2[i]]; 
    				miterGate.nets[2] = newOutputValue;
    				miterGateList.push_back(miterGate); // each iteration pushes a new gate assignment  
    				miterIntermediateOutputs.push_back(newOutputValue); // making a list of outputs for the intermediate values here ... <local use only>
    				newOutputValue++;  
    			}
    			// finished creating the XOR set ... Now lets take the next level of the output here
    			int finalMiterOutput = netCount1+netCount2+miterIntermediateOutputs.size();

    			//appending the list of 
    			if (miterIntermediateOutputs.size() != 1) 
    			{
    				Gate miterGate; 
    				miterGate.type = OR; 
    				for (int i = 0 ; i < miterIntermediateOutputs.size() ; i++)
    				{
    					miterGate.nets[i] = miterIntermediateOutputs[i]; 
    				}
    				miterGate.nets[(miterIntermediateOutputs.size() + 1)] = finalMiterOutput; 
    				miterGateList.push_back(miterGate); // add the final OR gate into the picture here !
    				miterOutputs.push_back(finalMiterOutput);
    				cout << "miter built successfully ! " << endl; 
    				returnCode = SUCCESS;
    			}
    			else 
    			{
    				miterOutputs.push_back(miterIntermediateOutputs[0]); // push the only value into the miterOutput ! 
    				cout << "Miter built successfully !!" << endl;
    				returnCode = SUCCESS;
    			}
    		}
    	}
    }
    else
    {
    	cout << "Miter Building Failed ... Something went really wrong !! " << endl; 
    	returnCode = FAILURE;
    }

    return returnCode;
}

int buildcnf(vector<vector<int> > cnf)
{
	//We are going to build the cnf for the given netlist ... and feedback the said cnf to the requester
	if(buildMiterCriterion())
	{
		cout << "Miter Criterion built .. creating the CNF based on cicuits.." << endl;

		/*now we have to create a cnf dump based on the gates for 3 circuits : 
		*1)gates1
		*2)gates2
		*3)miterGateList
		*4)input assignments */

		cout << "creating cnf out of miter circuit created !!" << endl;

		//inputs Assignment Case ... We will create a temporary Gate called EQUAL and Feed it to the CNFMapper 
		Gate InputGate;
		InputGate.type = EQUAL;
		cnfMapper(InputGate); // Test code 

		for (int i = 0 ; i < gates1.size(); i++)
		{
			cnfMapper(gates1[i]);
		}

		// starting with the dump of gates2 
		for (int i = 0 ; i < gates2.size(); i++)
		{
			cnfMapper(gates2[i]);
		}

		//starting with the dump of miterGateList 
		for (int i = 0 ; i < miterGateList.size(); i++ )
		{
			cnfMapper(miterGateList[i]);
		}

		return SUCCESS; 
	}
	else
	{
		cout << "Miter Criterion building Failed ... returning Failure ! " << endl; 
		return FAILURE; 
	}
	 // stub for the time being
}

void cnfMapper(Gate gType)
{
	// CNF Mapper function
	switch (gType.type){
		case OR:
			cout << "OR Gate detected...Characterestic Dumped into CNF " << endl;
			if (gType.nets.size() > 3)
			{
				vector<int > cnfClause;
				for (int i =0; i < (gType.nets.size()-1) ; i++){
					cnfClause.push_back(gType.nets[i]); // we are giving -1 because we are not including the output port into the equations
				}
				cnf.push_back(cnfClause);
			}
			else
			{
				vector<int > cnfClause1,cnfClause2,cnfClause3;
				//clause1
				cnfClause1.push_back(-(gType.nets[0]));
				cnfClause1.push_back(-(gType.nets[2]));
				//clause 2
				cnfClause2.push_back(-(gType.nets[1]));
				cnfClause2.push_back(gType.nets[2]);
				//clause 3 
				cnfClause3.push_back(gType.nets[0]);
				cnfClause3.push_back(gType.nets[1]);
				cnfClause3.push_back(-(gType.nets[2]));
				//Pushing in
				cnf.push_back(cnfClause1);
				cnf.push_back(cnfClause2);
				cnf.push_back(cnfClause3);
			} 
			break;
				
		case AND:
		{
			vector<int > cnfClause1,cnfClause2,cnfClause3;
			//clause1
			cnfClause1.push_back(gType.nets[0]);
			cnfClause1.push_back(-(gType.nets[2]));
			//clause 2
			cnfClause2.push_back(gType.nets[1]);
			cnfClause2.push_back(-(gType.nets[2]));
			//clause 3 
			cnfClause3.push_back(-(gType.nets[0]));
			cnfClause3.push_back(-(gType.nets[1]));
			cnfClause3.push_back(gType.nets[2]);
			//Pushing in
			cnf.push_back(cnfClause1);
			cnf.push_back(cnfClause2);
			cnf.push_back(cnfClause3);
			cout << "AND Gate detected ... Characterestic Dumped into CNF" << endl ;
			break;
		}

		case INV:
		{
			vector<int > cnfClause1,cnfClause2,cnfClause3;
			//Clause 1
			cnfClause1.push_back(gType.nets[0]);
			cnfClause1.push_back(gType.nets[1]);

			//Clause 2 
			cnfClause2.push_back(-(gType.nets[0]));
			cnfClause2.push_back(-(gType.nets[1]));

			//Pushing in 
			cnf.push_back(cnfClause1);
			cnf.push_back(cnfClause2);
			cout << "INV Gate detected ... Characterestic Dumped into CNF" << endl ;
			break;
		}

		case XOR:
		{
			vector<int > cnfClause1,cnfClause2,cnfClause3,cnfClause4;
			//clause 1
			cnfClause1.push_back(gType.nets[0]);
			cnfClause1.push_back(gType.nets[1]);
			cnfClause1.push_back(-(gType.nets[2]));
			//clause 2
			cnfClause2.push_back(-(gType.nets[0]));
			cnfClause2.push_back(-(gType.nets[1]));
			cnfClause2.push_back(-(gType.nets[2]));
			//clause3
			cnfClause3.push_back(-(gType.nets[0]));
			cnfClause3.push_back(gType.nets[1]);
			cnfClause3.push_back(gType.nets[2]);
			//clause 4
			cnfClause4.push_back(gType.nets[0]);
			cnfClause4.push_back(-(gType.nets[1]));
			cnfClause4.push_back(gType.nets[2]);
			//pushing in 
			cnf.push_back(cnfClause1);
			cnf.push_back(cnfClause2);
			cnf.push_back(cnfClause3);
			cnf.push_back(cnfClause4);
			cout << "XOR Gate detected... Characterestic Dumped into CNF" << endl; 
			break;
		}

		case ZERO: //prefer this to be dead code 
			cout << "ZERO gate detected ... nothing to do , as of now stubbed ... " << endl; 
			break;

		case ONE: //prefer this to be dead code 
			cout << "ONE gate detected ... nothing to do , as of now stubbed ... " << endl;
			break;

		case EQUAL: //to deal with the inputs Mapping in the Miter circuit 
			cout << "EQUAL GATE detected ... Feeding the case of inputs Mapping into the CNF ..." << endl;

			if (inputs1.size() == inputs2.size())
			{
				for (int i = 0 ; i < inputs1.size() ; i++)
				{
					// equality feeder 
					vector<int> clause1,clause2;

					for (int j = 0 ; j < inputs1.size() ; j++)
					{
						if (inputs1[i] == inputs2[j])
						{
							// logic here : IFF the inputs are of the same type, then create the CNF based on that !
							//clause 1
							clause1.push_back(-(map1[inputs1[i]]));
							clause1.push_back(map_modified2[inputs2[j]]);
						
							//clause 2
							clause2.push_back(map1[inputs1[i]]);
							clause2.push_back(-(map_modified2[inputs2[j]])); 
							
							//pushing in cnf
							cnf.push_back(clause1);
							cnf.push_back(clause2);
						}
					}
				}
			}
			break;

		case UNDEFINED:	//rare case .. Mostly Happens when we have an UNDEFINED Gate 
		default: 
			cout << "NO GATE detected ... NO CNF DUMPING" << endl; 
			break;
	}
}

//Needs a serious Revision of this code as some of the parts are still not conformed ! 
void DP(vector< vector<int> > cnf)
{
	// OK ... now we have got the generated from the above said Algorithms ... 
	bool emptyClause = false ; // Reconsider removong this once the whole algorithm is cleared 
	/*
	* Davis Putnam Algorithm Mechanism 
	*1) Clear off all the pure Literals
	*2) Apply the unit Clause rule.
	*3) Check if the CNF is empty ... Then stop the Algorithm and make the decision !
	*4) If Clause is Empty then Backtrack --> Not quite clear.
	*5) run the setting the variables to 0 or 1 and run the algorithm recursively. 
	*/ 
	
	// Checking Unit Clause rule
	for (int i = 0 ; i < cnf.size() ; i++)
	{
		vector<int> clause = cnf[i]; 
		
		if (clause.size() == 1 )
		{
			int literal = clause[0]; 

			if (literal >= 0) // positive 
			{
				setOneLiteral(literal);
			}
			else //negetive 
			{
				setZeroLiteral(literal);
			}  
		}
		else if (clause.size() == 0 )
		{
			emptyClause = true ; //wrong --> Needs FIXING 
		}
	}

	// Checking for the pureLiteral rule
	for (int check_variable = 0 ; check_variable < miterOutputs[0] ; check_variable++)
	{
		int check_variable_positive_counter = 0 ; 
		int check_variable_negetive_counter = 0 ;
		for (int i = 0 ; i < cnf.size() ; i++)
		{
			vector<int > clause = cnf[i];

			for (int j = 0 ; j < clause.size() ; j++)
			{
				if (clause[j] == check_variable)
				{
					check_variable_positive_counter++; // increment count by 1 
				}
				else if (clause[j] == (-check_variable))
				{
					check_variable_negetive_counter++; // increment count by 1  
				}
			}
		}

		if ((check_variable_negetive_counter > 0) && (check_variable_positive_counter == 0) )
		{
			setZeroLiteral(check_variable);
		}
		else if ((check_variable_positive_counter > 0) && (check_variable_negetive_counter == 0) )
		{
			setOneLiteral(check_variable);
		}
	}

	// Commence CNF checks here
	if (cnf.size() == 0 )
	{
		cout << "***************** FINAL RESULT ************************** " << endl ; 
		cout << "THE GIVE CIRCUITS ARE EQUIVALENT !! TERMINATING ALGORITHM !!! " << endl ; 
		cout << "********************************************************* " << endl ; 		
	}
	else if (emptyClause)// empty clause --> WRONG .. Needs FIXING FIXME 
	{
		// FIXME understand from Mathias Sauppe 

		cout << "***************** FINAL RESULT ************************** " << endl ; 
		cout << "THE GIVE CIRCUITS ARE NOT EQUIVALENT !! " << endl ; 
		cout << "********************************************************* " << endl ; 
		return ; // We Stop the algorithm here no use to contnue ... 
	}
	else
	{
		int cnf_rightmost_index = (cnf.size()-1); // randomising using the rightmost literal and setting the values on it ...
		vector<int> clause = cnf[cnf_rightmost_index];
		int backtrackLiteral = clause[(clause.size() - 1)];

		setZeroLiteral(backtrackLiteral); // should be creating a new cnf Automatically 
		DP(cnf);
		setOneLiteral(backtrackLiteral); 
		DP(cnf);  
		// Backtracking Logic 
	}
}

void setZeroLiteral (int Literal)
{
	for (int i = 0 ; i < cnf.size() ; i++)
	{
		vector <int > clause = cnf[i];
		for (int j = 0 ; j < clause.size() ; j++)
		{
			if(clause[j] == Literal)
			{
				//Literal to be removed 
				clause.erase(clause.begin()+j); //equivalent to putting 0 in place of a literal.

			}
			else if (clause[j] == (-Literal))
			{
				//clause to be removed !
				cnf.erase(cnf.begin()+i); //equivalent to puttng 1 in place of the literal.
				break; //goes to the next clause 
			}
		}
	}
}

void setOneLiteral (int Literal)
{
	for (int i = 0 ; i < cnf.size() ; i++)
	{
		vector <int > clause = cnf[i];
		for (int j = 0 ; j < clause.size() ; j++)
		{
			if(clause[j] == Literal)
			{
				//clause to be removed 
				cnf.erase(cnf.begin()+i); // equivalent to putting 1 in place of a literal.
				break; // goes to the next clause ...
			}
			else if (clause[j] == (-Literal))
			{
				//Literal to be removed !
				clause.erase(clause.begin()+j); // equivalent to putting 0 in place of the literal  
			}
		}
	}

}

int main(int argc, char ** argv)
{
	if (argc != 3)
	{
		cerr << "Wrong argument count!\n";
		return argReadERROR;
	}
	//How this works ? Simple! Just feed the files via the application pipe... <probably with the file systems>
	if (readFiles(argv[1], argv[2]) != 0)
	{
		cerr << "Error while reading files!\n";
		return fileReadERROR;
	}

	// The following global variables are now defined (examples are for file xor2.net):
	//
	// int netCount1, netCount2
	// - total number of nets in netlist 1 / 2
	// - e.g. netCount1 is 3
	//
	// vector<string> inputs1, outputs1, inputs2, outputs2
	// - names of inputs / outputs in netlist 1 / 2
	// - e.g. inputs1[0] contains "a"
	//
	// map<string, int> map1, map2
	// - mapping from input / output names to net numbers in netlist 1 / 2
	// - e.g. map1["a"] is 1, map1["b"] is 2, ...
	//
	// GateList gates1, gates2
	// - list (std::vector<Gate>) of all gates in netlist 1 / 2
	// - e.g.:
	//   - gates1[0].type is XOR
	//   - gates1[0].nets is std::vector<int> and contains three ints (one for each XOR port)
	//   - gates1[0].nets[0] is 1 (first XOR input port)
	//   - gates1[0].nets[1] is 2 (second XOR input port)
	//   - gates1[0].nets[2] is 3 (XOR output port)
	
	// Print out data structure - (for debugging)
	cout << "Netlist 1:\n==========\n";
	printDataForNetlist(1);
	cout << "\nNetlist 2:\n==========\n";
	printDataForNetlist(2);

	
	//
	// Add your to build the CNF.
	// The CNF should be a vector of vectors of ints. Each "inner" vector represents one clause. The "outer" vector represents the whole CNF.
	//
	int cnf_build = buildcnf(cnf); // netlists are global to the function mentioned 
	
	//
	// Check CNF for satisfiability using the Davis Putnam algorithm
	//
	if (cnf_build != 0)
	{
		cout << "CNF building Failed ... Marking whole SAT Algorithm as failure "; 
		return FAILURE;
	}
	else 
	{
		cout << "CNF build successful ... Now applying Davis Putnam Algorithm" << endl ;
		cout << "#############################################################" << endl; 
		cout << "CNF used is " << endl ;
		printCNF(cnf);  // TODO printing the table that we have created for the case of Circuit // Code Regen till here success --> dated 14/05/2015
        cout << "#############################################################" << endl ;
		// DP(cnf);
	}
	
	return SUCCESS;
}
