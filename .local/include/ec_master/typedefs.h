#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <vector>
#include <string>
#include <functional>
#include <any>
#include <memory>
#include <map>
#include <thread>
#include <atomic>
#include <variant>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "LibXml2/include/libxml/parser.h"
#include "LibXml2/include/libxml/tree.h"
#include "soem/ethercat.h"

#ifndef EC_SM_ACTIVE
#define EC_SM_ACTIVE 0x8000
#endif

#define EC_SM_INPUT     0x01    // Входной Sync Manager (данные от slave к master)
#define EC_SM_OUTPUT    0x02    // Выходной Sync Manager (данные от master к slave)
#define EC_SM_MBOX      0x04    // Почтовый ящик (Mailbox)
#define EC_SM_WATCHDOG  0x08    // Watchdog

#define EC_FMMU_INPUT   0x01    // Входной FMMU (для входных данных)
#define EC_FMMU_OUTPUT  0x02    // Выходной FMMU (для выходных данных)
#define EC_MAXFMMU      4       // Максимальное количество FMMU на slaveusing namespace std::chrono;

enum class ObjectType {
    BOOL        = 0x0001,
    INT8        = 0x0002,
    INT16       = 0x0003,
    INT32       = 0x0004,
    UINT8       = 0x0005,
    UINT16      = 0x0006,
    UINT32      = 0x0007,
    REAL32      = 0x0008,
    REAL64      = 0x0009,
    VISIBLE_STRING = 0x000A,
    OCTET_STRING  = 0x000B,
    DOMAIN       = 0x000F,
    ARRAY        = 0x1000,
    RECORD       = 0x1001,
    UNKNOWN      = 0xFFFF
};

// Коды ошибок
enum class ECErrorCode {
    Success = 0,
    FatalError,
    InvalidSlave,
    InvalidIOMap,
    InvalidParameter,
    DCConfigFailed,

    SDOReadFailed,
    SDOWriteFailed,

    RegisterReadFailed,
    RegisterWriteFailed,

    PDOOverflow,
    PDOEntryNotFound,
    PDOConfigFailed,

    SlaveConfigFailed,
    NoSlaves,

    DataTypeMismatch,
    NetworkInitFailed,

    StateTransitionTimeout,
    InvalidOperation,
    ThreadAlreadyRunning,
    PhysicalLayerError,

    ConfigLoadFailed,
    ConfigSaveFailed,

    CoEInitFailed,
    CoEObjectNotFound,
    CoEUploadFailed,
    CoEDownloadFailed,
    CoEEmergencyReceived,

    SyncManagerError,
    SyncManagerConfigFailed,

    FMMUError,
    FMMUConfigFailed,

    MailboxError,
    MailboxConfigFailed,
};

// Коды ошибок SOEM
enum class SOEMErrorCode {
    NO_ERROR = 0,
    INIT_FAILED,
    CONFIG_INIT_FAILED,
    STATE_CHANGE_FAILED,
    PROCESS_DATA_FAILED,
    SDO_READ_FAILED,
    SDO_WRITE_FAILED,
    REGISTER_READ_FAILED,
    REGISTER_WRITE_FAILED,
    FOE_OPERATION_FAILED,
    EOE_OPERATION_FAILED,
    DC_CONFIG_FAILED,
    MAILBOX_ERROR,
    WATCHDOG_ERROR,
    SLAVE_NOT_FOUND,
    INVALID_SLAVE_POSITION,
    CONFIG_MISMATCH,
    TIMEOUT,
    BUFFER_OVERFLOW,
    FRAME_ERROR,
    HARDWARE_ERROR,
    LINK_ERROR,
    PACKET_ERROR,
    FILE_ERROR,
    EoE_OPERATION_FAILED,
    UNKNOWN_ERROR
};

// Добавить новые структуры для CoE
struct CoEObjetEntry {
    uint16_t index;
    uint8_t subindex;
    std::string name;
    ObjectType type;
    uint32_t bitSize;
    std::string accessType; // "ro", "wo", "rw"
    std::string defaultValue;
};

struct CoEDictionary {
    std::vector<CoEObjetEntry> entries;
    std::map<uint16_t, std::string> objectNames;
};

// Типы для работы с Sync Manager и FMMU
struct SyncManagerConfig {
    uint8_t index;
    uint16_t startAddr;
    uint16_t length;
    uint32_t  flags;
    uint8_t type;
    /** SM type 0=unused 1=MbxWr 2=MbxRd 3=Outputs 4=Inputs */
    std::string typeStr() const {
        switch(type) {
        case 1: return "Mailbox Out";
        case 2: return "Mailbox In";
        case 3: return "Process Data Out";
        case 4: return "Process Data In";
        default: return "Unknown";
        }
    }
};

struct FMMUConfig {
    uint8_t index;
    uint32_t logicStart;
    uint16_t length;
    uint8_t logicStartBit;
    uint8_t logicEndBit;
    uint16_t physStart;
    uint8_t physStartBit;
    uint8_t type;
};

struct PDOEntry {
    uint16_t index;
    uint8_t subindex;
    int bitlen;
    int byte_offset;
    int bit_offset;
    std::string name;
    std::string type;
    std::string defaultValue;
    std::string unit;
    std::string description;
    std::string access;
    std::string mem_link;
    uint16_t reg_index;

    size_t getByteSize() const {
        return (bitlen + 7) / 8; // Округление вверх до целых байт
    }

    template <typename T>
    bool validateTypeSize() const {
        const size_t expectedBits = sizeof(T) * 8;
        return (bitlen == expectedBits);
    }
};

struct PDO {
    uint16_t index;
    uint16_t config_index;
    std::string name;
    bool enabled;
    bool mandatory;
    bool fixed;
    uint8_t sm_index;
    uint32_t offset;
    uint32_t entery_offset;
    std::vector<PDOEntry> entries;
};

// Структуры для хранения конфигурации
struct SlaveConfig {
    struct Info {
        std::string name;
        std::string vendor;
        uint32_t vendorId;
        uint32_t productCode;
        uint32_t revision;
        std::string serialNumber;
        std::string type;
        int channels;
        int resolution;
        int samplingRate;
        std::string description;
        std::string firmwareVersion;
        std::string bootloaderVersion;
        std::string hardwareVersion;
    };

    struct SYNCManager {
        int index;
        std::string type;
        uint32_t start;
        uint32_t size;
        std::string description;
        bool watchdog;
        int timeout;
    };

    struct InitCommand {
        uint16_t index;
        uint8_t subindex;
        int size;
        std::string value;
        std::string description;
        bool errorStop;
        int nextLine;
        int retries;
    };

    struct DistributedClock {
        bool enabled;
        int offset;
        int cycleTime;
        int shiftTime;
        std::string syncMode;
        bool activate;
        double pllBandwidth;
    };

    struct Mailbox {
        bool enabled;
        struct {
            bool CoE;
            bool FoE;
            bool EoE;
            bool SoE;
            std::string CoEVersion;
            bool Default;
        } protocols;
        struct {
            uint32_t receiveOffset;
            uint32_t receiveSize;
            uint32_t sendOffset;
            uint32_t sendSize;
        } buffer;
        struct {
            int request;
            int response;
            int mailbox;
        } timeout;
        int maxQueueSize;
        struct {
            bool EoE;
            std::string EoEIP;
            std::string EoEport;
            std::string EoENetmask;
            std::string EoEGateway;
        } eoeSettings;
    };

    struct Fmmu {
        uint32_t logicalAddress;
        uint32_t length;
        int type;
    };

    struct ErrorHandling {
        bool autoRecovery;
        int recoveryAttempts;
        int recoveryTimeout;
        std::string criticalAction;
        std::string errorAction;
        std::string warningAction;
    };

    Info info;
    int address;
    DistributedClock dc;
    Mailbox mailbox;
    std::vector<InitCommand> initCommands;
    std::vector<SYNCManager> syncManagers;
    std::vector<PDO> rxPdos;
    std::vector<PDO> txPdos;
    std::vector<Fmmu> fmmus;
    ErrorHandling errorHandling;

    std::vector<SyncManagerConfig> syncManagersCfg;
    std::vector<FMMUConfig> fmmuConfigsCfg;
};

struct MasterConfig {
    struct Info {
        std::string name;
        std::string vendor;
        std::string serialNumber;
        std::string description;
        std::string version;
        std::string buildDate;
    };

    struct NetworkSettings {
        std::string primaryInterface;
        std::string secondaryInterface;
        std::string primaryMAC;
        std::string secondaryMAC;
        std::string primaryIP;
        std::string secondaryIP;
        std::string subnetMask;
        std::string gateway;
        struct {
            int frame;
            int processData;
            int mailbox;
            int watchdog;
            int eoe;
        } timeouts;
        bool jumboFrames;
        int maxFrameSize;
    };

    struct DistributedClocks {
        bool enabled;
        int cycleTime;
        int shiftTime;
        int syncWindow;
        struct {
            bool enabled;
            int maxDrift;
            int compensationInterval;
            double compensationFactor;
        } driftCompensation;
        struct {
            bool enabled;
            int divider;
            int timeout;
            std::string action;
        } watchdog;
        std::vector<std::pair<std::string, bool>> syncModes;
    };

    struct ProcessDataSettings {
        bool lrwEnabled;
        bool taskMessages;
        bool processDataWatchdog;
        int maxRetries;
        int retryInterval;
        int maxFrameSize;
    };

    struct Redundancy {
        bool enabled;
        std::string switchoverMode;
        int switchoverTime;
        int monitoringInterval;
        bool linkDetection;
        bool hotStandby;
    };

    Info info;
    NetworkSettings network;
    DistributedClocks dc;
    ProcessDataSettings processData;
    Redundancy redundancy;
};

struct PDOData {
    uint16_t index;         // Индекс объекта (например, 0x6040)
    uint8_t subindex;       // Суб-индекс объекта
    std::string name;       // Имя переменной (из ESI или конфигурации)
    std::variant<bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float32, float64>
                value;      // Значение (тип зависит от данных)
    std::string type;       // Тип данных (например, "int16", "uint32")
    size_t size;            // Размер в байтах
    std::string mem_link;
};

// Типы callback-функций
using ErrorCallback = std::function<void(int slave, ECErrorCode code, const std::string& msg)>;
using LogCallback = std::function<void(const std::string& msg)>;
using DataCallback = std::function<void(int slave, const PDOData& data)>;

// Типы событий для автоматических реакций
enum class EventType {
    ConnectionLost,
    ConnectionRestored,
    SlaveStateChanged,
    HighErrorRate,
    CycleTimeExceeded,
    EmergencyMessageReceived,
    DCSyncLost,
    PDOOverflow,
    SDOTimeout
};

// Структура описания реакции
struct AutoReaction {
    std::function<void()> action;
    bool enabled;
    std::chrono::milliseconds minInterval; // Минимальный интервал между срабатываниями
    std::chrono::system_clock::time_point lastTriggered;
};

// Состояние соединения
enum class ConnectionState {
    Disconnected,
    Connecting,
    Connected,
    Error,
    Reconnecting
};

// Статистика сети
struct NetworkStatistics {
    uint64_t total_cycles;
    uint64_t success_cycles;
    uint64_t error_count;
    uint64_t wkc_errors;
    uint64_t sdo_operations;
    uint64_t pdo_operations;
    uint64_t frame_errors;
    uint64_t lost_frames;
    std::chrono::microseconds max_cycle_time;
    std::chrono::microseconds min_cycle_time;
    std::chrono::microseconds avg_cycle_time;
    uint64_t dc_sync_errors;
    uint64_t slave_state_changes;
};

// Состояние slave-устройства
struct SlaveState {
    uint16_t position;
    uint16_t state;
    uint32_t al_status;
    uint64_t rx_bytes;
    uint64_t tx_bytes;
    uint32_t error_count;
    bool operational;
    bool dc_synced;
    std::chrono::microseconds last_response_time;
};

// Callback для мониторинга состояния
using StateCallback = std::function<void(ConnectionState state)>;
using StatisticsCallback = std::function<void(const NetworkStatistics& stats)>;
using SlaveStateCallback = std::function<void(uint16_t slave, const SlaveState& state)>;
using WarningCallback = std::function<void(const std::string& warning_message)>;
using EmergencyCallback = std::function<void(uint16_t slave, uint16_t error_code, const std::string& emergency_message)>;


// Тип для протоколов Mailbox
struct MailboxProtocols {
    bool coe = true;    // CANopen over EtherCAT (обязательный для большинства устройств)
    bool foe = false;   // File Access over EtherCAT
    bool eoe = false;   // Ethernet over EtherCAT
    bool soe = false;   // Servo Drive over EtherCAT
    std::string coeVersion = "default"; // Версия CoE (например, "CiA402" для драйверов)
};

// Параметры буферов Mailbox
struct MailboxBuffers {
    uint32_t receiveOffset = 0x1000;  // Адрес буфера приема
    uint32_t receiveSize = 128;       // Размер буфера приема (байт)
    uint32_t sendOffset = 0x1100;     // Адрес буфера передачи
    uint32_t sendSize = 128;          // Размер буфера передачи (байт)
};

// Таймауты Mailbox
struct MailboxTimeouts {
    uint32_t request = 2000;    // Таймаут запроса (мс)
    uint32_t response = 2000;   // Таймаут ответа (мс)
    uint32_t mailbox = 5000;    // Общий таймаут Mailbox (мс)
};

// Статус Mailbox
struct MailboxStatus {
    bool valid = false;         // Валидны ли данные
    bool enabled = false;       // Включен ли Mailbox
    uint32_t lastActivity = 0;  // Время последней активности (мс)
    uint16_t sendBufferSize = 0;// Размер буфера отправки
    uint16_t receiveBufferSize = 0; // Размер буфера приема
    uint16_t errorCount = 0;    // Счетчик ошибок
};

// Полная конфигурация Mailbox
struct MailboxConfig {
    bool enabled = true;                // Включить/выключить Mailbox
    MailboxProtocols protocols;         // Настройки протоколов
    MailboxBuffers buffers;             // Параметры буферов
    MailboxTimeouts timeouts;           // Таймауты
    uint16_t maxQueueSize = 10;         // Максимальный размер очереди сообщений
    bool autoProcess = true;            // Автоматическая обработка входящих сообщений
};

// Перечисление протоколов Mailbox
enum class MailboxProtocol {
    CoE,    // CANopen over EtherCAT
    FoE,    // File Access over EtherCAT
    EoE,    // Ethernet over EtherCAT
    SoE     // Servo Drive over EtherCAT
};

#endif // TYPEDEFS_H
