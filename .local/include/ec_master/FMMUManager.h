#ifndef FMMUMANAGER_H
#define FMMUMANAGER_H

#include "typedefs.h"
#include <vector>
#include <mutex>
#include <unordered_map>

class EtherCATMaster;

class FMMUManager {
public:
    explicit FMMUManager(EtherCATMaster& master);

    // Конфигурация FMMU
    bool configureFMMU(uint16_t slave, const FMMUConfig& cfg);
    std::vector<FMMUConfig> getFMMUConfig(uint16_t slave) const;

    // Включение/выключение FMMU
    bool enableFMMU(uint16_t slave, uint8_t fmmuIndex, bool enable);

    // Проверка состояния FMMU
    bool isFMMUEnabled(uint16_t slave, uint8_t fmmuIndex) const;
    bool isFMMUActive(uint16_t slave, uint8_t fmmuIndex) const;

    // Получение информации
    uint32_t getLogicalStartAddress(uint16_t slave, uint8_t fmmuIndex) const;
    uint16_t getLength(uint16_t slave, uint8_t fmmuIndex) const;
    uint8_t getType(uint16_t slave, uint8_t fmmuIndex) const;

private:
    EtherCATMaster& master_;
    mutable std::mutex fmmu_mutex_;

    bool validateFMMUIndex(uint16_t slave, uint8_t fmmuIndex) const;
};

#endif // FMMUMANAGER_H
