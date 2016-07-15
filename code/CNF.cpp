#include "CNF.h"

CNF::CNF()
:m_netCount1(0)
{
}

CNF::~CNF() {}

void CNF::gateMapper(GateList gates, bool firstCircuit)
{
    for (int i = 0 ; i < gates.size(); i++ )
    {
        Gate currentGate = gates[i];
        std::vector<int> clause1;
        std::vector<int> clause2;
        std::vector<int> clause3;
        std::vector<int> clause4;

        switch (currentGate.type)
        {
            case XOR:
            {
                if (firstCircuit)
                {
                    clause1.push_back(currentGate.nets[0]);
                    clause1.push_back(currentGate.nets[1]);
                    clause1.push_back(-currentGate.nets[2]);

                    clause2.push_back(-currentGate.nets[0]);
                    clause2.push_back(-currentGate.nets[1]);
                    clause2.push_back(-currentGate.nets[2]);

                    clause3.push_back(currentGate.nets[0]);
                    clause3.push_back(-currentGate.nets[1]);
                    clause3.push_back(currentGate.nets[2]);

                    clause4.push_back(-currentGate.nets[0]);
                    clause4.push_back(currentGate.nets[1]);
                    clause4.push_back(currentGate.nets[2]);

                }
                else
                {
                    clause1.push_back(m_netCount1 + currentGate.nets[0]);
                    clause1.push_back(m_netCount1 + currentGate.nets[1]);
                    clause1.push_back(-m_netCount1 - currentGate.nets[2]);

                    clause2.push_back(-m_netCount1 - currentGate.nets[0]);
                    clause2.push_back(-m_netCount1 - currentGate.nets[1]);
                    clause2.push_back(-m_netCount1 - currentGate.nets[2]);

                    clause3.push_back(m_netCount1 + currentGate.nets[0]);
                    clause3.push_back(-m_netCount1 - currentGate.nets[1]);
                    clause3.push_back(m_netCount1 + currentGate.nets[2]);

                    clause4.push_back(-m_netCount1 - currentGate.nets[0]);
                    clause4.push_back(m_netCount1 + currentGate.nets[1]);
                    clause4.push_back(m_netCount1 + currentGate.nets[2]);
                }
                break;
            }

            case AND:
            {
                if (firstCircuit)
                {
                    clause1.push_back(currentGate.nets[0]);
                    clause1.push_back(-currentGate.nets[2]);

                    clause2.push_back(currentGate.nets[1]);
                    clause2.push_back(-currentGate.nets[2]);

                    clause3.push_back(-currentGate.nets[0]);
                    clause3.push_back(-currentGate.nets[1]);
                    clause3.push_back(currentGate.nets[2]);
                }
                else
                {
                    clause1.push_back(m_netCount1 + currentGate.nets[0]);
                    clause1.push_back(-m_netCount1 - currentGate.nets[2]);

                    clause2.push_back(m_netCount1 + currentGate.nets[1]);
                    clause2.push_back(-m_netCount1 - currentGate.nets[2]);

                    clause3.push_back(-m_netCount1 - currentGate.nets[0]);
                    clause3.push_back(-m_netCount1 - currentGate.nets[1]);
                    clause3.push_back(m_netCount1 + currentGate.nets[2]);
                }
                break;
            }

            case OR:
            {
                if (firstCircuit)
                {
                    clause1.push_back(-currentGate.nets[0]);
                    clause1.push_back(currentGate.nets[2]);

                    clause2.push_back(-currentGate.nets[1]);
                    clause2.push_back(currentGate.nets[2]);

                    clause3.push_back(currentGate.nets[0]);
                    clause3.push_back(currentGate.nets[1]);
                    clause3.push_back(-currentGate.nets[2]);
                }
                else
                {
                    clause1.push_back(-m_netCount1 - currentGate.nets[0]);
                    clause1.push_back(m_netCount1 + currentGate.nets[2]);

                    clause2.push_back(-m_netCount1 - currentGate.nets[1]);
                    clause2.push_back(m_netCount1 + currentGate.nets[2]);

                    clause3.push_back(m_netCount1 + currentGate.nets[0]);
                    clause3.push_back(m_netCount1 + currentGate.nets[1]);
                    clause3.push_back(-m_netCount1 - currentGate.nets[2]);
                }
                break;
            }

            case INV:
            {
                if (firstCircuit)
                {
                    clause1.push_back(currentGate.nets[0]);
                    clause1.push_back(currentGate.nets[1]);

                    clause2.push_back(-currentGate.nets[0]);
                    clause2.push_back(-currentGate.nets[1]);
                }
                else
                {
                    clause1.push_back(m_netCount1 + currentGate.nets[0]);
                    clause1.push_back(m_netCount1 + currentGate.nets[1]);

                    clause2.push_back(-m_netCount1 - currentGate.nets[0]);
                    clause2.push_back(-m_netCount1 - currentGate.nets[1]);
                }
                break;
            }

            case ZERO:
            case ONE:
            case UNDEFINED:
                break; // nothing is performed here as we dont process these gates
        }

        if (!clause1.empty())
        {
            cnf.push_back(clause1);
        }

        if (!clause2.empty())
        {
            cnf.push_back(clause2);
        }

        if (!clause3.empty())
        {
            cnf.push_back(clause3);
        }

        if (!clause4.empty())
        {
            cnf.push_back(clause4);
        }
    }
}

bool CNF::createCNF(netListDetails netList1, netListDetails netList2)
{
    // for netList calculations
    m_netCount1 = netList1.netCount;

    // for building the inputs of the Miter Circuitry
    for(int i = 0; i < netList1.inputs.size(); i++)
    {
        int a=0, b=0;
        std::vector<int> clause1;
        std::vector<int> clause2;

        a = netList1.map[netList1.inputs[i]];
        b = netList1.netCount + netList2.map[netList1.inputs[i]];

        clause1.push_back(a);
        clause1.push_back(-b);
        cnf.push_back(clause1);

        clause2.push_back(-a);
        clause2.push_back(b);
        cnf.push_back(clause2);
    }

    // Dump CNFs for the corresponding Gates in the Net list into the Miter Circuitry
    gateMapper(netList1.Gates, true);
    gateMapper(netList2.Gates, false);

    std::map<std::string, int>::iterator it1, it2;
    int stop1, stop2;
    stop1 = 0;

    for ( it1= netList1.map.begin(); it1!= netList1.map.end() ; ++it1 )
    {
        stop2 = 0;

        for ( it2= netList2.map.begin(); it2!= netList2.map.end() ; ++it2 )
        {
            if (it1->first == it2->first)
            {
                int a=-it1->second;
                int b=it2->second + m_netCount1;
                std::vector<int> clause1;
                std::vector<int> clause2;

                clause1.push_back(-a);
                clause1.push_back(-b);
                cnf.push_back(clause1);

                clause2.push_back(a);
                clause2.push_back(b);
                cnf.push_back(clause2);
            }

            stop2 += 1;

            if ( stop2 == netList1.inputs.size() )
            {
                break;
            }
        }

        stop1+=1;

        if ( stop1 == netList1.inputs.size() )
        {
            std::cout<< std::endl ;
            break;
        }
    }

    // Dump the output to the Miter Circuitry
    std::vector<int> out;
    int a,b,c = 0;
    for(size_t i=0; i< netList1.outputs.size();i++ )
    {
        for (size_t k = 0; k < netList1.outputs.size(); k++)
        {
            if ( netList1.outputs[i] == netList2.outputs[k])
            {
                a = netList1.map[netList1.outputs[i]];
                b = netList2.map[netList2.outputs[k]] + netList1.netCount ;
                c = netList1.netCount + netList2.netCount + (int) i + 1 ;
                out.push_back(c);

                std::vector<int> clause1;
                std::vector<int> clause2;
                std::vector<int> clause3;
                std::vector<int> clause4;

                clause1.push_back(a);
                clause1.push_back(b);
                clause1.push_back(-c);
                cnf.push_back(clause1);

                clause2.push_back(-a);
                clause2.push_back(-b);
                clause2.push_back(-c);
                cnf.push_back(clause2);

                clause3.push_back(-a);
                clause3.push_back(b);
                clause3.push_back(c);
                cnf.push_back(clause3);

                clause4.push_back(a);
                clause4.push_back(-b);
                clause4.push_back(c);
                cnf.push_back(clause4);
            }
        }
    }
    m_miterFinalNetCount = c; 
    cnf.push_back(out);

    return true; // All success
}

void CNF::printCNF()
{
    std::cout << "*************************************************" << std::endl;
    std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ " << std::endl;
    std::cout << "CNF Size is : " << cnf.size() << std::endl;
    std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ " << std::endl;

    for (int i = 0; i < cnf.size(); i++)
    {
        std::vector<int> clauses = cnf[i];

        for (int j = 0; j < clauses.size(); j++)
        {
            if (j != 0)
            {
                std::cout << " v ";

            }
            else
            {
                std::cout << " (";
            }
            std::cout << clauses[j];
        }
        std::cout << ") " << std::endl;
    }
    std::cout << std::endl;
    std::cout << "*************************************************" << std::endl;
}

void CNF::printCNF(std::vector<std::vector<int> > cnfData)
{
    std::cout << "*************************************************" << std::endl;
    std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ " << std::endl;
    std::cout << "CNF Size is : " << cnfData.size() << std::endl;
    std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ " << std::endl;

    for (int i = 0; i < cnfData.size(); i++)
    {
        std::vector<int> clauses = cnfData[i];

        for (int j = 0; j < clauses.size(); j++)
        {
            if (j != 0)
            {
                std::cout << " v ";

            }
            else
            {
                std::cout << " (";
            }
            std::cout << clauses[j];
        }
        std::cout << ") " << std::endl;
    }
    std::cout << std::endl;
    std::cout << "*************************************************" << std::endl;
}


std::vector<std::vector<int> > CNF::getCNF()
{
    return cnf;
}

int CNF::getMiterCircuitFinalNetCount()
{
    return m_miterFinalNetCount;
}
