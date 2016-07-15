#include "DavisPutnam.h"

davisPutnam::davisPutnam(const netListDetails netDetails1, const netListDetails netDetails2, const int totalProcessingNetListCount) :
        m_MaxnetCount(totalProcessingNetListCount)
       ,m_net1(netDetails1)
       ,m_net2(netDetails2)
{
    variableSet = new int[totalProcessingNetListCount]; // create the set of variables necessary for assignment
}

davisPutnam::~davisPutnam() {}

bool davisPutnam::findPureLiterals(std::vector<std::vector<int> > cnf, int &pureLiteral)
{
    bool returnCode = false;
    size_t size_CNF = cnf.size();
    
    if (cnf.size() != 0)
    {
        for (int checkElement = 1; checkElement <= m_MaxnetCount; checkElement++)
        {
            int positiveElement = 0;
            int negetiveElement = 0;
            
            for (int i = 0; i < cnf.size(); i++)
            {
                if (std::find(cnf[i].begin(), cnf[i].end(), checkElement) != cnf[i].end())
                {
                    positiveElement++;
                }
                else if (std::find(cnf[i].begin(), cnf[i].end(), (-checkElement)) != cnf[i].end())
                {
                    negetiveElement++;
                }
                else
                {
                    std::vector<int> clause = cnf[i];
                    size_t lastElement = clause.size();
                    
                    if (clause.size() != 0)
                    {
                        // checking the end element as well
                        if (clause[lastElement - 1] == checkElement)
                        {
                            positiveElement++;
                        }
                        else if (clause[lastElement] == (-checkElement))
                        {
                            negetiveElement++;
                        }
                    }
                }
            }
            
            if (((negetiveElement > 0) && (positiveElement == 0)) ||
                ((positiveElement > 0) && (negetiveElement == 0)))
            {
                pureLiteral = checkElement;
                returnCode = true;
                break;
            }
            else
            {
                pureLiteral = 0;
            }
        }
    }
    return returnCode;
}

std::vector<std::vector<int> > davisPutnam::setVariableToOne(std::vector<std::vector<int> > cnf, int literal)
{
    // before setting, we will log the value into the variable list
    if (literal > 0)
    {
        variableSet[literal] = 1;
    }
    else
    {
        variableSet[literal] = 0;
    }
    
    std::vector<std::vector<int> > newcnf;
    for (int clause = 0 ; clause < cnf.size() ; clause++)
    {
        std::vector<int> interestClause = cnf[clause];
        if (std::find(interestClause.begin(), interestClause.end(), literal) != interestClause.end())
        {
            continue;
        }
        else if (std::find(interestClause.begin(), interestClause.end(), -literal) != interestClause.end())
        {
            std::vector<int> newClause;
            if (interestClause.size() == 1)
            {
                newClause.push_back(0);
            }
            else
            {
                for (int clauseElement = 0 ; clauseElement < interestClause.size() ; clauseElement++ )
                {
                    if (interestClause[clauseElement] != -literal)
                    {
                        newClause.push_back(interestClause[clauseElement]);
                    }
                }
            }
            newcnf.push_back(newClause);
        }
        else
        {
            newcnf.push_back(cnf[clause]);
        }
    }
    
    return newcnf;
}

bool davisPutnam::isPureLiteral(std::vector<std::vector<int> > cnf, int literaltoSearch)
{
    for (int clause = 0 ; clause < cnf.size() ; clause++)
    {
        std::vector<int> interestClause = cnf[clause];
        
        if (std::find(interestClause.begin(), interestClause.end(), -literaltoSearch) != interestClause.end())
        {
            return false;
        }
    }
    return true;
}

bool davisPutnam::findUnitClause(std::vector<std::vector<int> > cnf, int &unitClauseObtained)
{
    for (int clause = 0 ; clause < cnf.size() ; clause++)
    {
        if (cnf[clause].size() == 1 && cnf[clause][0] != 0)
        {
            unitClauseObtained = cnf[clause][0];
            return true;
        }
    }
    return false;
}

bool davisPutnam::findemptyClause(std::vector<std::vector<int> > cnf)
{
    for (int clause = 0 ; clause < cnf.size() ; clause++)
    {
        std::vector<int> interestClause = cnf[clause];
        
        if (interestClause.size() == 1)
        {
            if (std::find(interestClause.begin(), interestClause.end(),0) != interestClause.end())
            {
                return true; // found a zero in one of the clauses ... lets restart the DP again in this
            }
        }
    }
    
    return false;
}

void davisPutnam::algorithm(std::vector<std::vector<int> > cnf)
{
    int pureLiteral = 0;
    int unitClauseObtained = 0;
    while( findPureLiterals(cnf, pureLiteral) || findUnitClause(cnf,unitClauseObtained) )
    {
        
        while(findPureLiterals(cnf, pureLiteral))
        {
            std::vector<std::vector<int> > newcnf;
            // std::cout << "pure literal processing";
            
#ifdef DEBUG_ENABLED
            std::cout << "Set pure Literal " << pureLiteral << " with zero" << std::endl;
#endif
            newcnf = setVariableToOne(cnf, pureLiteral);
            cnf = newcnf;
#ifdef DEBUG_ENABLED
            std::cout << "After performing pure literal processing for " << pureLiteral << std::endl;
            printCNF(cnf);
#endif
            
        }
        
        while (findUnitClause(cnf,unitClauseObtained))
        {
            std::vector<std::vector<int> > newcnf;
#ifdef DEBUG_ENABLED
            std::cout << "Setting unit clause found:" << unitClauseObtained << " to One" << std::endl;
#endif
            
            newcnf = setVariableToOne(cnf, unitClauseObtained);
            cnf = newcnf;
            
#ifdef DEBUG_ENABLED
            std::cout << " CNF obtained after unitClause parsing for "<< unitClauseObtained << std::endl;
            printCNF(cnf);
#endif
        }
        
        break;
    }
    
    // termination conditions
#ifdef DEBUG_ENABLED
    std::cout << " Entering Termination part of the algorithm" << std::endl;
#endif
    
    if (cnf.size() == 0)
    {
        std::cout << "Netlists Not Equivalent" << std::endl;
        std::cout << "Counter Example" << std::endl;
        // take the first circuit ..
        std::vector<std::string> input_Names = m_net1.inputs;
        std::map<std::string,int> netMap = m_net1.map;
        std::vector<std::string> output_names = m_net1.outputs;
        std::vector<std::string> output_names_net2 = m_net2.outputs;
        
        //for inputs only
        for (int inpuNet = 0; inpuNet < input_Names.size(); inpuNet++ )
        {
            std::cout << input_Names[inpuNet] << " --> " <<variableSet[netMap[input_Names[inpuNet]]] << std::endl;
        }
        
        //for outputs only ..
        for (int outNet = 0; outNet < output_names.size(); outNet++ )
        {
            std::cout << "netlist one output " << " --> " << variableSet[netMap[input_Names[outNet]]] << std::endl;
            std::cout << "netlist two output " << " --> " << variableSet[(netMap[input_Names[outNet]] + m_net2.netCount)] << std::endl;
        }
        
        exit(0);
    }
    else if (findemptyClause(cnf))
    {
        return;
    }
    else
    {
        // BACKTRACKING ALGORITHM
        
#ifdef DEBUG_ENABLED
        std::cout << " Entering Backtracking part of the algorithm" << std::endl;
        std::cout << "The rightmost element selected is : " << cnf[cnf.size()-1][cnf[cnf.size()-1].size()-1] << std::endl;
#endif
        
        int firstBurstElement = cnf.back().back();
        std::vector<std::vector<int> > cnf0 = setVariableToOne(cnf, -firstBurstElement);
#ifdef DEBUG_ENABLED
        std::cout << "Setting rightmost element " << firstBurstElement << " to Zero " << std::endl;
        printCNF(cnf0);
#endif        
        algorithm(cnf0);
        
        std::vector<std::vector<int> > cnf1 = setVariableToOne(cnf, firstBurstElement);
#ifdef DEBUG_ENABLED
        std::cout << "Failed with Setting rightmost element " << firstBurstElement << " to Zero, now setting to One " << std::endl;
        printCNF(cnf1);
#endif
        algorithm(cnf1);
    }
}
