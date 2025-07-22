#ifndef NETVARPARSER_H
#define NETVARPARSER_H

#include <string>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <cctype>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <cstdint>

using namespace std;

struct NetVar {
    uint16_t index;
    uint16_t subindex;
    int bitlen;
    std::string name;
    std::string type;
    std::string mem_link;
};

struct NetVarPDO {
    std::string index;
    std::string name;
    std::vector<NetVar> netVars;
};

struct NetVarSlave {
    int address;
    std::vector<NetVarPDO> rx_pdos;
    std::vector<NetVarPDO> tx_pdos;
};

struct NetworkVariablesConfig {
    std::string version;
    std::vector<NetVarSlave> slaves;
};

class NetVarParser {

public:
    NetworkVariablesConfig parse(const std::string& filename);

private:
    uint32_t parseHex(const string& hexStr);

};

#endif // NETVARPARSER_H
