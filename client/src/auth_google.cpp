#include "auth_google.h"
#include <user.h>
#include <QDesktopServices>
#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QOAuthHttpServerReplyHandler>
#include <QObject>
#include <QString>
#include <QUrl>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void GoogleAuth::work() {
    QOAuth2AuthorizationCodeFlow google{this};
    google.setScope("email");
    connect(&google, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
            &QDesktopServices::openUrl);

    google.setAuthorizationUrl(
        QUrl("https://accounts.google.com/o/oauth2/auth"));
    google.setClientIdentifier(
        "656689579627-iulsp1abjnrtd8hhui64i27qq5etgaej.apps.googleusercontent."
        "com");
    google.setAccessTokenUrl(QUrl("https://oauth2.googleapis.com/token"));
    google.setClientIdentifierSharedKey("NKhwrtNDtewe1dyswLVu-srR");

    google.setModifyParametersFunction(
        [](QAbstractOAuth::Stage stage,
           QMultiMap<QString, QVariant> *parameters) {
            // Percent-decode the "code" parameter so Google can match it
            if (stage == QAbstractOAuth::Stage::RequestingAccessToken) {
                QByteArray code = parameters->value("code").toByteArray();
                parameters->remove("code");
                parameters->insert("code", QUrl::fromPercentEncoding(code));
            }
        });

    auto replyHandler = new QOAuthHttpServerReplyHandler(10002, this);
    google.setReplyHandler(replyHandler);
    connect(&google, &QOAuth2AuthorizationCodeFlow::granted, [&google]() {
        user.set_google_token(google.token().toStdString());
    });

    QEventLoop eventLoop{this};
    QObject::connect(&google, &QOAuth2AuthorizationCodeFlow::granted,
                     &eventLoop, &QEventLoop::quit);
    google.grant();
    eventLoop.exec();
}