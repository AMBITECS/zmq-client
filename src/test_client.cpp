#include "request_manager.h"

#include <iostream>
#include <zmq.hpp>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <limits>
#include <algorithm>
#include "crc_utils.h"

namespace fs = std::filesystem;
using json = nlohmann::json;
using namespace std::chrono_literals;

#ifdef _WIN32
    #include <conio.h>  // Для _kbhit() и _getch()
    #include <windows.h>
    #include <condition_variable>
#else
    #include <sys/select.h>
    #include <termios.h>
    #include <unistd.h>
    #include <cstdlib>
    struct termios original_termios;
#endif
#include <iostream>
#include <string>

#ifdef _WIN32
int win32_peek_input() {
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD inputRecord;
    DWORD eventsRead;

    PeekConsoleInput(hInput, &inputRecord, 1, &eventsRead);
    if (eventsRead == 0)
        return -1;

    if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown) {
        return inputRecord.Event.KeyEvent.uChar.AsciiChar;
    }
    return -1;
}

void clear_screen() {
    static HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD count;
    DWORD cellCount;
    COORD homeCoords = {0, 0};

    if (hStdOut == INVALID_HANDLE_VALUE) return;

    if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
    cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    if (!FillConsoleOutputCharacter(hStdOut, ' ', cellCount
                                    , homeCoords, &count)) return;
    if (!FillConsoleOutputAttribute(hStdOut, csbi.wAttributes,
                                    cellCount, homeCoords, &count)) return;
    SetConsoleCursorPosition(hStdOut, homeCoords);

}

void enable_ansi_colors() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#else
void clear_screen() {
    std::system("clear");
    //std::cout << "\033[2J\033[H";
}
void enable_ansi_colors() {}
#endif

void enable_nonblocking_input() {
#ifdef _WIN32
    // В Windows ничего дополнительно настраивать не нужно
#else
    struct termios ttystate;
    tcgetattr(STDIN_FILENO, &ttystate);
    ttystate.c_lflag &= ~ICANON;  // Отключаем канонический режим
    ttystate.c_lflag &= ~ECHO;     // Отключаем эхо
    ttystate.c_cc[VMIN] = 0;       // Минимальное количество символов
    ttystate.c_cc[VTIME] = 0;      // Время ожидания
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
#endif
}

void set_nonblocking_terminal() {
#ifdef _WIN32
#else
    struct termios new_termios = original_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    new_termios.c_cc[VMIN] = 0;
    new_termios.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
#endif
}


int get_nonblocking_input() {
#ifdef _WIN32
    // Пробуем оба метода для надежности
    if (_kbhit()) {
        return _getch();
    }
    return win32_peek_input();
#else
    struct timeval tv = {0, 0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0) {
        unsigned char ch;
        if (read(STDIN_FILENO, &ch, 1) == 1) {
            return ch;
        }
    }
    return -1;
#endif
}

void init_console() {
#ifdef _WIN32
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);
    SetConsoleMode(hStdin, mode & (~ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT));
#endif
}

//void restore_terminal() {
//#ifdef _WIN32
//    // В Windows ничего дополнительно настраивать не нужно
//#else
//    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios_);
//#endif
//}

bool wait_for_input(std::chrono::milliseconds timeout) {
#ifdef _WIN32
    // Реализация для Windows
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD event_count = 0;
    GetNumberOfConsoleInputEvents(hStdin, &event_count);
    if (event_count > 0) return true;

    DWORD wait_result = WaitForSingleObject(hStdin, timeout.count());
    return (wait_result == WAIT_OBJECT_0);
#else
    // Реализация для Unix
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    timeval tv = {
        static_cast<long>(timeout.count() / 1000),
        static_cast<long>((timeout.count() % 1000) * 1000)
    };

    return select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) > 0;
#endif
}

/**
 * @class TestClient
 * @brief ZMQ клиент для тестирования взаимодействия с сервером ProContEx
 */
class TestClient {
    zmq::context_t ctx_;                      // ZMQ контекст
    zmq::socket_t adm_socket_;                // Сокет для административных команд
    zmq::socket_t sub_socket_;                // Сокет для подписки на данные
    std::string client_id_;                   // Идентификатор клиента
    std::string server_host_;                 // Адрес сервера
    std::thread listen_thread_;               // Поток для прослушивания сообщений
    std::thread heartbeat_thread_;            // Поток отправки heartbeat
    std::thread connection_monitor_thread_;   // Поток мониторинга соединения

    std::atomic<bool> sockets_ready_{false};  // Флаг готовности сокетов
    std::atomic<bool> running_{false};        // Флаг работы клиента
    std::atomic<bool> connection_ok_{false};  // Флаг состояния соединения
    std::atomic<bool> trying_to_connect_{false}; // Флаг попытки переподключения
    std::atomic<bool> heartbeat_active_{false}; // Флаг активности heartbeat

    bool debug_mode_{false};                   // Режим отладки

    std::mutex updates_mutex_;               // Мьютекс для доступа к обновлениям
    std::vector<Tag> last_updates_;          // Последние полученные обновления тегов

    // Для синхронизации heartbeat
    std::condition_variable heartbeat_received_{};
    std::mutex heartbeat_mutex_;    // Мьютекс для доступа к last_heartbeat_time_
    std::atomic<bool> heartbeat_expected_{false};
    std::chrono::steady_clock::time_point last_heartbeat_time_; // Время последнего heartbeat

    std::mutex console_mutex_;      // Мьютекс для доступа к меню
    std::mutex connection_mutex_;   // Мьютекс для доступа к операции подключения

    RequestManager request_manager_{};

    std::mutex send_mutex_{};

    enum class RequestMode {
        Async,  // Асинхронная отправка (по умолчанию)
        Sync    // Синхронный запрос-ответ
    };

public:
    /**
     * @brief Конструктор клиента
     * @param id Идентификатор клиента
     * @param server_host Адрес сервера
     */
    TestClient(std::string id, std::string server_host)
            : ctx_(2),
              adm_socket_(ctx_, zmq::socket_type::dealer),
              sub_socket_(ctx_, zmq::socket_type::sub),
              client_id_(std::move(id)),
              server_host_(std::move(server_host)),
              last_heartbeat_time_(std::chrono::steady_clock::now()),
              last_updates_()  // Инициализация пустым вектором
    {
        adm_socket_.set(zmq::sockopt::routing_id, client_id_);
        sub_socket_.set(zmq::sockopt::subscribe, "");
    }

    ~TestClient() {
        stop();
    }

    /**
     * @brief Запуск клиента
     */
    void start() {
        running_ = true;

        // Запускаем основные потоки
        connection_monitor_thread_ = std::thread(&TestClient::connection_and_heartbeat_loop, this);
        listen_thread_ = std::thread(&TestClient::listen_loop, this);


        // Первая попытка подключения
        if (connect()) {
            connection_ok_ = true;
        } else {
            if (debug_mode_) {
                std::cerr << "Initial connection failed, will keep trying...\n";
            }

        }
    }

    /**
     * @brief Остановка клиента
     */
    void stop() {
        // 1. Флаг остановки
        running_ = false;

        // 2. Остановка heartbeat (самый "тихий" поток)
        if (heartbeat_thread_.joinable()) {
            heartbeat_active_ = false;
            heartbeat_thread_.join(); // Ожидаем завершения
        }

        // 3. Остановка монитора подключения
        if (connection_monitor_thread_.joinable()) {
            connection_monitor_thread_.join();
        }

        // 4. Принудительная разблокировка listen_loop
        if (listen_thread_.joinable()) {
            // Отправляем пустое сообщение для разблокировки zmq::poll
            zmq::message_t wakeup_msg(0);
            adm_socket_.send(wakeup_msg, zmq::send_flags::dontwait);
            listen_thread_.join();
        }

        // 5. Закрытие сокетов
        cleanup_resources();

        // 7. Закрытие контекста (через деструктор)
    }

    /**
     * @brief Основной цикл меню
     */
    void runMenu() {
        while (running_) {
            printMenu();

            int choice = -1;
            std::cin >> choice;  // Блокирующий ввод (ждёт пользователя)

            if (!std::cin) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            clear_screen();
            if (choice == 0) {
                running_ = false;
            } else {
                debug_mode_ = true;
                handleMenuChoice(choice);
                debug_mode_ = false;

                // Пауза перед возвратом в меню
                std::cout << "\nPress Enter to return to menu...";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cin.get();
            }
        }
    }

//    void runMenu() {
//        while (running_) {
//            printMenu();
//
//            // Ожидаем ввод с таймаутом для проверки соединения
//            if (wait_for_input(100ms)) {
//                int choice;
//                if (std::cin >> choice) {
//                    clear_screen();
//
//                    if (choice == 0) {
//                        running_ = false;
//                    } else {
//                        debug_mode_ = true;
//
//                        handleMenuChoice(choice);
//
//                        // Пауза перед возвратом в меню
//                        std::cout << "\nPress Enter to return to menu...";
//                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
//                        std::cin.get();
//
//                        debug_mode_ = false;
//
//                    }
//                } else {
//                    std::cin.clear();
//                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
//                }
//            }
//        }
//    }

private:
    /* Вспомогательные методы */
    /**
     * @brief Отправка сообщения
     * @param socket Сокет для отправки
     * @param msg Сообщение
     */
    bool send_message(const json& message) {
        Response response;
        if (send_message(message, RequestMode::Sync, 3s, &response)) {
            return response.isSuccess();
        }
        return false;

    }

    bool send_message(const json& message, RequestMode mode,
                      std::chrono::milliseconds timeout = 2s,
                      Response* out_response = nullptr) {
        std::unique_lock<std::mutex> lock(send_mutex_);

        // 1. Подготовка запроса
        std::shared_ptr<SyncRequest> sync_request;
        if (mode == RequestMode::Sync) {
            if (!out_response) return false;

            sync_request = request_manager_.create(
                    message["key"].get<std::string>(),
                    message["request"].get<std::string>()
            );

            // Добавляем таймстемп для отладки
            if (debug_mode_) {
                std::cout << "[REQ] Sending: " << message.dump()
                          << " (timeout: " << timeout.count() << "ms)\n";
            }
        }

        // 2. Отправка сообщения
        try {
            zmq::message_t zmq_msg(message.dump());
            auto send_result = adm_socket_.send(zmq_msg, zmq::send_flags::dontwait);
            if (!send_result) {
                if (debug_mode_) std::cerr << "[REQ] Send failed (EAGAIN)\n";
                return false;
            }
        } catch (const zmq::error_t& e) {
            if (debug_mode_) std::cerr << "[REQ] Send error: " << e.what() << "\n";
            connection_ok_ = false;
            return false;
        }

        // 3. Ожидание ответа (только для синхронного режима)
        if (mode == RequestMode::Sync && sync_request) {
            lock.unlock(); // Важно: отпускаем мьютекс перед ожиданием

            bool success = sync_request->wait(*out_response, timeout);

            if (debug_mode_) {
                std::cout << "[REQ] Wait result: " << (success ? "OK" : "TIMEOUT")
                          << ", Response: " << out_response->toJSON() << "\n";
            }

            return success;
        }

        return true;
    }

    /**
     * @brief Отправка запроса на подключение
     */
    bool send_connect() {
        json msg = {
                {"key", client_id_},
                {"request", "connect"},
                {"timeout", 3000}  // 3 секунды таймаут
        };

        Response response;
        if (send_message(msg, RequestMode::Sync, 5s, &response)) {  // Увеличенный таймаут
            if (response.isSuccess()) {
                // Дополнительная проверка ответа
                if (response.result==200) {
                    return true;
                }
            } else if (debug_mode_) {
                std::cerr << "Connection refused: " << response.message << "\n";
            }
        } else if (debug_mode_) {
            std::cerr << "No response to connect request\n";
        }

        return false;
    }

    bool send_subscribe(const std::vector<std::string>& tags) {
        json msg = {
                {"key", client_id_},
                {"request", "subscribe_values"},
                {"topic", "default"},
                {"keys", tags}
        };
        Response response;
        return send_message(msg, RequestMode::Sync, 5s, &response) &&
               response.isSuccess();
    }

    /**
     * @brief Отправка heartbeat
     * @return true если heartbeat успешен
     */
    bool send_heartbeat() {
        json msg = { {"key", client_id_}, {"request", "heartbeat"} };
        Response response;
        return send_message(msg, RequestMode::Sync, 1s, &response) &&
               response.isSuccess();
    }


    void cleanup_resources() {
        try {
            sockets_ready_ = false;

            if (adm_socket_.handle() != nullptr) {
                adm_socket_.close();
            }
            if (sub_socket_.handle() != nullptr) {
                sub_socket_.close();
            }
        } catch (...) {
            if (debug_mode_) {
                std::cerr << "Warning: error during socket cleanup\n";
            }
        }
    }

    /**
     * @brief Подключение к серверу
     */
    bool connect() {
        std::lock_guard<std::mutex> lock(connection_mutex_);
        if (connection_ok_) return true;
        bool result = false;
        trying_to_connect_ = true;
        cleanup_resources();
        try {
            adm_socket_ = zmq::socket_t(ctx_, zmq::socket_type::dealer);
            sub_socket_ = zmq::socket_t(ctx_, zmq::socket_type::sub);
            adm_socket_.set(zmq::sockopt::routing_id, client_id_);
            sub_socket_.set(zmq::sockopt::subscribe, "");

            adm_socket_.connect("tcp://" + server_host_ + ":5551");
            sub_socket_.connect("tcp://" + server_host_ + ":5552");

            sockets_ready_ = true;

            // Используем send_heartbeat вместо check_connection
            result = send_connect(); //send_heartbeat();

            if (result && debug_mode_) { std::cout << "Connected to server successfully\n";}
        } catch (...) {
            if (debug_mode_) {
                std::cerr << "Connection error\n";
            }
            cleanup_resources();
        }
        trying_to_connect_ = false;
        return result;
    }

    void notify_connection_state(bool connected) {
        if (connection_ok_ != connected) {
            connection_ok_ = connected;
            if (debug_mode_) {
                std::cerr << "Connection state changed to: "
                          << (connected ? "ONLINE" : "OFFLINE") << "\n";
            }
            // Принудительная перерисовка меню
            if (running_) {
                printMenu();
            }
        }
    }

    /**
     * @brief Мониторинг соединения
     */
    void connection_and_heartbeat_loop() {
        while (running_) {
            bool current_state = connection_ok_;
            if (!current_state) {
                current_state = connect(); // Пытаемся подключиться
            } else {
                current_state = send_heartbeat(); // Проверяем соединение
            }
            notify_connection_state(current_state);
            std::this_thread::sleep_for(current_state ? 3s : 5s);
        }
    }

    /**
     * @brief Обновление времени последнего heartbeat
     */
    void update_heartbeat_time() {
        std::lock_guard<std::mutex> lock(heartbeat_mutex_);
        last_heartbeat_time_ = std::chrono::steady_clock::now();
    }

    /**
     * @brief Получение времени последнего heartbeat
     * @return Время последнего heartbeat
     */
    std::chrono::steady_clock::time_point get_last_heartbeat_time() {
        std::lock_guard<std::mutex> lock(heartbeat_mutex_);
        return last_heartbeat_time_;
    }

    /* Основные обработчики */

    /**
     * @brief Цикл прослушивания сообщений
     */
    void listen_loop() {
        while (running_) {
            if (!sockets_ready_) {
                std::this_thread::sleep_for(100ms);
                continue;
            }

            zmq::pollitem_t items[] = {
                    {sub_socket_, 0, ZMQ_POLLIN, 0},
                    {adm_socket_, 0, ZMQ_POLLIN, 0}
            };

            try {
                int rc = zmq::poll(items, 2, 100ms);

                if (rc == -1 && errno == EINTR) {
                    // Системный вызов был прерван, продолжаем работу
                    if (debug_mode_) std::cout << "[ZMQ] Poll interrupted, continuing...\n";
                    continue;
                }

                if (rc > 0) {
                    if (items[0].revents & ZMQ_POLLIN) {
                        handle_pub_message();
                    }

                    if (items[1].revents & ZMQ_POLLIN) {
                        zmq::message_t msg;
                        if (adm_socket_.recv(msg, zmq::recv_flags::dontwait)) {
                            if (debug_mode_) {
                                std::cout << "[ADM] Raw message: "
                                          << msg.to_string_view() << "\n";
                            }
                            handle_adm_message(msg);
                        }
                    }
                }
            }
            catch (const zmq::error_t& e) {
                if (e.num() == EINTR) continue;  // Игнорируем прерывания
                if (debug_mode_) {
                    std::cerr << "[ZMQ ERROR] " << e.what()
                              << " (errno: " << e.num() << ")\n";
                }
                connection_ok_ = false;
            }
            catch (const std::exception& e) {
                if (debug_mode_) {
                    std::cerr << "[STD ERROR] " << e.what() << "\n";
                }
                connection_ok_ = false;
            }
            catch (...) {
                if (debug_mode_) {
                    std::cerr << "[UNKNOWN ERROR] Unexpected exception\n";
                }
                connection_ok_ = false;
            }
        }
    }
    /**
     * @brief Обработчик сообщений от сервера (публикации)
     */
    void handle_pub_message() {
        zmq::message_t msg;
        if (sub_socket_.recv(msg)) {
            try {
                auto update = SendValues::fromJSON(msg.to_string());
                if (update.key == client_id_) {
                    {
                        std::lock_guard<std::mutex> lock(updates_mutex_);
                        // Обновляем только те теги, которые пришли в сообщении
                        for (const auto& new_tag : update.values) {
                            bool found = false;
                            for (auto& existing_tag : last_updates_) {
                                if (existing_tag.key == new_tag.key) {
                                    existing_tag = new_tag; // Обновляем существующий тег
                                    found = true;
                                    break;
                                }
                            }
                            if (!found) {
                                last_updates_.push_back(new_tag); // Добавляем новый тег
                            }
                        }
                    }

                    if (debug_mode_) {
                        std::cout << "\n[PUB] Received updates (" << update.values.size() << " tags)\n";
                        for (const auto& tag : update.values) {
                            std::cout << "  " << tag.key << " = " << tag.value
                                      << " (" << toString(tag.quality) << ")\n";
                        }
                    }
                }
            } catch(const std::exception& e) {
                if (debug_mode_) {
                    std::cerr << "Failed to process update message: " << e.what() << "\n";
                }
            } catch(...) {
                if (debug_mode_) {
                    std::cerr << "Failed to process update message (unknown error)\n";
                }
            }
        }
    }

    void readTagValue() {
        std::cout << "Enter tag name: ";
        std::string tag_name;
        std::cin >> tag_name;

        json msg = {
                {"key", client_id_},
                {"request", "read_tag"},
                {"tag", tag_name}
        };

        Response response;
        if (send_message(msg, RequestMode::Sync, 1s, &response)) {
            if (response.isSuccess()) {
                std::cout << "Tag value: " << response.message << "\n";
            } else {
                std::cerr << "Error: " << response.message << "\n";
            }
        } else {
            std::cerr << "No response from server\n";
        }
    }

    void writeTagValue() {
        std::cout << "Enter tag name: ";
        std::string tag_name;
        std::cin >> tag_name;

        std::cout << "Enter new value: ";
        std::string value;
        std::cin >> value;

        json msg = {
                {"key", client_id_},
                {"request", "write_tag"},
                {"tag", tag_name},
                {"value", value}
        };

        Response response;
        if (send_message(msg, RequestMode::Sync, 1s, &response)) {
            if (response.isSuccess()) {
                std::cout << "Value written successfully\n";
            } else {
                std::cerr << "Error: " << response.message << "\n";
            }
        } else {
            std::cerr << "No response from server\n";
        }
    }

    /**
     * @brief Обработчик административных сообщений
     */
    void handle_adm_message(zmq::message_t& msg) {
        try {
            auto response = Response::fromJSON(msg.to_string());

            // Сначала пробуем обработать как синхронный ответ
            if (!request_manager_.process_response(response)) {

                // Асинхронные сообщения
                if (response.request == "heartbeat") {
                    //last_heartbeat_ = std::chrono::steady_clock::now();
                    return;
                }

                // Other responses
                if (debug_mode_) {
                    std::cout << "[ADM] Response: " << response.toJSON() << "\n";
                }

            }

        } catch(const json::exception& e) {
            if (debug_mode_) {
                std::cerr << "Failed to parse response: " << e.what() << "\n";
            }
        }
    }

    std::vector<Tag> getLastUpdates() {
        std::lock_guard<std::mutex> lock(updates_mutex_);
        return last_updates_;
    }

    /**
     * @brief Вывод меню с обновлением статуса
     */
    void printMenu() {
        std::lock_guard<std::mutex> lock(console_mutex_);
        clear_screen();

        // Получаем последние обновления
        auto updates = getLastUpdates();

        std::cout << "=== ZMQ Client ==="
                  << "\nStatus: " << (connection_ok_ ? "\033[32mONLINE\033[0m" : "\033[31mOFFLINE\033[0m")
                  << "\nSubscribed tags (" << updates.size() << "):\n";

        // Выводим последние значения тегов
        for (const auto& tag : updates) {
            std::cout << "  " << tag.key << " = " << tag.value
                      << " (" << toString(tag.quality) << ")\n";
        }

        std::cout << "\nMenu:\n"
                  << "1. Subscribe to tags\n"
                  << "2. Unsubscribe\n"
                  << "3. Send program\n"
                  << "4. Get PLC status\n"
                  << "5. Start PLC\n"
                  << "6. Stop PLC\n"
                  << "7. Pause PLC\n"
                  << "8. Resume PLC\n"
                  << "9. Read tag value\n"
                  << "10. Write tag value\n"
                  << "0. Exit\n\n"
                  << "Choice: " << std::flush;
    }

    /**
     * @brief Обработчик выбора в меню
     */
    void handleMenuChoice(int choice) {
            if (choice != 0) {
                if (!connection_ok_) return;
                switch (choice) {
                    case 1: testSubscribe(); break;
                    case 2: testUnsubscribe(); break;
                    case 3: testFileTransfer(); break;
                    case 4: { // Добавляем новый пункт меню
                        std::string status;
                        if (getExecutionStatus(status)) {
                            std::cout << "Current PLC status: " << status << "\n";
                        } else {
                            std::cerr << "Failed to get status\n";
                        }
                        break;
                    }
                    case 5:
                        if (sendExecutionStart()) {
                            std::cout << "Start command sent successfully\n";
                        }
                        break;
                    case 6:
                        if (sendExecutionStop()) {
                            std::cout << "Stop command sent successfully\n";
                        }
                        break;
                    case 7: // Pause
                        if (connection_ok_) {
                            if (sendExecutionPause()) {
                                std::cout << "Pause command sent successfully\n";
                            }
                        } else {
                            std::cerr << "Not connected to server!\n";
                        }
                        break;

                    case 8: // Resume
                        if (connection_ok_) {
                            if (sendExecutionResume()) {
                                std::cout << "Resume command sent successfully\n";
                            }
                        } else {
                            std::cerr << "Not connected to server!\n";
                        }
                        break;
                    case 9: readTagValue(); break;
                    case 10: writeTagValue(); break;
                    default:
                        std::cout << "Invalid choice!\n";
                }

                std::this_thread::sleep_for(500ms);
            } else {
                running_ = false;

            }
    }

    bool sendExecutionStart() {
        json msg = {
                {"key", client_id_},
                {"request", "execution_start"}
        };

        Response response;
        if (send_message(msg, RequestMode::Sync, 3s, &response)) {
            if (response.isSuccess()) {
                std::cout << "PLC start command accepted. " << response.message << "\n";
                return true;
            } else {
                std::cerr << "Failed to start PLC: " << response.message << "\n";
            }
        } else {
            std::cerr << "No response from server\n";
        }
        return false;
    }

    bool sendExecutionStop() {
        json msg = {
                {"key", client_id_},
                {"request", "execution_stop"}
        };

        Response response;
        if (send_message(msg, RequestMode::Sync, 3s, &response)) {
            if (response.isSuccess()) {
                std::cout << "PLC stop command accepted. " << response.message << "\n";
                return true;
            } else {
                std::cerr << "Failed to stop PLC: " << response.message << "\n";
            }
        } else {
            std::cerr << "No response from server\n";
        }
        return false;
    }

    bool sendExecutionPause() {
        json msg = {
                {"key", client_id_},
                {"request", "execution_pause"}
        };

        Response response;
        if (send_message(msg, RequestMode::Sync, 3s, &response)) {
            if (response.isSuccess()) {
                std::cout << "PLC pause command accepted. "
                          << response.message << "\n";
                return true;
            } else {
                std::cerr << "Failed to pause PLC: "
                          << response.message << "\n";
            }
        } else {
            std::cerr << "No response from server\n";
        }
        return false;
    }

    bool sendExecutionResume() {
        json msg = {
                {"key", client_id_},
                {"request", "execution_resume"}
        };

        Response response;
        if (send_message(msg, RequestMode::Sync, 3s, &response)) {
            if (response.isSuccess()) {
                std::cout << "PLC resume command accepted. "
                          << response.message << "\n";
                return true;
            } else {
                std::cerr << "Failed to resume PLC: "
                          << response.message << "\n";
            }
        } else {
            std::cerr << "No response from server\n";
        }
        return false;
    }

    bool getExecutionStatus(std::string& out_status) {
        json msg = {
                {"key", client_id_},
                {"request", "execution_status"}
        };

        Response response;
        if (send_message(msg, RequestMode::Sync, 1s, &response)) {
            if (response.isSuccess()) {
                out_status = response.message;
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Тест подписки на регистры
     */
    void testSubscribe() {
        std::cout << "=== Subscribe Test ===\n";
        std::cout << "Enter tags to subscribe (comma separated, e.g. MW0,MW1,IX0.5): ";

        // Очищаем буфер перед чтением строки
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // Читаем всю строку
        std::string input;
        std::getline(std::cin, input);

        // Разделяем теги
        std::vector<std::string> tags;
        size_t pos = 0;
        while ((pos = input.find(',')) != std::string::npos) {
            std::string tag = input.substr(0, pos);
            if (!tag.empty()) {
                tags.push_back(tag);
            }
            input.erase(0, pos + 1);
        }
        if (!input.empty()) {
            tags.push_back(input);
        }

        // Отправляем на сервер
        if (!tags.empty()) {
            json msg = {
                    {"key", client_id_},
                    {"request", "subscribe_values"},
                    {"topic", "default_topic"},
                    {"keys", tags}
            };
            adm_socket_.send(zmq::buffer(msg.dump()));
        }

        std::cout << "Subscribed to " << tags.size() << " tags\n";
    }

    /**
     * @brief Тест отписки от регистров
     */
    void testUnsubscribe() {
        std::cout << "\n=== Unsubscribe Test ===\n";
        json msg = {
                {"key", client_id_},
                {"request", "unsubscribe_values"},
                {"topic", "default_topic"}
        };
        send_message(msg);
        std::cout << "Unsubscribe request sent\n";
    }

    /**
     * @brief Тест передачи файлов
     */
    void testFileTransfer() {
        std::cout << "\n=== File Transfer Test ===\n";

        // Очистка буфера ввода перед чтением
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::string program_name;
        std::cout << "Enter program name: ";
        std::getline(std::cin, program_name);

        // Дополнительная проверка на пустое имя
//        while (program_name.empty()) {
//            std::cout << "Program name cannot be empty! Try again: ";
//            std::getline(std::cin, program_name);
//        }

        std::vector<std::string> files;
        std::cout << "Enter file paths (one per line, empty line to finish):\n";

        std::string file_path;
        while (true) {
            std::cout << "> ";
            std::getline(std::cin, file_path);

            if (file_path.empty()) {
                break;
            }

            if (!fs::exists(file_path)) {
                std::cout << "File not found: " << file_path << "\n";
                continue;
            }

            files.push_back(file_path);
        }

        if (files.empty()) {
            std::cout << "No files to transfer\n";
            return;
        }

        std::cout << "Sending " << files.size() << " files...\n";
        send_program(program_name, files);
        std::cout << "File transfer completed\n";
    }

    /**
     * @brief Отправка программы на сервер
     * @param program_name Имя программы
     * @param file_paths Пути к файлам программы
     */
    void send_program(const std::string& program_name, const std::vector<std::string>& file_paths) {
        uint64_t program_hash = calculate_program_hash(file_paths);

        json prog_start = {
                {"key", client_id_},
                {"request", "prog_start"},
                {"prog_name", program_name},
                {"prog_hash", program_hash}
        };
        send_message(prog_start);

        for (const auto& file_path : file_paths) {
            send_file(file_path);
        }

        json prog_end = {
                {"key", client_id_},
                {"request", "prog_end"}
        };
        send_message(prog_end);
    }

    /**
     * @brief Отправка одного файла
     * @param file_path Путь к файлу
     */
    void send_file(const std::string& file_path) {
        fs::path path(file_path);
        std::string file_name = path.filename().string();
        uint64_t file_size = fs::file_size(path);

        // 1. Отправляем file_start
        json file_start = {
                {"key", client_id_},
                {"request", "file_start"},
                {"file_name", file_name},
                {"file_size", file_size}
        };
        send_message(file_start);

        std::ifstream file(file_path, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << file_path << std::endl;
            return;
        }

        const size_t CHUNK_SIZE = 63 * 1024; // Оптимальный размер для Base64
        std::vector<char> buffer(CHUNK_SIZE);
        uint64_t total_sent = 0;

        // 2. Отправляем чанки файла
        while (file) {
            file.read(buffer.data(), buffer.size());
            size_t bytes_read = file.gcount();

            if (bytes_read > 0) {
                std::string chunk_data(buffer.data(), bytes_read);
                std::string encoded = utils::base64_encode(chunk_data);

                json file_chunk = {
                        {"key", client_id_},
                        {"request", "file_chunk"},
                        {"chunk_data", encoded},
                        {"chunk_size", bytes_read} // Оригинальный размер, не закодированный
                };

                if (!send_message(file_chunk)) {
                    std::cerr << "Failed to send chunk" << std::endl;
                    break;
                }

                total_sent += bytes_read;
                if (debug_mode_) {
                    float progress = (total_sent * 100.0f) / file_size;
                    std::cout << "\rProgress: " << std::fixed << std::setprecision(1)
                              << progress << "% (" << total_sent << "/" << file_size << ")";
                    std::cout.flush();
                }
            }
        }

        // 3. Отправляем file_end
        json file_end = {
                {"key", client_id_},
                {"request", "file_end"}
        };
        send_message(file_end);

        if (debug_mode_) {
            std::cout << "\nFile transfer completed: " << file_name
                      << " (" << file_size << " bytes)" << std::endl;
        }
    }

    /**
     * @brief Расчет хеша программы
     * @param file_paths Пути к файлам программы
     * @return Хеш программы
     */
    static uint64_t calculate_program_hash(const std::vector<std::string>& file_paths) {
        uint64_t combined_hash = 0;

        for (const auto& file_path : file_paths) {
            uint32_t file_crc = utils::calculate_file_crc32(file_path);
            uint64_t file_size = fs::file_size(file_path);
            combined_hash ^= (static_cast<uint64_t>(file_crc) << 32) | file_size;
        }

        return combined_hash;
    }
};

/**
 * @brief Точка входа в программу
 */
//int main() {
//#ifdef _WIN32
//    enable_ansi_colors();
//#else
//    setenv("TERM", "xterm-256color", 1);  // Перезаписать, если переменная уже есть
//#endif
//
//    try {
//        TestClient client("test_client_1", "192.168.1.68");
//        client.start();
//        client.runMenu();
//        client.stop();
//    }
//    catch (const std::exception& e) {
//        std::cerr << "Error: " << e.what() << std::endl;
//        return 1;
//    }
//    return 0;
//}

int main() {
#ifdef _WIN32
    enable_ansi_colors();
#else
    // Перезаписать, если переменная уже есть
    setenv("TERM", "xterm-256color", 1);
#endif

    try {
        std::string server_address;

        // Запрос адреса сервера
        std::cout << "=== ZMQ Client ===" << std::endl;
        std::cout << "Enter server address [default: 192.168.1.68]: ";
        std::getline(std::cin, server_address);

        // Установка адреса по умолчанию, если ввод пустой
        if (server_address.empty()) {
            server_address = "192.168.1.68";
            std::cout << "Using default address: " << server_address << std::endl;
        }

        TestClient client("test_client_1", server_address);
        client.start();
        client.runMenu();
        client.stop();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}