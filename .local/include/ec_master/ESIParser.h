#ifndef ESIPARSER_H
#define ESIPARSER_H

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <algorithm>

struct ESIError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct VendorInfo {
    uint32_t Id = 0;
    std::string Name;
};

struct DataType {
    struct SubItem {
        uint8_t SubIdx = 0;
        std::string Name;
        std::string Type;
        uint16_t BitSize = 0;
        uint16_t BitOffs = 0;
        struct Flags {
            std::string Access;
            std::string Category;
        } Flags;
        std::string Reference;  // Новая строка для ссылок
        bool IsPadding = false; // Флаг для заполнения
    };

    std::string Name;
    std::string BaseType;
    uint16_t BitSize = 0;
    std::vector<SubItem> SubItems;
    std::vector<std::pair<std::string, uint8_t>> EnumInfo;
};

struct Object {
    uint16_t Index = 0;
    std::string Name;
    std::string Type;
    uint16_t BitSize = 0;
    std::vector<uint8_t> DefaultData;
    struct Flags {
        std::string Access;
        std::string Category;
    } Flags;
    std::vector<Object> SubItems;
    std::string Reference; // Ссылки между объектами
    std::vector<std::pair<std::string, std::string>> InfoComments; // Комментарии
};

struct PdoEntry {
    uint16_t Index = 0;
    uint8_t SubIndex = 0;
    uint16_t BitLen = 0;
    std::string Name;
    std::string DataType;
};

struct Pdo {
    uint16_t Index = 0;
    std::string Name;
    bool Mandatory = false;
    bool Fixed = false;
    uint8_t sm_index = 0;
    std::vector<PdoEntry> Entries;
};

struct SyncManager {
    uint16_t MinSize = 0;
    uint16_t MaxSize = 0;
    uint16_t StartAddress = 0;
    uint8_t ControlByte = 0;
    uint16_t DefaultSize = 0;
    bool Enable = false;
    uint8_t Type;
};

struct Fmmu {
    std::string Type;
    uint16_t LogicalStartAddr = 0;
    uint16_t LogicalEndAddr = 0;
    uint8_t BitSize = 0;
    uint16_t PhysicalAddr = 0;
};

struct GroupDevice {
    std::string Name;
    std::string ProductCode;
    uint32_t VendorId = 0;
};

struct Group {
    std::string Name;
    std::string Type;
    std::vector<GroupDevice> Devices;
    std::unordered_map<std::string, std::string> CustomParams;
};

struct DeviceInfo {
    struct StateMachine {
        struct Timeout {
            uint32_t Preop = 0;
            uint32_t Safeop = 0;
            uint32_t BackToInit = 0;
            uint32_t BackToSafeop = 0;
        } Timeout;
    } StateMachine;

    struct Mailbox {
        struct Timeout {
            uint32_t Request = 0;
            uint32_t Response = 0;
        } Timeout;
        uint16_t MaxSize = 0;
    } Mailbox;

    uint32_t EepromSize;
};

struct EtherCATDevice {
    VendorInfo Vendor;
    std::string Type;
    uint32_t ProductCode;
    uint32_t RevisionNo;
    std::string Name;
    DeviceInfo Info;
    std::vector<DataType> DataTypes;
    std::vector<Object> Objects;
    std::vector<SyncManager> SyncManagers;
    std::vector<Fmmu> Fmmus;
    std::vector<Pdo> RxPdos;
    std::vector<Pdo> TxPdos;
    std::vector<Group> Groups;

    const DataType* findDataType(const std::string& name) const;
    const Object* findObject(uint16_t index) const;
};

class ESIParser {
public:
    ESIParser() = default;
    ~ESIParser() = default;

    EtherCATDevice parse(const std::string& filename);

private:
    void parseVendor(xmlNodePtr node);
    void parseDevice(xmlNodePtr node);
    void parseDataTypes(xmlNodePtr node);
    void parseObjects(xmlNodePtr node);
    void parseObject(xmlNodePtr node, Object& obj);
    void parsePdos(xmlNodePtr node, bool isRx);
    void parseSyncManagers(xmlNodePtr node);
    void parseFmmus(xmlNodePtr node);
    void parseDeviceInfo(xmlNodePtr node);
    void parseStateMachineInfo(xmlNodePtr node);
    void parseMailboxInfo(xmlNodePtr node);
    void parseGroups(xmlNodePtr node);
    void processObjectReferences();

    template<typename T>
    T getAttribute(xmlNodePtr node, const char* attr, const T& defaultValue = T());

    std::string getAttributeStr(xmlNodePtr node, const char* attr, const std::string& defaultValue = "");
    uint32_t parseHex(const std::string& hexStr);
    std::vector<uint8_t> parseHexData(const std::string& hexStr);

    EtherCATDevice m_device;
    std::unordered_map<std::string, const DataType*> m_dataTypeMap;
};

#endif // ESIPARSER_H
