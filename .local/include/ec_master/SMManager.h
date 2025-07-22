#ifndef SMMANAGER_H
#define SMMANAGER_H

#include "typedefs.h"
#include <vector>
#include <mutex>
#include <unordered_map>

class EtherCATMaster;

class SMManager {
public:
    explicit SMManager(EtherCATMaster& master);

    // Конфигурация Sync Manager
    bool configureSyncManager(uint16_t slave, const SyncManagerConfig& cfg);
    std::vector<SyncManagerConfig> getSyncManagerConfig(uint16_t slave) const;

    // Включение/выключение Sync Manager
    bool enableSyncManager(uint16_t slave, uint8_t smIndex, bool enable);

    // Проверка состояния Sync Manager
    bool isSyncManagerEnabled(uint16_t slave, uint8_t smIndex) const;
    bool isSyncManagerActive(uint16_t slave, uint8_t smIndex) const;

    // Получение информации
    uint16_t getSyncManagerStartAddress(uint16_t slave, uint8_t smIndex) const;
    uint16_t getSyncManagerLength(uint16_t slave, uint8_t smIndex) const;

    // Обновление конфигурации
    void updateConfiguration();

private:
    EtherCATMaster& master_;
    mutable std::mutex sm_mutex_;

    bool validateSMIndex(uint16_t slave, uint8_t smIndex) const;
};

#endif // SMMANAGER_H
