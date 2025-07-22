#ifndef DCMANAGER_H
#define DCMANAGER_H

#include "typedefs.h"
#include <chrono>
#include <mutex>

class EtherCATMaster;

class DCManager {
public:
    explicit DCManager(EtherCATMaster& master);
    ~DCManager();

    // Включение/выключение Distributed Clocks
    bool enable(bool enable);
    bool isEnabled() const;

    // Конфигурация синхронизации
    bool configure(int64_t cycleTimeNs, int64_t syncWindowNs);
    bool sync(int64_t cycleTimeNs, int64_t syncWindowNs);

    // Получение информации
    int64_t getSystemTime() const;
    int64_t getSlaveClock(uint16_t slave) const;
    int32_t getClockDifference(uint16_t slave) const;

    bool checkDCSupport(uint16_t slave) const;

    // Управление синхронизацией
    bool startSync();
    bool stopSync();
    bool isSynced() const;

    // Калибровка
    bool calibrateClocks();
    bool adjustClockOffset(uint16_t slave, int32_t offsetNs);

    // Управление компенсацией дрейфа
    bool enableDriftCompensation(bool enable, int32_t maxDriftNs = 1000,
                                 int32_t compensationIntervalMs = 10000);
    bool isDriftCompensationEnabled() const;

private:
    EtherCATMaster& master_;
    mutable std::mutex dc_mutex_;
    bool dc_enabled_ = false;
    bool dc_synced_ = false;

    // Параметры компенсации дрейфа
    std::atomic<bool> drift_comp_enabled_{false};
    std::atomic<int32_t> max_drift_ns_{1000};
    std::atomic<int32_t> comp_interval_ms_{10000};
    std::thread drift_comp_thread_;
    std::atomic<bool> drift_comp_running_{false};

    // Метод для потока компенсации
    void driftCompensationThread();

    void logDCStatus() const;
};

#endif // DCMANAGER_H
