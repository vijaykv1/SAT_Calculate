#include "DavisPutnam.h"
#include "fileParser.h"

int main(int argc, char ** argv)
{
    // 2 File availability check
    if (argc != 3)
    {
        std::cerr << "Wrong argument count!" << std::endl;
        return -1;
    }

    // File Parsing Class handle
    fileParser *netListFileParser = new fileParser();
    if (!netListFileParser)
    {
        std::cerr << "Invalid file parser object" << std::endl;
        return -1;
    }
    
    if (netListFileParser->readFiles(argv[1], argv[2]) != 0)
    {
        std::cerr << "Error while reading files! " << std::endl;
        return -1;
    }

    // Since Only two files are read, we can run a loop and call the functions
    for (int i = 1; i < 3 ; i++)
    {
        std::cout << "NetList " << i << ":" << std::endl << "==========" << std::endl;
        netListFileParser->printNetListData(i);
    }

    // data extracted by now , lets give this for further processing to the CNF creator class
    // Create CNF
    CNF *cnfBuilder = new CNF();
    if (!cnfBuilder)
    {
        std::cerr << "Invalid CNF object" << std::endl;
        return -1;
    }
    
    if (!cnfBuilder->createCNF(netListFileParser->getNetListDetails(1), netListFileParser->getNetListDetails(2)))
    {
        std::cerr << "CNF Creation Failed" << std::endl;
        return -1;
    }

    // Only for debug
    cnfBuilder->printCNF();

    // created CNF, lets Run Davis putnam on the CNF
    davisPutnam *dp = new davisPutnam(netListFileParser->getNetListDetails(1), netListFileParser->getNetListDetails(2), cnfBuilder->getMiterCircuitFinalNetCount());
    if(!dp)
    {
        std::cerr << "Invalid davis putnam object" << std::endl;
        return -1;
    }
    
    //start algorithm
    dp->algorithm(cnfBuilder->getCNF());
 
    // Will reach here only if the Algorithm has ended with the success
    std::cout<<"Net Lists are Equivalent."<< std::endl;

    return 0;
}
