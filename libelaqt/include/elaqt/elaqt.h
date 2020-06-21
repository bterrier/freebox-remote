#ifndef ELAQT_H
#define ELAQT_H

#include <QObject>

extern "C" {
#include <ela/ela.h>
#include <ela/backend.h>
}

namespace ElaQt
{
ela_el_backend *backend();

class Obj: public QObject
{
    Q_OBJECT
public:
    enum Flag {
        Readable = ELA_EVENT_READABLE,
        TimeOut = ELA_EVENT_TIMEOUT,
        Writable = ELA_EVENT_WRITABLE,
        Once = ELA_EVENT_ONCE
    };
    Q_DECLARE_FLAGS(Flags, Flag);
    Q_FLAG(Flag);
};
};

#endif // ELAQT_H
