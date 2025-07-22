#ifndef ENIPARSER_H
#define ENIPARSER_H

#include "typedefs.h"

class ENIParser
{
public:
    ENIParser();
    ~ENIParser();

    // Конфигурация и инициализация
    bool loadConfig(const std::string& filename, MasterConfig &masterConfig, std::vector<SlaveConfig> &slaveConfigs);
    bool saveConfig(const std::string& filename, MasterConfig &masterConfig, std::vector<SlaveConfig> &slaveConfigs);

private:
    bool parseMasterConfig(xmlNodePtr node, MasterConfig &masterConfig);
    bool parseSlaveConfig(xmlNodePtr node, std::vector<SlaveConfig> &slaveConfigs);
    PDO parsePdo(xmlNodePtr pdoNode, bool isRx);
    PDOEntry parsePdoEntry(xmlNodePtr entryNode);
    SlaveConfig::SYNCManager parseSyncManager(xmlNodePtr smNode);
    SlaveConfig::InitCommand parseInitCommand(xmlNodePtr cmdNode);
    SlaveConfig::DistributedClock parseDistributedClock(xmlNodePtr dcNode);
    SlaveConfig::Mailbox parseMailbox(xmlNodePtr mbNode);
    SlaveConfig::Fmmu parseFmmu(xmlNodePtr fmmuNode);
    SlaveConfig::ErrorHandling parseErrorHandling(xmlNodePtr ehNode);

    void logMessage(const std::string& message) const;
    void errorMessage(SOEMErrorCode code, const std::string& message, const std::string& detail = "", int slavePos = -1);

    std::string toHexString(uint32_t value) const;

};

#endif // ENIPARSER_H
