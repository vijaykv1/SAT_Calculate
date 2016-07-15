/*!
 * @File dataManagement.h
 * @Authors : Varun Vijaykumar <varun.vijaykumar@s2014.tu-chemnitz.de>
 *
 * @date : 12 - June - 2016
 * @brief : To maintain the data Types used in the project
 */

#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>

/*!
 * @brief enum for representing different types of Gates
 */
typedef enum {
    AND, /*!< And Gate */
    OR,  /*!< Or Gate */
    INV, /*!< Not Gate */
    XOR, /*!< Exor Gate */
    ZERO,/*!< Zero Gate */
    ONE, /*!< One Gate */
    UNDEFINED /*!< in case of unidentified gates */
} GateType;

/*!
 * @brief Structure for describing the properties of the Gate
 */
typedef struct {
    GateType type;          /*!< the gate type */
    std::vector<int> nets;  /*!< Nets of the gate */
} Gate;
typedef std::vector<Gate> GateList;

/*!
 * @brief Structure to describe the data read from the file
 */
typedef struct _netListDetails {
    int netCount;                     /*!< Number of net in the circuitry */
    std::vector<std::string> inputs;  /*!< Number of inputs in the circuitry */
    std::vector<std::string> outputs; /*!< Number of output in the circuitry */
    std::map<std::string, int> map;   /*!< mapping of nets in the circuitry (in & out only) */
    GateList Gates;                   /*!< List of gates in circuitry */
} netListDetails;
