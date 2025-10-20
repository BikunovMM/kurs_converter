#ifndef SESSIONSMANAGER_H
#define SESSIONSMANAGER_H

#include <QSettings>

namespace Sessions {
    class Manager
    {
    public:
        Manager();
        ~Manager();

        static void save_session(const char *login, const char *password, long long iduser);
        static void load_session(char *login, char *password, long long *id);
        static void delete_session();
        static bool contains_session();
        static long long iduser();
    private:
        static QSettings *settings;
    };
}

#endif // SESSIONSMANAGER_H
