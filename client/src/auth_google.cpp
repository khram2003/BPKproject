#include "auth_google.h"
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QOAuthHttpServerReplyHandler>
#include <QObject>
#include <QString>
#include <QUrl>
#include <iostream>

GoogleAuth::GoogleAuth(QObject *parent) : QObject(parent) {
    this->google = new QOAuth2AuthorizationCodeFlow(this);
    this->google->setScope("email");

    connect(this->google, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
            &QDesktopServices::openUrl);

    QString val;
    QFile file;
    // видимо надо указать свой путь до этого файла
    //    file.setFileName(QDir::toNativeSeparators("secret.json"));
    //    QDir::setCurrent(qApp->applicationDirPath());
    //    file.setFileName(
    //        "client_secret_656689579627_iulsp1abjnrtd8hhui64i27qq5etgaej_apps.json");
    //    if (!file.exists()){
    //        std::cerr<<file.fileName();
    //    }
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        val = file.readAll();
        file.close();
    }

    QJsonDocument document = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject object = document.object();
    const auto settingsObject = object["web"].toObject();

    //    const QUrl authUri(settingsObject["auth_uri"].toString());
    const QUrl authUri("https://accounts.google.com/o/oauth2/auth");

    //    const auto clientId = settingsObject["client_id"].toString();
    const QString clientId =
        "656689579627-iulsp1abjnrtd8hhui64i27qq5etgaej.apps.googleusercontent."
        "com";

    //    const QUrl tokenUri(settingsObject["token_uri"].toString());
    const QUrl tokenUri("https://oauth2.googleapis.com/token");

    //    const auto clientSecret(settingsObject["client_secret"].toString());
    const auto clientSecret("NKhwrtNDtewe1dyswLVu-srR");
    //    const auto redirectUris = settingsObject["redirect_uris"].toArray();
    const auto redirectUris = "http://127.0.0.1:9008/";
    //    const QUrl redirectUri(redirectUris[0].toString());
    const QUrl redirectUri("http://127.0.0.1:9008/");
    const auto port = static_cast<quint16>(9008);

    this->google->setAuthorizationUrl(authUri);
    this->google->setClientIdentifier(clientId);
    this->google->setAccessTokenUrl(tokenUri);
    this->google->setClientIdentifierSharedKey(clientSecret);

    //    this->google->setModifyParametersFunction([&](QAbstractOAuth::Stage
    //    stage, QVariantMap* parameters) {
    //      // Percent-decode the "code" parameter so Google can match it
    //      if (stage == QAbstractOAuth::Stage::RequestingAccessToken) {
    //          QByteArray code = parameters->value("code").toByteArray();
    //          (*parameters)["code"] = QUrl::fromPercentEncoding(code);
    //      }
    //    });

    auto replyHandler = new QOAuthHttpServerReplyHandler(port, this);
    this->google->setReplyHandler(replyHandler);
    this->google->grant();

    connect(this->google, &QOAuth2AuthorizationCodeFlow::granted, [=]() {
        const QString token = this->google->token();
        emit gotToken(token);
    });

    //    connect(this->google, &QOAuth2AuthorizationCodeFlow::granted, [=]() {
    //        qDebug() << __FUNCTION__ << __LINE__ << "Access Granted!";
    //        auto reply =
    //        this->google->get(QUrl("https://www.googleapis.com/plus/v1/people/me"));
    //        connect(reply, &QNetworkReply::finished, [reply]() {
    //            qDebug() << "REQUEST FINISHED. Error? " << (reply->error() !=
    //            QNetworkReply::NoError); qDebug() << reply->readAll();
    //        });
    //    });
}