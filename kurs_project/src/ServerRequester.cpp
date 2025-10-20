#include "ServerRequester.hpp"
#include <boost/json/src.hpp>

Server::Requester::Requester()
    //: io_ctx_(), socket_(io_ctx_)
{

}

Server::Requester::~Requester()
{

}

void Server::Requester::register_user(const char *login,
                                      const char *password,
                                      const char *email,
                                      QLabel *log_err_lb,
                                      QPushButton *reg_btn,
                                      QPushButton *log_btn,
                                      QPushButton *prof_btn,
                                      QPushButton *sett_btn,
                                      QDialog     *diag)
{
    try {
        if (!diag) {
            return;
        }
        asio::io_context  io_ctx;
        json::object      req_json;
        std::string       req_str{};
        tcp::socket      *socket = new tcp::socket(io_ctx);

        req_json["operation"] = Server::SQL_INSERT_USER;
        req_json["login"]     = login;
        req_json["password"]  = password;
        req_json["email"]     = email;

        req_str = json::serialize(req_json);

        socket->async_connect(
            tcp::endpoint(ip::make_address(Server::IP()),
            Server::PORT()),
            [socket, req_str, log_err_lb, login, password,
             reg_btn, log_btn, prof_btn, sett_btn, diag]
                (const boost::system::error_code& err) mutable
            {
                if (err) {
                    fprintf(stderr, "Failed to async_connect.\n");
                    delete(socket);
                    return;
                }
                if (!diag ) {
                    delete(socket);
                    return;
                }

                std::cout << "Connected to server." << std::endl;

                asio::async_write(*socket,
                  asio::buffer(req_str),
                    [socket, log_err_lb, login, password, reg_btn,
                     log_btn, prof_btn, sett_btn, diag]
                        (const boost::system::error_code& err,
                         size_t bytes) mutable
                    {
                        if (err || !diag) {
                            delete(socket);
                            return;
                        }

                        std::cout << "writing to server." << std::endl;

                        char *buffer = (char*)malloc(128 * sizeof(char));
                        if (!buffer) {
                            delete(socket);
                            return;
                        }

                        socket->async_read_some(
                            asio::buffer(buffer, 127),
                            [socket, buffer, log_err_lb, login, password,
                             reg_btn, log_btn, prof_btn, sett_btn, diag]
                            (const boost::system::error_code& err,
                                size_t bytes) mutable
                            {
                                if (err) {
                                    free(buffer);
                                    delete(socket);
                                    log_err_lb->setText("Server error...");
                                    return;
                                }

                                if (!diag) {
                                    free(buffer);
                                    delete(socket);
                                    return;
                                }

                                buffer[bytes] = '\0';

                                std::cout << "reading from server: " << buffer << std::endl;

                                json::object res_json = json::parse(buffer).as_object();
                                std::cout << "..." << std::endl;
                                const long long status = res_json["status"].as_int64();

                                if (status == Server::OP_OK) {
                                    //swithc ui to registered
                                    if (!Sessions::Manager::contains_session()) {
                                        reg_btn->setHidden(true);
                                        log_btn->setHidden(true);
                                        prof_btn->setHidden(false);
                                        sett_btn->setHidden(false);
                                        printf("set hidden.\n");
                                    }

                                    Sessions::Manager::save_session(login, password, res_json["iduser"].as_int64());
                                    diag->done(0);

                                    printf("registered\n");
                                }
                                else {
                                    log_err_lb->setText("this login is already taken.");
                                }

                                free(buffer);
                                delete(socket);
                            }
                        );
                    }
                );
            }
        );

        io_ctx.run();
    }
    catch (std::exception& e) {
        fprintf(stderr, "Failed to register user. Error: %s.\n",
                e.what());
    }
}

static void login_user(char *login,
                       char *password,
                       QLabel *log_err_lb,
                       QLabel *pass_err_lb,
                       QPushButton *reg_btn,
                       QPushButton *log_btn,
                       QPushButton *prof_btn,
                       QPushButton *sett_btn)
{
    try {
        if (!log_err_lb) {
            return;
        }
        asio::io_context  io_ctx;
        json::object      req_json;
        std::string       req_str{};
        tcp::socket      *socket = new tcp::socket(io_ctx);

        req_json["operation"] = Server::SQL_SELECT_USER;
        req_json["login"]     = login;
        req_json["password"]  = password;

        req_str = json::serialize(req_json);

        socket->async_connect(
            tcp::endpoint(ip::make_address(Server::IP()),
            Server::PORT()),
            [socket, req_str, log_err_lb, pass_err_lb, login,
            password, reg_btn, log_btn, prof_btn, sett_btn]
            (const boost::system::error_code& err) mutable
            {
                if (err) {
                    fprintf(stderr, "Failed to async_connect.\n");
                    delete(socket);
                    return;
                }
                if (!log_err_lb) {
                    delete(socket);
                    return;
                }

                std::cout << "Connected to server." << std::endl;

                asio::async_write(*socket,
                    asio::buffer(req_str),
                    [socket, log_err_lb, pass_err_lb, login,
                    password, reg_btn, log_btn, prof_btn, sett_btn]
                    (const boost::system::error_code& err,
                    size_t bytes) mutable
                    {
                        if (err || !log_err_lb) {
                            delete(socket);
                            return;
                        }

                        std::cout << "writing to server." << std::endl;

                        char *buffer = (char*)malloc(128 * sizeof(char));
                        if (!buffer) {
                            delete(socket);
                              return;
                        }

                        socket->async_read_some(
                            asio::buffer(buffer, 127),
                            [socket, buffer, log_err_lb, pass_err_lb, login,
                             password, reg_btn, log_btn, prof_btn, sett_btn]
                            (const boost::system::error_code& err,
                            size_t bytes) mutable
                            {
                                if (err) {
                                    free(buffer);
                                    delete(socket);
                                    log_err_lb->setText("Server error...");
                                    return;
                                }

                                if (!log_err_lb) {
                                    free(buffer);
                                    delete(socket);
                                    return;
                                }

                                std::cout << "reading from server" << std::endl;

                                json::object res_json = json::parse(buffer).as_object();
                                const long long status = res_json["status"].as_int64();

                                if (status == Server::OP_OK) {
                                    //swithc ui to registered
                                    if (!Sessions::Manager::contains_session()) {
                                        reg_btn->setHidden(true);
                                        log_btn->setHidden(true);
                                        prof_btn->setHidden(false);
                                        sett_btn->setHidden(false);
                                    }

                                    Sessions::Manager::save_session(login, password, res_json["iduser"].as_int64());

                                    printf("registered");
                                }
                                else if (status == Server::OP_LOGIN_ERR){
                                    log_err_lb->setText("wrong login");
                                }
                                else {
                                    pass_err_lb->setText("wrong password");
                                }

                                free(buffer);
                                delete(socket);
                            }
                        );
                    }
                );
            }
        );

        io_ctx.run();
    }
    catch (std::exception& e) {
        fprintf(stderr, "Failed to register user. Error: %s.\n",
                e.what());
    }
}

void Server::Requester::add_convertation_to_history(const char *inpath,
                                                    const char *outpath,
                                                    std::function<void(char*, long long)> show_banner)
{
    try {
        asio::io_context  io_ctx;
        json::object      req_json;
        std::string       req_str{};
        tcp::socket      *socket = new tcp::socket(io_ctx);

        req_json["operation"] = Server::SQL_INSERT_CONVERTS;
        req_json["infn"]    = inpath;
        req_json["outfn"]   = outpath;
        req_json["iduser"]    = Sessions::Manager::iduser();

        req_str = json::serialize(req_json);
        std::cout << "req_str: " << req_str << std::endl;

        socket->async_connect(
            tcp::endpoint(ip::make_address(Server::IP()),
            Server::PORT()),
            [socket, req_str, show_banner]
            (const boost::system::error_code& err) mutable
            {
                if (err) {
                    fprintf(stderr, "Failed to async_connect.\n");
                    delete(socket);
                    return;
                }
                std::cout << "connected: "<< req_str << std::endl;

                asio::async_write(*socket, asio::buffer(req_str),
                    [socket, show_banner]
                    (const boost::system::error_code& err,
                     size_t bytes) mutable
                    {
                        if (err) {
                            fprintf(stderr,
                                "Failed to async_write.\n");
                            delete(socket);
                            return;
                        }

                        std::cout << "async_write: " << std::endl;

                        char *buffer = (char*)
                            malloc(11 * sizeof(char));
                        if (!buffer) {
                            delete(socket);
                            return;
                        }

                        socket->async_read_some(
                            asio::buffer(buffer, 10),
                            [socket, buffer, show_banner]
                            (const boost::system::error_code& err,
                             size_t bytes) mutable
                            {
                                if (err) {
                                    fprintf(stderr,
                                        "Failed to async_connect.\n");
                                    delete(socket);
                                    free(buffer);
                                    return;
                                }

                                std::cout << "async_read_some" << std::endl;

                                const long long buffer_len =
                                    std::strtoll(buffer, nullptr, 10);

                                buffer = (char*)
                                    realloc(buffer, (buffer_len + 1) * sizeof(char));
                                if (!buffer) {
                                    fprintf(stderr,
                                            "Failed to realloc buffer.\n");
                                    delete(socket);
                                    return;
                                }

                                std::cout << "buffer len: " << buffer_len << std::endl;

                                asio::async_read(*socket,
                                    asio::buffer(buffer, buffer_len),
                                    asio::transfer_exactly(buffer_len),
                                    [socket, buffer, show_banner, buffer_len]
                                    (const boost::system::error_code& err,
                                     size_t bytes) mutable
                                    {
                                        if (err) {
                                            fprintf(stderr,
                                                "! Failed to async_read.\n");
                                            delete(socket);
                                            free(buffer);
                                            return;
                                        }

                                        show_banner(buffer, buffer_len);

                                        delete(socket);
                                    }
                                );
                            }
                        );
                    }
                );
            }
        );
        io_ctx.run();
    }
    catch (std::exception& e) {
        fprintf(stderr, "Failed to add_convertation_to_history. Err: %s", e.what());
    }
}
