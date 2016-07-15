/*!
 * @File CNF.h
 * @Authors : Varun Vijaykumar <varun.vijaykumar@s2014.tu-chemnitz.de>
 *
 * @date : 12 - June - 2016
 * @brief : To perform CNF Management
 */

#pragma once

#include "dataManagement.h"

/*!
 * @brief Class to CNF Management
 */
class CNF
{

public:

    /*!
     * @brief CNF Class constructor
     */
    CNF();

    /*!
     * @brief CNF Class destructor
     */
    ~CNF();

    /*!
     * @brief to create cnf based on the net list details
     * @param  netList1 First Circuitry netlist
     * @param  netList2 Second Circuitry netlist
     * @return true if success else false
     */
    bool createCNF(netListDetails netList1, netListDetails netList2);

    /*!
     * @brief to extract CNF from the CNF creator
     */
    std::vector<std::vector<int> > getCNF();

    /*!
     * @brief to print the CNF obtained
     */
    void printCNF();

    /*!
     * @brief to print the CNF obtained
     * @param cnfData CNF to print
     */
    void printCNF(std::vector <std::vector<int> > cnfData);

    /*!
     * @brief to get the final net count of Miter circuitry
     * @return final miter netcount
     */
    int getMiterCircuitFinalNetCount();

private:

    /*!
     * @brief to map the CNF according to the Gate type
     * @gates list of gates
     * @firstCircuit true if this the first file input
     */
    void gateMapper(GateList gates, bool firstCircuit);

    std::vector <std::vector<int> > cnf; /*!< cnf to work on */
    int m_netCount1; /*!< net count of First circuitry */
    int m_miterFinalNetCount; /*!< Final Miter Circuitry net count */

}; // Class CNF
