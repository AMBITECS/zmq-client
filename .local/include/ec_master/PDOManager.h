#ifndef PDOMANAGER_H
#define PDOMANAGER_H

#pragma once
#include "typedefs.h"
#include <vector>
#include <unordered_map>
#include <mutex>
#include <functional>
#include <bitset>

class EtherCATMaster;

class PDOManager {
public:
    explicit PDOManager(EtherCATMaster& master);

    // Конфигурация PDO
    bool configureRxPDO();
    bool configureTxPDO();
    bool configurePDO(uint16_t slave, const PDO& config);

    // Работа с PDO
    template <typename T>
    bool readPDO(uint16_t slave, uint16_t pdoIndex, uint8_t subindex, T& value);

    template <typename T>
    bool writePDO(uint16_t slave, uint16_t pdoIndex, uint8_t subindex, const T& value);

    // Получение конфигурации
    std::vector<PDO> getCachedRxPDO(uint16_t slave) const;
    std::vector<PDO> getCachedTxPDO(uint16_t slave) const;
    std::vector<PDO> readRxPDOConfiguration(uint16_t slave);
    std::vector<PDO> readTxPDOConfiguration(uint16_t slave);

    bool configureSlavePDOs(const std::vector<SlaveConfig>& slaveConfigs);

    // Обработка данных
    void processTxPDOData(uint16_t slave, const PDO& pdo);
    void processRxPDOData(uint16_t slave, const PDO& pdo);

    // Исключения
    class EtherCATException : public std::runtime_error {
    public:
        ECErrorCode code;
        EtherCATException(ECErrorCode code, const std::string& msg)
            : std::runtime_error(msg), code(code) {}
    };

private:
    std::variant<bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float32, float64>
    parseRawData(uint8_t* data, size_t offset, size_t size, const std::string& type);

    void packRawData(uint8_t* data, size_t offset, size_t size, const std::string& type,
    const std::variant<bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float32, float64> value);

    void show_data(int slave, const PDOData& data);
private:
    EtherCATMaster& master_;
    mutable std::mutex pdo_mutex_;
    std::unordered_map<uint16_t, std::vector<PDO>> rxPdoCache_;
    std::unordered_map<uint16_t, std::vector<PDO>> txPdoCache_;
};

#endif // PDOMANAGER_H
