#ifndef EC_MASTER_H
#define EC_MASTER_H

#include <optional>

#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <string>
#include <algorithm>

#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "typedefs.h"
#include "interfaces.h"
#include "driver_factory.h"

#include "ESIParser.h"
#include "ENIParser.h"
#include "NetVarParser.h"
#include "soem/ethercat.h"

#include "SDOManager.h"
#include "PDOManager.h"
#include "SMManager.h"
#include "FMMUManager.h"
#include "DCManager.h"
#include "MonitoringManager.h"
#include "MailBoxManager.h"

#define EC_DIR_OUTPUT   0
#define EC_DIR_INPUT    1

class EtherCATMaster  : public IDriver, public IClientCallback {
    friend class SDOManager;
    friend class PDOManager;
    friend class SMManager;
    friend class FMMUManager;
    friend class DCManager;
    friend class MonitoringManager;
    friend class MailBoxManager;

public:
    explicit EtherCATMaster(const std::shared_ptr<IClientFactory>& _factory = nullptr);
    ~EtherCATMaster();

    void initialize() override;
    void shutdown() override;
    std::string name() const override;

    void onInit() override;
    void onExit() override;

    void updateInputs() override;
    void updateOutputs(const std::vector<OnDataChange> &data) override;

protected:
    // Доступ к менеджерам
    auto& pdoManager()  { return *pdo_manager_;         }
    auto& sdoManager()  { return *sdo_manager_;         }
    auto& smManager()   { return *sm_manager_;          }
    auto& fmmuManager() { return *fmmu_manager_;        }
    auto& dcManager()   { return *dc_manager_;          }
    auto& monitoring()  { return *monitoring_manager_;  }
    auto& mailbox()     { return *mailbox_manager_;     }

    // Загрузка конфигурации из ENI файла
    bool loadConfig(const std::string& eni_filename, const std::string& netvar_filename) {  net_var = netvar_parser.parse(netvar_filename); return eni_parser.loadConfig(eni_filename, masterConfig, slaveConfigs);    }
    // Сохранение конфигурации в новый ENI файл
    bool saveConfig(const std::string& filename) {  return eni_parser.saveConfig(filename, masterConfig, slaveConfigs);    }

    std::string getInterfaceNameByMac(const std::string& targetMac);

    // Основные методы
    bool init();
    void start();
    void stop();

    void slaveInfo(int slave);

    // Callback-и
    void setLogCallback(LogCallback cb)                 { log_cb_ = cb;                             }
    void setErrorCallback(ErrorCallback cb)             { error_cb_ = cb;                           }

    // Callback-и Мониторинг состояния
    void setStateCallback(StateCallback cb)             { monitoring().setStateCallback(cb);        }
    void setStatisticsCallback(StatisticsCallback cb)   { monitoring().setStatisticsCallback(cb);   }
    void setSlaveStateCallback(SlaveStateCallback cb)   { monitoring().setSlaveStateCallback(cb);   }
    void setEmergencyCallback(EmergencyCallback cb)     { monitoring().setEmergencyCallback(cb);    }
    void setWarningCallback(WarningCallback cb)         { monitoring().setWarningCallback(cb);      }

public:
    // Работа с регистрами
    bool readRegisters(int slavePos, uint16_t address, void* data, size_t size);
    bool writeRegisters(int slavePos, uint16_t address, const void* data, size_t size);

public:
    // Конфигурация SM
    bool configureSyncManager(uint16_t slave, const SyncManagerConfig& cfg){    return smManager().configureSyncManager(slave, cfg);   }

    // Исключения
    class EtherCATException : public std::runtime_error {
    public:
        ECErrorCode code;
        EtherCATException(ECErrorCode code, const std::string& msg)
            : std::runtime_error(msg), code(code) {}
    };

public:
    void setDetailedLogging(bool enable){ monitoring().setDetailedLogging(enable); }
    void startMonitoring(uint32_t interval_ms = 1000){ monitoring().start(interval_ms); }
    void stopMonitoring(){ monitoring().stop(); }

    // Мониторинг состояния
    ConnectionState getConnectionState() { return monitoring().getConnectionState(); }
    NetworkStatistics getStatistics() { return monitoring().getStatistics(); }
    SlaveState getSlaveState(uint16_t slave) { return monitoring().getSlaveState(slave); }

private:
    void processThread();

    bool configurePDO();
    bool configureSM();
    bool configureFMMU();
    bool configureDistributedClocks();
    bool configureMB();
    void setupAutoReactions();

    // Вспомогательные методы
    void errorMessage(int slave, ECErrorCode code, const std::string& msg);
    void logMessage(const std::string& message);
    bool validateSlave(int slave, bool throwException = true);
    bool waitForState(int slave, uint16_t state, int timeout_ms = 2000);
    void updateMasterConfiguration();
    size_t calculateIOMapSize();
    std::string toHexString(uint16_t value);

    // Методы для восстановления
    void restoreSlaveConfiguration();

private:
    std::shared_ptr<IRegClient> _m_client;
    //std::shared_ptr<IClientFactory> _m_factory;

    std::vector<RegItem> _reg_MS;
    std::vector<RegItem> _reg_SM;
    std::vector<ItemData> _reg_MS_data;
    std::vector<ItemData> _reg_SM_data;

    mutable std::mutex _reg_mutex;

private:
    void adjustCycleTime(std::chrono::microseconds actualCycleTime);
    void setTargetCycleTime(std::chrono::microseconds time);
    void enableAdaptiveCycle(bool enable);
    std::chrono::microseconds getCurrentCycleTime();
    std::chrono::microseconds getLastCycleDuration();

private:
    std::unique_ptr<PDOManager> pdo_manager_;
    std::unique_ptr<SDOManager> sdo_manager_;
    std::unique_ptr<SMManager> sm_manager_;
    std::unique_ptr<FMMUManager> fmmu_manager_;
    std::unique_ptr<DCManager> dc_manager_;
    std::unique_ptr<MonitoringManager> monitoring_manager_;
    std::unique_ptr<MailBoxManager> mailbox_manager_;

private:
    // Состояние
    std::string ifname_;
    bool is_initialized_ = false;
    bool is_running_ = false;
    std::thread thread_;

    // Синхронизация
    std::mutex state_mutex_;
    std::mutex pdo_mutex_;
    std::mutex config_mutex_;

    // Конфигурация
    ENIParser eni_parser;
    NetVarParser netvar_parser;
    NetworkVariablesConfig net_var;

    MasterConfig masterConfig;
    std::vector<SlaveConfig> slaveConfigs;
//    std::vector<uint8_t> m_ioMap;
    uint8_t m_ioMap[2048];
    int expectedWKC_ = 0;

    std::unordered_map<uint16_t, std::vector<PDO>> rxPdoCache;
    std::unordered_map<uint16_t, std::vector<PDO>> txPdoCache;

    // Callback-и
    ErrorCallback error_cb_;
    LogCallback log_cb_;

private:
    // Адаптивное управление циклом
    std::chrono::microseconds currentCycleTime_;
    std::chrono::microseconds minCycleTime_;
    std::chrono::microseconds maxCycleTime_;
    std::chrono::microseconds targetCycleTime_;
    double smoothingFactor_;
    std::chrono::steady_clock::time_point lastCycleStart_;
    std::chrono::microseconds measuredCycleTime_;
    std::chrono::microseconds sleepAdjustment_;
    bool adaptiveCycleEnabled_;
};

#endif // EC_MASTER_H

