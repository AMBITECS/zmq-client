#ifndef MAILBOXMANAGER_H
#define MAILBOXMANAGER_H

#include <vector>
#include <mutex>
#include <functional>
#include <memory>
#include <cstring>
#include "typedefs.h"
#include "soem/ethercat.h"

class EtherCATMaster;

class MailBoxManager {
public:
    explicit MailBoxManager(EtherCATMaster& master);

    // Основные функции конфигурации
    bool configure(uint16_t slave, const MailboxConfig& config);
    bool enable(uint16_t slave, bool enable = true);
    bool isEnabled(uint16_t slave) const;
    bool checkSupport(uint16_t slave) const;

    // Работа с протоколами
    bool setProtocols(uint16_t slave, bool coe, bool foe, bool eoe, bool soe);
    bool isProtocolSupported(uint16_t slave, MailboxProtocol protocol) const;

    // Обмен данными
    bool send(uint16_t slave, const std::vector<uint8_t>& data);
    std::vector<uint8_t> receive(uint16_t slave);

    // Асинхронная обработка
    void setReceiveCallback(uint16_t slave, std::function<void(const std::vector<uint8_t>&)> callback);
    void processIncomingMessages();

    // Статус и информация
    MailboxStatus getStatus(uint16_t slave) const;
    std::string getProtocolInfo(uint16_t slave) const;

    void setEmergencyCallback(uint16_t slave, std::function<void(const std::vector<uint8_t>&)> callback);
    void processAsyncMessages();

private:
    bool checkForAsyncMessage(uint16_t slave);
    bool verifySlaveSupport(uint16_t slave);
    std::vector<uint8_t> receiveEmergencyMessage(uint16_t slave);

private:
    EtherCATMaster& master_;
    mutable std::mutex mutex_;

    struct SlaveMailboxInfo {
        std::function<void(const std::vector<uint8_t>&)> callback;
        uint32_t lastActivity = 0;
        std::function<void(const std::vector<uint8_t>&)> emergencyCallback;
        uint32_t lastEmergencyCounter = 0;
    };

    std::unordered_map<uint16_t, SlaveMailboxInfo> slaveMailboxes_;

    bool checkBufferSize(uint16_t slave, size_t size) const;
};

#endif // MAILBOXMANAGER_H
