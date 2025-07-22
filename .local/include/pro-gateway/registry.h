//-----------------------------------------------------------------------------
// Copyright © 2016-2025 AMBITECS <info@ambi.biz>
//-----------------------------------------------------------------------------
#ifndef REGISTRY_H
#define REGISTRY_H

#include "address.h"

#include <stdexcept>
#include <cstring>
#include <vector>
#include <unordered_map>

constexpr unsigned int   REGISTRY_SIZE = 65536;   // Размер категорий регистров

//-----------------------------------------------------------------------------
// Registry (улучшенная версия)
//-----------------------------------------------------------------------------
class Registry {
public:
    using Storage  = std::vector<uint8_t>;  // тип хранилища реестра
    using Category = Address::Category;     // тип категории реестра
    using DataType = Address::Type;         // тип данных в реестре

private:
    // Регистры контроллера по категориям IEC
    struct RegStorage {
        Storage A;  // A - текущий слой
        Storage B;  // B - предыдущий слой
    };

    using  CategoriesMap  = std::unordered_map<Category, RegStorage>;
    CategoriesMap  categories_;

    //const Storage&    getStorage(Category cat) const { return categories_.at(cat).A; }

public:
    // Внутренний метод для получения хранилища по категориям IEC
    Storage& getStorage(Category cat) { return categories_[cat].A; }
    const RegStorage& getRegStorage(Category cat) const { return categories_.at(cat); }

public:
    explicit Registry(size_t memory_size = REGISTRY_SIZE) {
        // Инициализируем все категории по умолчанию
        categories_[Category::INPUT]   = RegStorage{};
        categories_[Category::OUTPUT]  = RegStorage{};
        categories_[Category::MEMORY]  = RegStorage{};
        categories_[Category::SPECIAL] = RegStorage{};
        resize(memory_size);
    }

    void resize(size_t new_size) {
        for (auto& [cat, reg] : categories_) {
            reg.A.resize(new_size);
            reg.B.resize(new_size);
        }
    }

    // Синхронизация слоёв (A -> B)
    void commit() {
        for (auto& [cat, reg] : categories_) {
            reg.B = reg.A;
        }
    }

    //-------------------------------------------------------------------------
    // Accessor - шаблон доступа к типу T в категории CAT по смещению offset_
    //-------------------------------------------------------------------------
    template<typename T, Category CAT>
    class Accessor
    {
    private:
        Registry &parent_;
        uint64_t offset_;

        Storage &                       getStorage()        { return parent_.categories_[CAT].A; }
        [[nodiscard]] const Storage &   getStorage() const  { return parent_.categories_[CAT].A; }

        void checkBounds(size_t additional = 0) const {
            if (offset_ + additional + RegisterTraits<T>::size > getStorage().size()) {
                throw std::out_of_range("Register access out of range");
            }
        }

    public:
        using value_type = T;   // Тип данных

        Accessor(Registry &parent, uint64_t offset) : parent_(parent), offset_(offset) {
            // Для битовых типов не проверяем выравнивание
            if constexpr (!std::is_same_v<T, bool>) {
                if (offset_ % RegisterTraits<T>::size != 0) {
                    throw std::runtime_error("Unaligned register access");
                }
            }
        }

        // Явное приведение типов (извлечение значения)
        [[nodiscard]] T get() const { return static_cast<T>(*this); }

        // Неявное приведение типов (извлечение значения)
        operator T() const { //NOLINT - неявное преобразование типа
            checkBounds();
            if constexpr (sizeof(T) == 1) {
                return getStorage()[offset_];
            } else {
                // (1) *reinterpret_cast<const T*>(&storage[offset_]);
                // - небезопасно, нарушает strict aliasing rules
                // (2) std::memcpy(&value, &getStorage()[offset_], sizeof(T));
                // - решает вопросы с выравниванием, оптимизуруется компилятором
                T value;
                std::memcpy(&value, &getStorage()[offset_], sizeof(T));
                return value;
            }
        }

        // Запись с защитой от aliasing (запись значения)
        Accessor &operator=(T value) {
            checkBounds();
            std::memcpy(&getStorage()[offset_], &value, sizeof(T));
            return *this;
        }

        Accessor& operator=(const Accessor& other) {
            if (this != &other) {
                parent_ = other.parent_;
                offset_ = other.offset_;
            }
            return *this;
        }

        Accessor& operator&=(T value) {
            checkBounds();
            T current;
            std::memcpy(&current, &getStorage()[offset_], sizeof(T));
            current &= value;
            std::memcpy(&getStorage()[offset_], &current, sizeof(T));
            return *this;
        }

        Accessor& operator|=(T value) {
            checkBounds();
            T current;
            std::memcpy(&current, &getStorage()[offset_], sizeof(T));
            current |= value;
            std::memcpy(&getStorage()[offset_], &current, sizeof(T));
            return *this;
        }

        Accessor& operator^=(T value) {
            checkBounds();
            T current;
            std::memcpy(&current, &getStorage()[offset_], sizeof(T));
            current ^= value;
            std::memcpy(&getStorage()[offset_], &current, sizeof(T));
            return *this;
        }

        Accessor& operator+=(T value) {
            checkBounds();
            T current;
            std::memcpy(&current, &getStorage()[offset_], sizeof(T));
            current += value;
            std::memcpy(&getStorage()[offset_], &current, sizeof(T));
            return *this;
        }

        Accessor& operator-=(T value) {
            checkBounds();
            T current;
            std::memcpy(&current, &getStorage()[offset_], sizeof(T));
            current -= value;
            std::memcpy(&getStorage()[offset_], &current, sizeof(T));
            return *this;
        }

        Accessor& operator&=(const Accessor& other) {
            return *this &= static_cast<T>(other);
        }

        Accessor& operator|=(const Accessor& other) {
            return *this |= static_cast<T>(other);
        }

        //---------------------------------------------------------------------
        // BitReference - побитовый доступ через Accessor
        //---------------------------------------------------------------------
        class BitReference {
            Accessor &accessor_;
            uint8_t bit_pos_;

        public:
            BitReference(Accessor &accessor, uint8_t bit_pos): accessor_(accessor), bit_pos_(bit_pos)
            {
                if (bit_pos_ >= sizeof(T) * 8) {
                    throw std::out_of_range("Bit position out of range");
                }
            }

            // Неявное приведение к bool (чтение бита)
            operator bool() const { //NOLINT - неявное преобразование типа
                return (static_cast<T>(accessor_) >> bit_pos_) & 0x01;
            }

            // Безопасная запись бита (установка или сброс)
            BitReference &operator=(bool b) {
                T value = static_cast<T>(accessor_);
                T mask = static_cast<T>(1) << bit_pos_;
                accessor_ = b ? (value | mask) : (value & ~mask);
                return *this;
            }

            BitReference& operator&=(bool b) {
                bool current = static_cast<bool>(*this);
                *this = current & b;
                return *this;
            }

            BitReference& operator|=(bool b) {
                bool current = static_cast<bool>(*this);
                *this = current | b;
                return *this;
            }

            BitReference& operator^=(bool b) {
                bool current = static_cast<bool>(*this);
                *this = current ^ b;
                return *this;
            }

        };

        BitReference operator[](uint8_t bit_pos) {
            static_assert(std::is_integral_v<T>, "Bit access only available for integral types");
            return BitReference(*this, bit_pos);
        }

        //---------------------------------------------------------------------
        // IndexProxy - выдача Accessor через operator[]
        //---------------------------------------------------------------------
        class IndexProxy {
        private:
            Registry& parent_;
            uint64_t  base_offset_; // Базовое смещение
            uint64_t  count_;       // Количество элементов

            // Вычисляет максимальное количество элементов
            uint64_t calculateCount(Registry& parent, uint64_t offset) const {
                const auto& storage = parent.getStorage(CAT);
                if (storage.size() <= offset) return 0;
                return (storage.size() - offset) / RegisterTraits<T>::size;
            }

        public:
            using value_type = T;   // Тип данных

            explicit IndexProxy(Registry& parent, uint64_t base_offset = 0, uint64_t count = 0)
                    : parent_(parent),
                      base_offset_(base_offset),
                      count_(count == 0 ? calculateCount(parent, base_offset) : count)
            {
                // Дополнительная проверка, если count задан явно
                if (count != 0) {
                    const auto& storage = parent.getStorage(CAT);
                    uint64_t required_size = base_offset_ + count * RegisterTraits<T>::size;
                    if (required_size > storage.size()) {
                        throw std::out_of_range(
                                "Requested count " + std::to_string(count) +
                                " exceeds available storage for type size " +
                                std::to_string(RegisterTraits<T>::size)
                        );
                    }
                }
            }

            [[nodiscard]] bool isChanged(size_t index) const {
                const auto& storage = parent_.getRegStorage(CAT);
                const size_t offset = base_offset_ + index * sizeof(T);
                return memcmp( &storage.A[offset], &storage.B[offset], sizeof(T)) != 0;
            }

        public:

            // Константная версия operator[]
            Accessor operator[](uint64_t index) const {
                if (index >= count_) {
                    throw std::out_of_range("Register index out of range");
                }
                uint64_t offset = base_offset_ + index * RegisterTraits<T>::size;
                return Accessor(parent_, offset);
            }

            // Итератор для range-based for
            class Iterator {
                Registry* parent_;
                uint64_t base_offset_;
                uint64_t index_;
                uint64_t count_;

                // (1) Храним кэшированный Accessor как член класса
                // (для конструкций типа for (auto & i : MW) checksum ^= i;)
                //Accessor current_accessor_;

            public:
                Iterator(Registry* parent, uint64_t offset, uint64_t index, uint64_t count):
                    parent_(parent),
                    base_offset_(offset),
                    index_(index),
                    count_(count)
                    //,current_accessor_(*parent, offset + index * RegisterTraits<T>::size)
                    {}

                // (2) Accessor operator*() {
                //    return Accessor(*parent_, base_offset_ + index_ * RegisterTraits<T>::size);
                //}

                // (3) Accessor& operator*() {
                //    // Обновляем позицию current_accessor_
                //    current_accessor_ = Accessor(*parent_, base_offset_ + index_ * RegisterTraits<T>::size);
                //    return current_accessor_;
                //}

                Accessor operator*() const {
                    return Accessor(*parent_, base_offset_ + index_ * RegisterTraits<T>::size);
                }

                Iterator& operator++() { ++index_; return *this; }
                bool operator!=(const Iterator& other) const { return index_ != other.index_; }
            };

            Iterator begin() { return Iterator(&parent_, base_offset_, 0, count_); }
            Iterator end()   { return Iterator(&parent_, base_offset_, count_, count_); }

            Accessor operator[](uint64_t index) {
                if (index >= count_) {
                    throw std::out_of_range("Register index out of range");
                }
                uint64_t offset = base_offset_ + index * RegisterTraits<T>::size;
                return Accessor(parent_, offset);
            }

            [[nodiscard]] uint64_t size() const { return count_; } // Добавляем метод size()
        };

    }; // Accessor

    template<typename T, Category CAT>
    Accessor<T, CAT> get(uint64_t offset) {
        //static_assert(std::is_arithmetic_v<T>, "Only arithmetic types are supported");
        return Accessor<T, CAT>(*this, offset);
    }

    // Базовые аксессоры по категориям
    template<typename T> using IRegister = Accessor<T, Category::INPUT>;
    template<typename T> using QRegister = Accessor<T, Category::OUTPUT>;
    template<typename T> using MRegister = Accessor<T, Category::MEMORY>;
    template<typename T> using SRegister = Accessor<T, Category::SPECIAL>;

    // Proxy-типы для входных регистров (I)
    using IX = IRegister<T_BOOL>  ::IndexProxy;
    using IB = IRegister<T_UINT8> ::IndexProxy;
    using IW = IRegister<T_UINT16>::IndexProxy;
    using ID = IRegister<T_UINT32>::IndexProxy;
    using IL = IRegister<T_UINT64>::IndexProxy;
    using IF = IRegister<T_REAL32>::IndexProxy;  // для float  (F)
    using IE = IRegister<T_REAL64>::IndexProxy;  // для double (E)

    // Proxy-типы для выходных регистров (Q)
    using QX = QRegister<T_BOOL>  ::IndexProxy;
    using QB = QRegister<T_UINT8> ::IndexProxy;
    using QW = QRegister<T_UINT16>::IndexProxy;
    using QD = QRegister<T_UINT32>::IndexProxy;
    using QL = QRegister<T_UINT64>::IndexProxy;
    using QF = QRegister<T_REAL32>::IndexProxy;
    using QE = QRegister<T_REAL64>::IndexProxy;

    // Proxy-типы для регистров памяти (M)
    using MX = MRegister<T_BOOL>  ::IndexProxy;
    using MB = MRegister<T_UINT8> ::IndexProxy;
    using MW = MRegister<T_UINT16>::IndexProxy;
    using MD = MRegister<T_UINT32>::IndexProxy;
    using ML = MRegister<T_UINT64>::IndexProxy;
    using MF = MRegister<T_REAL32>::IndexProxy;
    using ME = MRegister<T_REAL64>::IndexProxy;

    // Proxy-типы для специальных регистров (S)
    using SX = SRegister<T_BOOL>  ::IndexProxy;
    using SB = SRegister<T_UINT8> ::IndexProxy;
    using SW = SRegister<T_UINT16>::IndexProxy;
    using SD = SRegister<T_UINT32>::IndexProxy;
    using SL = SRegister<T_UINT64>::IndexProxy;
    using SF = SRegister<T_REAL32>::IndexProxy;
    using SE = SRegister<T_REAL64>::IndexProxy;
};

#endif // REGISTRY_H
