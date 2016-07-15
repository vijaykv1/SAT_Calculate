/*!
 * @File DavisPutnam.h
 * @Authors : Varun Vijaykumar <varun.vijaykumar@s2014.tu-chemnitz.de>
 *
 * @date : 12 - June - 2016
 * @brief : To perform Circuit Equivalence testing using SAT approach (Davis Putnam)
 */

#pragma once

#include <algorithm>
#include "dataManagement.h"
#include "CNF.h"


//#define DEBUG_ENABLED

/*!
 * @brief Class for performing Davis Putnam Algorithm
 */
class davisPutnam : public CNF
{
public:

    /*!
     * @brief davisPutnam class constructor
     */
    davisPutnam(const netListDetails netDetails1, const netListDetails netDetails2, const int totalProcessingNetListCount);

    /*!
     * @brief davisPutnam destructor
     */
    ~davisPutnam();

    /*!
     * @brief The Davis putnam algorithm
     * @param cnf the CNF to be processed
     */
    void algorithm(std::vector<std::vector<int> > cnf);

private:

    /*!
     * @brief Find pure Literal in CNF
     * @param cnf the CNF to be processed
     * @param pureLiteralList the list of purliterals (bty reference)
     * @return number of pureliterals available
     */
    bool findPureLiterals(std::vector<std::vector<int> > cnf, int &pureLiterals);

    /*!
     * @brief To find if the literal passed is a pure literal
     * @param cnf the CNF to be processed
     * @return true or false
     */
    bool isPureLiteral(std::vector<std::vector<int> > cnf, int literaltoSearch);

    /*!
     * @brief Find unit clause in CNF
     * @param cnf the CNF to be processed
     * @param unitClauseObtained the unit clause obtained
     * @return true or false
     */
    bool findUnitClause(std::vector<std::vector<int> > cnf, int &unitClauseObtained);

    /*!
     * @brief Find empty clause in CNF
     * @param cnf the CNF to be processed
     * @return true or false
     */
    bool findemptyClause(std::vector<std::vector<int> > cnf);

    /*!
     * @brief To set the literal specified as a boolean 1
     * @param cnf the CNF to be processed
     * @return The new cnf after processing
     */
    std::vector<std::vector<int> > setVariableToOne(std::vector<std::vector<int> > cnf, int literal);

    std::map<int, int> Cnf_Variables; /*!< CNF variables copy */
    int *variableSet;
    int m_MaxnetCount; /*!< Maximum possible nets after CNF */

    netListDetails m_net1; /*!< net lists read from file 1 */
    netListDetails m_net2; /*!< net lists read from file 2 */
};
