#include "fileParser.h"

fileParser::fileParser(){}

fileParser::~fileParser(){}

int fileParser::readFile(std::string filename, netListDetails &netDetails)
{
    std::ifstream file(filename.c_str());

    if (! file.is_open())
    {
        std::cerr << "Failed opening file" << std::endl;
        return -1;
    }

    std::string curLine;

    // net count
    getline(file, curLine);
    netDetails.netCount = atoi(curLine.c_str());

    // inputs
    getline(file, curLine);
    std::stringstream inputsStream(curLine);
    std::string buf;
    while (inputsStream >> buf)
    {
        netDetails.inputs.push_back(buf);
    }

    // outputs
    getline(file, curLine);
    std::stringstream outputsStream(curLine);
    while (outputsStream >> buf)
    {
        netDetails.outputs.push_back(buf);
    }

    // mapping
    for (size_t i=0; i < netDetails.inputs.size() + netDetails.outputs.size(); i++)
    {
        getline(file, curLine);

        std::stringstream mappingStream(curLine);

        mappingStream >> buf;

        int curNet = atoi(buf.c_str());

        mappingStream >> buf;
        netDetails.map[buf] = curNet;
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
        std::stringstream gateStream(curLine);
        gateStream >> buf;

        Gate curGate;
        curGate.type = (buf == "and" ? AND : buf == "or" ? OR : buf == "inv" ? INV : buf == "xor" ? XOR : buf == "zero" ? ZERO : buf == "one" ? ONE : UNDEFINED);

        if (curGate.type == UNDEFINED)
        {
            std::cerr << "Undefined Gate type specified in the file ... exiting" << std::endl;
            return -1;
        }

        while (gateStream >> buf)
        {
            int curNet = atoi(buf.c_str());
            curGate.nets.push_back(curNet);
        }

        netDetails.Gates.push_back(curGate);
    }

    return 0;
}

int fileParser::readFiles( std::string filename1, std::string filename2)
{
    if (readFile(filename1, netDetails1) != 0)
    {
        std::cerr << "Failed reading file 1" << std::endl;
        return -1;
    }

    if (readFile(filename2, netDetails2) != 0)
    {
        std::cerr << "Failed reading file 2" << std::endl;
        return -1;
    }

    return 0;
}

void fileParser::printData(netListDetails netListDet)
{
    std::cout << "Net count: " << netListDet.netCount << std::endl << std::endl;
    std::cout << "Inputs:" << std::endl;

    for (size_t i=0; i< netListDet.inputs.size(); i++)
    {
        std::cout << netListDet.inputs[i] << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Outputs:" << std::endl;

    for (size_t i=0; i< netListDet.outputs.size(); i++)
    {
        std::cout << netListDet.outputs[i] << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Mapping (input/output port to net number):" << std::endl;

    for (size_t i=0; i< netListDet.inputs.size(); i++)
    {
        std::cout << netListDet.inputs[i] << " -> "<< netListDet.map[netListDet.inputs[i]] << std::endl;
    }

    for (size_t i=0; i< netListDet.outputs.size(); i++)
    {
        std::cout << netListDet.outputs[i] << " -> "<< netListDet.map[netListDet.outputs[i]] << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Gates:" << std::endl;

    for (size_t i=0; i< netListDet.Gates.size(); i++)
    {
        Gate & curGate = netListDet.Gates[i];
        std::cout << (curGate.type == AND ? "AND" : curGate.type == OR ? "OR" : curGate.type == INV ? "INV" : curGate.type == XOR ? "XOR" : curGate.type == ZERO ? "ZERO" : curGate.type == ONE ? "ONE" : "ERROR");
        std::cout << ": ";
        for (size_t j=0; j<curGate.nets.size(); j++)
        {
            std::cout << curGate.nets[j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void fileParser::printNetListData(int netlistNumber)
{
    if (netlistNumber == 1)
    {
        printData(netDetails1);
    }
    else if (netlistNumber == 2)
    {
        printData(netDetails2);
    }
    else
    {
        std::cout << "Invalid netlist number " << netlistNumber << " (must be 1 or 2)" << std::endl;
    }
}

netListDetails fileParser::getNetListDetails(int netList)
{
    if (netList == 1 )
    {
        return netDetails1;
    }
    else
    {
        return netDetails2;
    }
}

GateList fileParser::getGateList (int netList)
{
    if (netList == 1)
    {
        return netDetails1.Gates;
    }
    else
    {
        return netDetails2.Gates;
    }
}

int fileParser::getNumberOfNets(int netList)
{
    if (netList == 1)
    {
        return netDetails1.netCount;
    }
    else
    {
        return netDetails2.netCount;
    }
}

std::vector<std::string> fileParser::getInputs(int netList)
{
    if (netList == 1)
    {
        return netDetails1.inputs;
    }
    else
    {
        return netDetails2.inputs;
    }
}

std::vector<std::string> fileParser::getOutputs(int netList)
{
    if (netList == 1)
    {
        return netDetails1.outputs;
    }
    else
    {
        return netDetails2.outputs;
    }
}
