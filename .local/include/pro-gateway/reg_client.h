#ifndef REG_CLIENT_H
#define REG_CLIENT_H

#include "interfaces.h"

class RegServer;

#include <vector>
#include <functional>
#include <atomic>

class RegClient : public IRegClient
{
private:
    std::string     name_;
    IClientCallback *callback_;
    std::unordered_map<Address, uint64_t> subscriptions_; // addr -> key

    std::atomic<bool> active_{true};
    RegServer& server_;

public:
    explicit RegClient(std::string name, IClientCallback* callback);
    ~RegClient() override;

    void shutdown();

    const std::string& getName() const { return name_; }

    // Одиночные операции
    uint64_t read(const Address& addr) override;
    void write(const Address& addr, uint64_t value) override;

    // Массовые операции
    void read(std::vector<ItemData>& items) override;
    void write(const std::vector<ItemData>& items) override;

    // Подписка
    void subscribe(const std::vector<RegItem>& items) override;
    void unsubscribe(const std::vector<Address>& addresses) override;
    void unsubscribeAll() override;

    // Обновление по изменениям
    void update();

    // Внутренние геттеры (не часть IRegClient)
    IClientCallback* getCallback() const { return callback_; }
};

#endif // REG_CLIENT_H