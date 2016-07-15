/*!
 * @file fileParser.h
 * @Authors : Varun Vijaykumar <varun.vijaykumar@s2014.tu-chemnitz.de>
 *
 * @date : 12 - June - 2016
 * @brief : To perform File parsing operations
 */

#pragma once

#include <fstream>
#include <sstream>
#include "dataManagement.h"

// For File reading and collecting Data
/*!
 * @brief Class to read the .net files that are fed to the binary and arrange data in accordance to the net list type
 */
class fileParser
{

public:

    /*!
     * @brief fileParser Class constructor
     */
    fileParser();

    /*!
     * @brief fileparser Class destructor
     */
    ~fileParser();

    /*!
     * @brief Function to read both the files
     * @param filename1 file name to be read.
     * @param filename2 file name to be read.
     * @return 0 if success else -1
     */
    int readFiles( std::string filename1, std::string filename2);

    /*!
     * @brief Function to print the net List details obtained via file parsing
     * @param netListNumber The net list to be printed.
     */
    void printNetListData(int netListNumber);

    /*!
     * @brief Function to get the details obtained via file parsing
     * @param netList the net list to be obtained.
     * @return structure information of the netlist circuitry
     */
    netListDetails getNetListDetails(int netList);

    /*!
     * @brief Function to get the list of gates in circuitry
     * @param Circuitry number
     * @return List of Gates
     */
    GateList getGateList (int netList);

    /*!
     * @brief function to get the number of nets in the specified circuitry
     * @param circuitry number
     * @return number of nets
     */
    int getNumberOfNets(int netList);

    /*!
     * @brief function to extract the input information
     * @param circuitry number
     * @return list of inputs of the circuitry
     */
    std::vector<std::string> getInputs(int netList);

    /*!
     * @brief function to extract the outputs of the circuitry
     * @param circuitry number
     * @return list of outputs of the specified circuitry
     */
    std::vector<std::string> getOutputs(int netList);

private:

    /*!
     * @brief Function to read & parse the file specified.
     * @detail all the information will be pushed to net list structures.
     * @param filename file to be read.
     * @param netListDet the structure to be populated
     * @return 0 if success else -1
     */
    int readFile(std::string filename, netListDetails &netListDet);

    /*!
     * @brief Function to print the specified netlist structure.
     * @param netListDet netlist details to be printed.
     */
    void printData(netListDetails netListDet);

    netListDetails netDetails1; /*!< Details obtained for the first file */
    netListDetails netDetails2; /*!< Details obtained for the second file */
}; // class fileParser
