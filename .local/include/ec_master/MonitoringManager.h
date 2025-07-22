#ifndef MONITORINGMANAGER_H
#define MONITORINGMANAGER_H

#include "typedefs.h"
#include <atomic>
#include <thread>
#include <mutex>
#include <functional>
#include <unordered_map>

class EtherCATMaster;

class MonitoringManager {
public:
    explicit MonitoringManager(EtherCATMaster& master);
    ~MonitoringManager();

    // Управление мониторингом
    void start(uint32_t interval_ms = 1000);
    void stop();
    bool isRunning() const;

    // Callback-функции
    void setStateCallback(StateCallback cb);
    void setStatisticsCallback(StatisticsCallback cb);
    void setSlaveStateCallback(SlaveStateCallback cb);
    void setEmergencyCallback(EmergencyCallback cb);
    void setWarningCallback(WarningCallback cb);

    // Получение данных
    NetworkStatistics getStatistics() const;
    SlaveState getSlaveState(uint16_t slave) const;
    ConnectionState getConnectionState() const;

    // Настройки мониторинга
    void setMonitoringInterval(uint32_t interval_ms);
    uint32_t getMonitoringInterval() const;
    void setDetailedLogging(bool enable);
    bool getDetailedLogging() const;

    // Управление автоматическими реакциями
    void setAutoReaction(EventType type, std::function<void()> reaction,
                         bool enabled = true,
                         std::chrono::milliseconds minInterval = std::chrono::seconds(1));
    void enableAutoReaction(EventType type, bool enable);
    void removeAutoReaction(EventType type);
    void triggerReaction(EventType type);

    // Добавляем методы для управления переподключением
    void setReconnectSettings(int max_attempts, int delay_ms);
    bool attemptReconnect();
    bool isReconnecting() const;

    void updateStatistics(bool processDataSuccess, int wkc, std::chrono::microseconds cycleTime);

private:
    void monitoringThread();
    void updateConnectionState();
    void checkSlaveStates();
    void processEmergencyMessages();
    void checkNetworkHealth();

    // Методы для обработки реакций
    void checkAndTriggerReaction(EventType type);

private:
    EtherCATMaster& master_;
    mutable std::mutex monitor_mutex_;

    // Состояние мониторинга
    std::atomic<bool> running_{false};
    std::atomic<uint32_t> interval_ms_{1000};
    std::atomic<bool> detailed_logging_{false};
    std::thread monitor_thread_;

    // Данные мониторинга
    NetworkStatistics stats_{};
    std::unordered_map<uint16_t, SlaveState> slave_states_;
    ConnectionState connection_state_ = ConnectionState::Disconnected;

    // Callback-и
    StateCallback state_cb_;
    StatisticsCallback stats_cb_;
    SlaveStateCallback slave_state_cb_;
    EmergencyCallback emergency_cb_;
    WarningCallback warning_cb_;

    // Система автоматических реакций
    std::map<EventType, AutoReaction> auto_reactions_;
    mutable std::mutex reactions_mutex_;

    // Добавляем поля для управления переподключением
    std::atomic<bool> is_reconnecting_{false};
    std::atomic<int> reconnect_attempts_{0};
    std::atomic<int> max_reconnect_attempts_{5};
    std::atomic<int> reconnect_delay_ms_{2000};

};

#endif // MONITORINGMANAGER_H
