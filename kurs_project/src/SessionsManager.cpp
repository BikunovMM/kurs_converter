#include "SessionsManager.hpp"

void Sessions::Manager::save_session(const char *login, const char *password, long long iduser) {
    settings->setValue("user/login", login);
    settings->setValue("user/password", password);
    settings->setValue("user/id", iduser);
}

void Sessions::Manager::load_session(char *login, char *password, long long *id) {
    snprintf(login, 0, "%s",
             settings->value("user/login").toByteArray().data());
    snprintf(password, 0, "%s",
             settings->value("user/password").toByteArray().data());
    //id = settings->value("user/id").toULongLong().
}

void Sessions::Manager::delete_session() {
    settings->clear();
}

bool Sessions::Manager::contains_session() {
    return settings->contains("user/id");
}

long long Sessions::Manager::iduser() {
    return settings->value("user/id").toLongLong();
}

QSettings* Sessions::Manager::settings = new QSettings("bikunovmm", "converter");
