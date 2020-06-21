#ifndef FOILHID_H
#define FOILHID_H

#include <QObject>


extern "C" {
#include <ela/ela.h>

#include <foils/hid.h>
}
Q_DECLARE_METATYPE(foils_hid_state)
class FoilHid : public QObject
{
    Q_OBJECT
    Q_PROPERTY(foils_hid_state state READ state NOTIFY stateChanged)
public:
    explicit FoilHid(ela_el *el, QObject *parent = nullptr);
    ~FoilHid() override;
    bool connectToHost(const QLatin1String &hostname, quint16 port);
    Q_INVOKABLE void send();
    foils_hid_state state() const
    {
        return m_state;
    }

    Q_INVOKABLE static QString stateString(foils_hid_state state);

public slots:

signals:

    void stateChanged(foils_hid_state state);

private:
    static QHash<foils_hid *, FoilHid *> s_instances;

    static void onStatusChanged(foils_hid *client, foils_hid_state state);

    void setState(foils_hid_state state)
    {
        if (m_state == state)
            return;

        m_state = state;
        emit stateChanged(m_state);
    }
    foils_hid m_client;

    foils_hid_state m_state;
};

#endif // FOILHID_H
