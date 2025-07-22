#ifndef REG_SERVER_H
#define REG_SERVER_H

#include "registry.h"
#include "reg_client.h"

#include <memory>
#include <unordered_map>
#include <string>
#include <mutex>
#include <atomic>

constexpr unsigned int BUFFER_SIZE = 1024;

class IRegClient;
class RegClient;
class IClientCallback;

class RegServer {
private:
    // Центральное хранилище регистров
    Registry registry_;

    // Proxy-объекты для всех типов регистров
    Registry::IX ix_{registry_};
    Registry::QX qx_{registry_};
    Registry::MX mx_{registry_, 0, 2048};
    Registry::SX sx_{registry_};

    Registry::IB ib_{registry_};
    Registry::QB qb_{registry_};
    Registry::MB mb_{registry_, 0, 2048};
    Registry::SB sb_{registry_};

    Registry::IW iw_{registry_};
    Registry::QW qw_{registry_};
    Registry::MW mw_{registry_, 2048, 1024};
    Registry::SW sw_{registry_};

    Registry::ID id_{registry_};
    Registry::QD qd_{registry_};
    Registry::MD md_{registry_, 4096, 1024};
    Registry::SD sd_{registry_};

    Registry::IL il_{registry_};
    Registry::QL ql_{registry_};
    Registry::ML ml_{registry_, 8192, 1024};
    Registry::SL sl_{registry_};

    Registry::IF if_{registry_};
    Registry::QF qf_{registry_};
    Registry::MF mf_{registry_};
    Registry::SF sf_{registry_};

    Registry::IE ie_{registry_};
    Registry::QE qe_{registry_};
    Registry::ME me_{registry_};
    Registry::SE se_{registry_};

    // Клиенты
//    std::unordered_map<std::string, std::shared_ptr<RegClient>> clients_;
//    mutable std::mutex clients_mutex_;

    // Изменяем тип мьютекса на recursive_mutex
    mutable std::recursive_mutex clients_mutex_;
    std::unordered_map<std::string, std::weak_ptr<RegClient>> clients_; // Используем weak_ptr

    // Добавляем флаг завершения работы
    std::atomic<bool> shutdown_{false};

    // Приватный конструктор
    RegServer() : registry_(REGISTRY_SIZE) {}

public:
    // Singleton
    static RegServer& instance();

    // Удаление копирования
    RegServer(const RegServer&) = delete;
    RegServer& operator=(const RegServer&) = delete;

    // Явный деструктор для корректного завершения
    ~RegServer() { clearAllClients(); }

    // Методы доступа к proxy-объектам (неконстантные)
    Registry::IX& IX() { return ix_; }
    Registry::QX& QX() { return qx_; }
    Registry::MX& MX() { return mx_; }
    Registry::SX& SX() { return sx_; }

    Registry::IB& IB() { return ib_; }
    Registry::QB& QB() { return qb_; }
    Registry::MB& MB() { return mb_; }
    Registry::SB& SB() { return sb_; }

    Registry::IW& IW() { return iw_; }
    Registry::QW& QW() { return qw_; }
    Registry::MW& MW() { return mw_; }
    Registry::SW& SW() { return sw_; }

    Registry::ID& ID() { return id_; }
    Registry::QD& QD() { return qd_; }
    Registry::MD& MD() { return md_; }
    Registry::SD& SD() { return sd_; }

    Registry::IL& IL() { return il_; }
    Registry::QL& QL() { return ql_; }
    Registry::ML& ML() { return ml_; }
    Registry::SL& SL() { return sl_; }

    Registry::IF& IF() { return if_; }
    Registry::QF& QF() { return qf_; }
    Registry::MF& MF() { return mf_; }
    Registry::SF& SF() { return sf_; }

    Registry::IE& IE() { return ie_; }
    Registry::QE& QE() { return qe_; }
    Registry::ME& ME() { return me_; }
    Registry::SE& SE() { return se_; }

    // Константные версии методов доступа
    const Registry::IX& IX() const { return ix_; }
    const Registry::QX& QX() const { return qx_; }
    const Registry::MX& MX() const { return mx_; }
    const Registry::SX& SX() const { return sx_; }

    const Registry::IB& IB() const { return ib_; }
    const Registry::QB& QB() const { return qb_; }
    const Registry::MB& MB() const { return mb_; }
    const Registry::SB& SB() const { return sb_; }

    const Registry::IW& IW() const { return iw_; }
    const Registry::QW& QW() const { return qw_; }
    const Registry::MW& MW() const { return mw_; }
    const Registry::SW& SW() const { return sw_; }

    const Registry::ID& ID() const { return id_; }
    const Registry::QD& QD() const { return qd_; }
    const Registry::MD& MD() const { return md_; }
    const Registry::SD& SD() const { return sd_; }

    const Registry::IL& IL() const { return il_; }
    const Registry::QL& QL() const { return ql_; }
    const Registry::ML& ML() const { return ml_; }
    const Registry::SL& SL() const { return sl_; }

    const Registry::IF& IF() const { return if_; }
    const Registry::QF& QF() const { return qf_; }
    const Registry::MF& MF() const { return mf_; }
    const Registry::SF& SF() const { return sf_; }

    const Registry::IE& IE() const { return ie_; }
    const Registry::QE& QE() const { return qe_; }
    const Registry::ME& ME() const { return me_; }
    const Registry::SE& SE() const { return se_; }

    // Управление клиентами
    std::shared_ptr<IRegClient> createClient(const std::string& name, IClientCallback* callback);
    void removeClient(const std::string& name);
    [[maybe_unused]] void clearAllClients();

    // Функции-обёртки
    uint64_t getProxyValue(const Address& addr) const;
    void setProxyValue(const Address& addr, uint64_t value);
    bool isProxyChanged(const Address& addr) const;

    // Обновление всех зарегистрированных клиентов
    void notifyInit();
    void notifyExit();
    void notifyDataRead();
    void notifyDataWrite();

    // Фиксация изменений
    void commit();

private:
    // Вспомогательные методы для работы с категориями
    template<Registry::Category CAT> uint64_t getValueByCategory(const Address& addr) const;
    template<Registry::Category CAT> void setValueByCategory(const Address& addr, uint64_t value);
    template<Registry::Category CAT> bool isChangedByCategory(const Address& addr) const;
    template<Registry::Category CAT, typename T> auto& getProxy();
    template<Registry::Category CAT, typename T> const auto& getProxy() const;

    // Внутренние геттеры (не часть IRegClient)
    //IClientCallback* getCallback() const { return callback_; }
};

// Глобальные ссылки на proxy-объекты
inline Registry::IX& IX = RegServer::instance().IX();
inline Registry::QX& QX = RegServer::instance().QX();
inline Registry::MX& MX = RegServer::instance().MX();
inline Registry::SX& SX = RegServer::instance().SX();

inline Registry::IB& IB = RegServer::instance().IB();
inline Registry::QB& QB = RegServer::instance().QB();
inline Registry::MB& MB = RegServer::instance().MB();
inline Registry::SB& SB = RegServer::instance().SB();

inline Registry::IW& IW = RegServer::instance().IW();
inline Registry::QW& QW = RegServer::instance().QW();
inline Registry::MW& MW = RegServer::instance().MW();
inline Registry::SW& SW = RegServer::instance().SW();

inline Registry::ID& ID = RegServer::instance().ID();
inline Registry::QD& QD = RegServer::instance().QD();
inline Registry::MD& MD = RegServer::instance().MD();
inline Registry::SD& SD = RegServer::instance().SD();

inline Registry::IL& IL = RegServer::instance().IL();
inline Registry::QL& QL = RegServer::instance().QL();
inline Registry::ML& ML = RegServer::instance().ML();
inline Registry::SL& SL = RegServer::instance().SL();

inline Registry::IF& IF = RegServer::instance().IF();
inline Registry::QF& QF = RegServer::instance().QF();
inline Registry::MF& MF = RegServer::instance().MF();
inline Registry::SF& SF = RegServer::instance().SF();

inline Registry::IE& IE = RegServer::instance().IE();
inline Registry::QE& QE = RegServer::instance().QE();
inline Registry::ME& ME = RegServer::instance().ME();
inline Registry::SE& SE = RegServer::instance().SE();

#endif // REG_SERVER_H
