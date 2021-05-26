#ifndef AUTH_GOOGLE_H
#define AUTH_GOOGLE_H
#include <QNetworkReply>
#include <QOAuth2AuthorizationCodeFlow>
#include <QObject>

class GoogleAuth : public QObject {
    Q_OBJECT
public:
    explicit GoogleAuth(QObject *parent = nullptr);

signals:
    void gotToken(const QString &token);

private:
    QOAuth2AuthorizationCodeFlow *google;
};
#endif  // AUTH_GOOGLE_H
