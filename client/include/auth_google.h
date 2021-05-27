#ifndef AUTH_GOOGLE_H
#define AUTH_GOOGLE_H
#include <QNetworkReply>
#include <QOAuth2AuthorizationCodeFlow>
#include <QObject>

class GoogleAuth : public QObject {
    Q_OBJECT
public:
    GoogleAuth() = default;

    void work();
};
#endif  // AUTH_GOOGLE_H
