#include <iostream>
#include <QDesktopServices>
#include <QGuiApplication>
#include <QObject>
#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>

class GoogleAuth : public QObject {
Q_OBJECT
public:
    GoogleAuth() {}

    void work() {
        QOAuth2AuthorizationCodeFlow google{this};
        google.setScope("email");
        connect(&google, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
                &QDesktopServices::openUrl);

        google.setAuthorizationUrl(
            QUrl("https://accounts.google.com/o/oauth2/auth"));
        google.setClientIdentifier(
            "656689579627-iulsp1abjnrtd8hhui64i27qq5etgaej.apps.googleusercontent."
            "com");
        google.setAccessTokenUrl(
            QUrl("https://oauth2.googleapis.com/token"));
        google.setClientIdentifierSharedKey("NKhwrtNDtewe1dyswLVu-srR");


        auto replyHandler = new QOAuthHttpServerReplyHandler(10002, this);
        google.setReplyHandler(replyHandler);
        connect(&google, &QOAuth2AuthorizationCodeFlow::granted, []() {
          std::cerr << "Granted" << std::endl;
        });
        google.setModifyParametersFunction([](QAbstractOAuth::Stage stage, QMultiMap<QString, QVariant>* parameters) {
          // Percent-decode the "code" parameter so Google can match it
          if (stage == QAbstractOAuth::Stage::RequestingAccessToken) {
              QByteArray code = parameters->value("code").toByteArray();
              parameters->remove("code");
              parameters->insert("code", QUrl::fromPercentEncoding(code));
          }
        });

        QEventLoop eventLoop{this};
        QObject::connect(&google, &QOAuth2AuthorizationCodeFlow::granted, &eventLoop, &QEventLoop::quit);
        google.grant();
        eventLoop.exec();
    }
};