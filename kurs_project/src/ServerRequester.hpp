#ifndef SERVERREQUESTER_HPP
#define SERVERREQUESTER_HPP

#include <cstdlib>
#include <QFile>
#include <QLabel>
#include <QPushButton>
#include <QDialog>
#include <boost/asio.hpp>
#include <boost/json.hpp>
#include "SessionsManager.hpp"
#include <iostream>

namespace ip   = boost::asio::ip;
namespace asio = boost::asio;
namespace json = boost::json;
using     tcp  = boost::asio::ip::tcp;

namespace Server {
    class Requester
    {
    public:
        Requester();
        ~Requester();

        static void register_user(const char *login,
                                  const char *password,
                                  const char *email,
                                  QLabel *log_err_lb,
                                  QPushButton *reg_btn,
                                  QPushButton *log_btn,
                                  QPushButton *prof_btn,
                                  QPushButton *sett_btn,
                                  QDialog *diag);
        static void login_user(const char *login,
                               const char *password,
                               QLabel *log_err_lb,
                               QLabel *pass_err_lb,
                               QPushButton *reg_btn,
                               QPushButton *log_btn,
                               QPushButton *prof_btn,
                               QPushButton *sett_btn);
        static void add_convertation_to_history(const char *inpath,
                                                const char *outpath,
                                                std::function<void(char*, long long)> show_banner);

        static int set_up();
        static char* ip();
        static int port();

    private:
        static char     ip_[16];
        static unsigned short port_;
    };

    enum SQL_OPERATIONS {
        SQL_SELECT_USER,
        SQL_INSERT_USER,
        SQL_SELECT_CONVERTS,
        SQL_INSERT_CONVERTS
    };

    enum OP_STATUS {
        OP_OK,
        OP_LOGIN_ERR,
        OP_PASSWORD_ERR
    };
}

#endif // SERVERREQUESTER_HPP
