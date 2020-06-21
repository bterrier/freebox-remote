#include "elaqt/elaqt.h"

#include <QAbstractEventDispatcher>
#include <QCoreApplication>
#include <QLoggingCategory>
#include <QSocketNotifier>
#include <QTimer>

#include <QDebug>

Q_DECLARE_LOGGING_CATEGORY(elaQt)
Q_LOGGING_CATEGORY(elaQt, "ela.qt", QtDebugMsg)

struct ela_event_source {
    ela_handler_func *func;
    void *priv = nullptr;
    int fd = 0;
    uint32_t flags = 0;

    QSocketNotifier *readNotifier = nullptr;
    QSocketNotifier *writeNotifier = nullptr;
    bool notifyOnce;
    QTimer *timer = nullptr;
    bool timerOnce;
    timeval tv;

    ~ela_event_source()
    {
        delete timer;
        delete writeNotifier;
        delete readNotifier;
    }

    void onEvent(int flag)
    {
        qCDebug(elaQt) << __FUNCTION__ << this << ElaQt::Obj::Flags(flag);

        if (timer) {
            timer->stop();
        }


        if ((flags & ELA_EVENT_TIMEOUT) && !(flags & ELA_EVENT_ONCE)) {
            if (timer)
                timer->start();
        } else {
            if (readNotifier)
                readNotifier->setEnabled(false);

            if (writeNotifier)
                writeNotifier->setEnabled(false);
        }


        func(this, fd, flag, priv);
        qCDebug(elaQt) << __FUNCTION__ << "end";

    }

    void enable()
    {
        qCDebug(elaQt) << __func__ << ElaQt::Obj::Flags(flags) << fd;

        if (flags & ELA_EVENT_READABLE) {
            delete readNotifier;
            readNotifier = new QSocketNotifier(fd, QSocketNotifier::Read);
            qApp->eventDispatcher()->registerSocketNotifier(readNotifier);
            readNotifier->setEnabled(true);


            QObject::connect(readNotifier, &QSocketNotifier::activated, readNotifier, [this]() {
                onEvent(ELA_EVENT_READABLE);
            });
        }


        if (flags & ELA_EVENT_WRITABLE) {
            delete writeNotifier;
            writeNotifier = new QSocketNotifier(fd, QSocketNotifier::Write);
            writeNotifier->setEnabled(true);
            qApp->eventDispatcher()->registerSocketNotifier(writeNotifier);

            QObject::connect(writeNotifier, &QSocketNotifier::activated, writeNotifier, [this]() {
                onEvent(ELA_EVENT_WRITABLE);
            });
        }

        if (flags & ELA_EVENT_TIMEOUT) {
            if (!timer) {
                timer = new QTimer;
                timer->setSingleShot(true);
                QObject::connect(timer, &QTimer::timeout, timer, [this]() {
                    onEvent(ELA_EVENT_TIMEOUT);
                });
            }

            timer->setInterval(tv.tv_sec * 1000 + tv.tv_usec / 1000);
            timer->start();
        }
    }

    void disable()
    {
        if (timer)
            timer->stop();

        if (readNotifier)
            readNotifier->setEnabled(false);

        if (writeNotifier)
            writeNotifier->setEnabled(false);

    }
};


namespace ElaQt
{

ela_el_backend _backend;

class Context : public ela_el
{
public:
    Context()
    {
        backend = ElaQt::backend();
    }
};

ela_el *create()
{
    auto context = new Context();
    return context;
}

void close(struct ela_el *context)
{
    auto c = static_cast<Context *>(context);
    delete c;
}



void run(struct ela_el *context)
{
    qDebug() << Q_FUNC_INFO;
    Q_UNUSED(context);

    qApp->exec();
}

void exit(struct ela_el *context)
{
    Q_UNUSED(context);
    qDebug() << Q_FUNC_INFO;
    qApp->exit();
}

ela_error_t set_fd(
    struct ela_el *context,
    struct ela_event_source *src,
    int fd,
    uint32_t ela_flags)
{
    Q_UNUSED(context)

    qDebug() << Q_FUNC_INFO << src << fd << Obj::Flags(ela_flags);

    src->notifyOnce = ela_flags & ELA_EVENT_ONCE;

    const uint32_t fd_flags
        = (ELA_EVENT_ONCE | ELA_EVENT_READABLE | ELA_EVENT_WRITABLE);

    src->flags = (src->flags & ~fd_flags) | (ela_flags & fd_flags);
    src->fd = fd;

    return 0;
}


/** Allocate an new event source structure. See @ref ela_source_alloc */
ela_error_t source_alloc(
    struct ela_el *context,
    ela_handler_func *func,
    void *priv,
    struct ela_event_source **ret)
{
    Q_UNUSED(context)

    auto source = new ela_event_source;

    qDebug() << Q_FUNC_INFO << source << func << priv;
    source->func = func;
    source->priv = priv;
    *ret = source;

    return 0;
}

/** Release an event source structure. See @ref ela_source_free */
void source_free(
    struct ela_el *context,
    struct ela_event_source *src)
{
    Q_UNUSED(context)

    qDebug() << Q_FUNC_INFO << src;

    delete src;
}


ela_error_t add(struct ela_el *context,
                struct ela_event_source *src)
{

    qDebug() << Q_FUNC_INFO << src;

    src->enable();

    return 0;
}

ela_error_t remove(struct ela_el *context,
                   struct ela_event_source *src)
{

    qDebug() << Q_FUNC_INFO << src;
    src->disable();
    return 0;
}

ela_error_t set_timeout(
    struct ela_el *context,
    struct ela_event_source *src,
    const timeval *tv,
    uint32_t ela_flags)
{

    Q_UNUSED(context)

    qDebug() << Q_FUNC_INFO << src << Obj::Flags(ela_flags);

    constexpr uint32_t timeout_flags = (ELA_EVENT_ONCE | ELA_EVENT_TIMEOUT);


    if (tv) {
        memcpy(&src->tv, tv, sizeof(*tv));
        ela_flags |= ELA_EVENT_TIMEOUT;
        src->flags
            = (src->flags & ~timeout_flags) | (ela_flags & timeout_flags);
    } else {
        src->flags &= ~ELA_EVENT_TIMEOUT;
    }

    return 0;
}




ela_el_backend *backend()
{
    static bool first = true;

    if (first) {
        first = false;

        _backend.name = "Qt";
        _backend.create = &ElaQt::create;
        _backend.close = &ElaQt::close;
        _backend.exit = &ElaQt::exit;
        _backend.run = &ElaQt::run;
        _backend.set_fd = &ElaQt::set_fd;
        _backend.source_alloc = &ElaQt::source_alloc;
        _backend.source_free = &ElaQt::source_free;
        _backend.add = &ElaQt::add;
        _backend.remove = &ElaQt::remove;
        _backend.set_timeout = &ElaQt::set_timeout;
    }

    return &_backend;
}
}

extern "C" ela_el *elaqt_create(int argc, char *argv[]) {

    ela_register(ElaQt::backend());
    QCoreApplication *app = new QCoreApplication(argc, argv);
    auto el = ela_create("Qt");
    return el;
}
