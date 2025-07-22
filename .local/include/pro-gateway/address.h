#ifndef REGISTER_ADDRESS_H
#define REGISTER_ADDRESS_H

#include "variant.h"

template<typename T>
struct RegisterTraits { static_assert(sizeof(T) == 0, "Unsupported register type"); };

template<> struct RegisterTraits<bool>     { static constexpr size_t size = 1; };
template<> struct RegisterTraits<int8_t>   { static constexpr size_t size = 1; };
template<> struct RegisterTraits<uint8_t>  { static constexpr size_t size = 1; };
template<> struct RegisterTraits<int16_t>  { static constexpr size_t size = 2; };
template<> struct RegisterTraits<uint16_t> { static constexpr size_t size = 2; };
template<> struct RegisterTraits<int32_t>  { static constexpr size_t size = 4; };
template<> struct RegisterTraits<uint32_t> { static constexpr size_t size = 4; };
template<> struct RegisterTraits<float>    { static constexpr size_t size = 4; };
template<> struct RegisterTraits<int64_t>  { static constexpr size_t size = 8; };
template<> struct RegisterTraits<uint64_t> { static constexpr size_t size = 8; };
template<> struct RegisterTraits<double>   { static constexpr size_t size = 8; };

class Address {
private:
    // Все данные упакованы в 64 бита
    uint64_t value{};
    constexpr explicit Address(uint64_t packed) : value(packed) {}

    [[nodiscard]] constexpr uint64_t calculateOffset() const {
        switch(type()) {
            case TYPE_WORD:   return index() * 2;
            case TYPE_DWORD:
            case TYPE_REAL:   return index() * 4;
            case TYPE_LWORD:
            case TYPE_LREAL:  return index() * 8;
            default: return index(); // BIT и BYTE
        }
    }

    [[nodiscard]] constexpr size_t getSize() const {
        switch(type()) {
            case Address::TYPE_WORD:   return 2;    // 16-bit
            case Address::TYPE_DWORD:               // 32-bit
            case Address::TYPE_REAL:   return 4;    // float
            case Address::TYPE_LWORD:               // 64-bit
            case Address::TYPE_LREAL:  return 8;    // double
            default: return 1;    // 8-bit
        }
    }

public:
    // Категории регистров (4 бита)
    enum Category : uint8_t {
        INPUT   = 0,        // 'I' - входные регистры
        OUTPUT  = 1,        // 'Q' - выходные регистры
        MEMORY  = 2,        // 'M' - регистры памяти
        SPECIAL = 3         // 'S' - специальные регистры
    };

    // Типы данных (4 бита)
    enum Type : uint8_t {
        TYPE_BIT    = 0,    // 'X' - бит
        TYPE_BYTE   = 1,    // 'B' - байт
        TYPE_WORD   = 2,    // 'W' - слово
        TYPE_DWORD  = 3,    // 'D' - двойное слово
        TYPE_LWORD  = 4,    // 'L' - длинное слово
        TYPE_REAL   = 5,    // 'F' - вещественное (float)
        TYPE_LREAL  = 6     // 'E' - длинное вещественное (double)
    };

    // Битовые поля в 64-битном значении
    static constexpr uint64_t CATEGORY_SHIFT = 60;
    static constexpr uint64_t TYPE_SHIFT     = 56;
    static constexpr uint64_t BITPOS_SHIFT   = 48;

    static constexpr uint64_t CATEGORY_MASK  = 0xF000000000000000;
    static constexpr uint64_t TYPE_MASK      = 0x0F00000000000000;
    static constexpr uint64_t BITPOS_MASK    = 0x00FF000000000000;
    static constexpr uint64_t INDEX_MASK     = 0x0000FFFFFFFFFFFF;

    // Конструкторы
    constexpr Address() : value(0) {}
    constexpr Address(Category cat, Type type, uint64_t index, uint8_t bitpos = 0xFF)
            : value((static_cast<uint64_t>(cat)     << CATEGORY_SHIFT) |
                    (static_cast<uint64_t>(type)    << TYPE_SHIFT)     |
                    (static_cast<uint64_t>(bitpos)  << BITPOS_SHIFT)   |
                    (index & INDEX_MASK))
    {
        if (type == TYPE_BIT && bitpos > 7) {
            throw std::invalid_argument("Bit position must be 0-7");
        }
    }

    // Фабричные методы
    static Address of(const std::string& key);
    static constexpr Address of(uint64_t val) { return Address(val); }

    [[nodiscard]] std::string toString() const;

    // Преобразования
    constexpr operator uint64_t() const { return value; } //NOLINT
    [[nodiscard]] constexpr uint64_t get() const { return value; }

    // Методы доступа
    [[nodiscard]] constexpr Category category() const {
        return static_cast<Category>((value & CATEGORY_MASK) >> CATEGORY_SHIFT);
    }

    [[nodiscard]] constexpr Type type() const {
        return static_cast<Type>((value & TYPE_MASK) >> TYPE_SHIFT);
    }
    [[nodiscard]] constexpr size_t   size()   const { return getSize();  }
    [[nodiscard]] constexpr uint64_t index()  const { return value & INDEX_MASK; }
    [[nodiscard]] constexpr uint8_t  bitpos() const {
        return (type() == TYPE_BIT) ? ((value & BITPOS_MASK) >> BITPOS_SHIFT) : 0xFF;
    }

    [[nodiscard]] constexpr uint64_t offset() const { return calculateOffset();  }

    [[nodiscard]] constexpr bool isBit() const {
        return type() == TYPE_BIT && ((value & BITPOS_MASK) >> BITPOS_SHIFT) != 0xFF;
    }

    friend bool operator==(const Address& lhs, const Address& rhs) { return lhs.value == rhs.value; }

    template<typename T>
    static constexpr size_t alignment() { return RegisterTraits<T>::size; }

    [[nodiscard]] bool checkAlignment(Type type) const {
        return (offset() % alignmentFor(type)) == 0;
    }

    [[nodiscard]] static constexpr size_t alignmentFor(Type type) {
        switch(type) {
            case TYPE_WORD:   return 2;
            case TYPE_DWORD:
            case TYPE_REAL:   return 4;
            case TYPE_LWORD:
            case TYPE_LREAL:  return 8;
            default: return 1;
        }
    }

    [[nodiscard]] const char* getDataTypeName() const {
        switch(type()) {
            case TYPE_BIT:   return "BIT";
            case TYPE_BYTE:  return "BYTE";
            case TYPE_WORD:  return "WORD";
            case TYPE_DWORD: return "DWORD";
            case TYPE_LWORD: return "LWORD";
            case TYPE_REAL:  return "REAL";
            case TYPE_LREAL: return "LREAL";
            default: return "UNKNOWN";
        }
    }

//    template<DataType DT> struct DataTypeToNative;
};

//// Определяем специализации
//template<> struct Address::DataTypeToNative<Address::TYPE_BIT>    { using type = bool;     };
//template<> struct Address::DataTypeToNative<Address::TYPE_BYTE>   { using type = uint8_t;  };
//template<> struct Address::DataTypeToNative<Address::TYPE_WORD>   { using type = uint16_t; };
//template<> struct Address::DataTypeToNative<Address::TYPE_DWORD>  { using type = uint32_t; };
//template<> struct Address::DataTypeToNative<Address::TYPE_LWORD>  { using type = uint64_t; };
//template<> struct Address::DataTypeToNative<Address::TYPE_REAL>   { using type = float;    };
//template<> struct Address::DataTypeToNative<Address::TYPE_LREAL>  { using type = double;   };

// Хэш-функция
namespace std {
    template<>
    struct hash<Address> {
        size_t operator()(const Address& addr) const {
            return ((static_cast<size_t>(addr.category()) << 24) ^
                    (static_cast<size_t>(addr.type()) << 16) ^
                    (addr.index() << 8) ^ addr.bitpos());
        }
    };
}

#endif // REGISTER_ADDRESS_H