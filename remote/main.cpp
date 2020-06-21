#include <iostream>

#include <QDateTime>
#include <QFileInfo>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSocketNotifier>
#include <QAbstractEventDispatcher>
#include <QHostAddress>
#include <QTimer>


extern "C" {
#include <ela/ela.h>

#include <foils/hid.h>
}

#include <elaqt/elaqt.h>
#include "foilhid.h"

#include <QDebug>

QString type2string(QtMsgType type)
{
    switch (type) {
    case QtWarningMsg:
        return "WARNG";

    case QtCriticalMsg:
        return "ERROR";

    case QtFatalMsg:
        return "FATAL";

    case QtInfoMsg:
        return "INFO ";

    case QtDebugMsg:
        return "DEBUG";
    }

    return "     ";
}

static void messagehandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    QString output = QStringLiteral("%1 [%2] %3 (%4:%5)").arg(QTime::currentTime().toString(Qt::ISODateWithMs))
            .arg(type2string(type))
            .arg(message)
            .arg(QFileInfo(context.file).fileName())
            .arg(context.line);
    std::cerr << output.toUtf8().constData() << std::endl;
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(&messagehandler);
    ela_register(ElaQt::backend());

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QObject::connect(&app, &QCoreApplication::aboutToQuit, &app, []() {
        qDebug() << "aboutToQuit()";
    });

    quint16 port = 24322;
    const auto addr = "192.168.1.24";
    QHostAddress haddr(addr);
    qDebug() << haddr.toString() << haddr.toIPv4Address();
    in_addr iaddr { haddr.toIPv4Address() };

    auto el = ela_create("Qt");

    {
        FoilHid fh(el);




        //mapping_dump();

        //term_input_init(&ks.input_state, input_handler, el);

        //foils_hid_client_connect_ipv4(&client, &iaddr, port);
        fh.connectToHost(QLatin1String(addr), port);


        QQmlApplicationEngine engine;
        engine.rootContext()->setContextProperty("fh", &fh);

        const QUrl url(QStringLiteral("qrc:/main.qml"));

        QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                         &app, [url](QObject * obj, const QUrl & objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
        engine.load(url);

        ela_run(el);
    }
    qDebug() << "close...";
    ela_close(el);

    qDebug() << "closed";
    return 0;
}
