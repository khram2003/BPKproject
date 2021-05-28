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
    google.setScope("profile email");
    connect(&google, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
            &QDesktopServices::openUrl);

    std::ifstream is("BPK_CLIENT_CONFIG.json");
    if (!is) {
        throw std::runtime_error(
            "Couldn't open BPK_CLIENT_CONFIG, check if file exists\n");
    }
    json j = json::parse(is);
    is.close();
    json auth_info = j["google_authentification"];
    google.setAuthorizationUrl(
        QUrl(QString::fromStdString(auth_info["auth_uri"].get<std::string>())));
    const auto client_id = auth_info["client_id"].get<std::string>();
    google.setClientIdentifier(client_id.c_str());
    google.setAccessTokenUrl(QUrl(
        QString::fromStdString(auth_info["token_uri"].get<std::string>())));
    const auto shared_key = auth_info["client_secret"].get<std::string>();
    google.setClientIdentifierSharedKey(shared_key.c_str());

    google.setModifyParametersFunction(
        [](QAbstractOAuth::Stage stage,
           QMultiMap<QString, QVariant> *parameters) {
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
        user.download_userinfo();
    });

    QEventLoop eventLoop{this};
    QObject::connect(&google, &QOAuth2AuthorizationCodeFlow::granted,
                     &eventLoop, &QEventLoop::quit);
    google.grant();
    eventLoop.exec();
}