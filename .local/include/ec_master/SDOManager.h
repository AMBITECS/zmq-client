#ifndef SDOMANAGER_H
#define SDOMANAGER_H

#pragma once
#include "typedefs.h"
#include <mutex>
#include <vector>

class EtherCATMaster;

class SDOManager {
public:
    explicit SDOManager(EtherCATMaster& master);

    // Основные операции
    bool readSDO(uint16_t slave, uint16_t index, uint8_t subindex, void* data, size_t size);
    bool writeSDO(uint16_t slavePos, uint16_t index, uint8_t subindex, const void* data, size_t size);

    // Работа с объектным словарем
    // CoEDictionary readCoEDictionary(uint16_t slave);
    // bool downloadCoEObject(uint16_t slave, const CoEObjetEntry& entry, const std::vector<uint8_t>& data);
    // std::vector<uint8_t> uploadCoEObject(uint16_t slave, const CoEObjetEntry& entry);

    // Исключения
    class EtherCATException : public std::runtime_error {
    public:
        ECErrorCode code;
        EtherCATException(ECErrorCode code, const std::string& msg)
            : std::runtime_error(msg), code(code) {}
    };

private:
    EtherCATMaster& master_;
    mutable std::mutex sdo_mutex_;
};

#endif // SDOMANAGER_H
