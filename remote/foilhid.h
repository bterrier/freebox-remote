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
    enum ConsumerCode : quint32 {
        HID_CONSUMER_SUB_CHANNEL_INCREMENT = 0x171,
        HID_CONSUMER_ALTERNATE_AUDIO_INCREMENT = 0x173,
        HID_CONSUMER_ALTERNATE_SUBTITLE_INCREMENT = 0x175,
        HID_CONSUMER_CHANNEL_INCREMENT = 0x9c,
        HID_CONSUMER_CHANNEL_DECREMENT = 0x9d,
        HID_CONSUMER_PLAY = 0xb0,
        HID_CONSUMER_PAUSE = 0xb1,
        HID_CONSUMER_RECORD = 0xb2,
        HID_CONSUMER_FAST_FORWARD = 0xb3,
        HID_CONSUMER_REWIND = 0xb4,
        HID_CONSUMER_SCAN_NEXT_TRACK = 0xb5,
        HID_CONSUMER_SCAN_PREVIOUS_TRACK = 0xb6,
        HID_CONSUMER_STOP = 0xb7,
        HID_CONSUMER_EJECT = 0xb8,
        HID_CONSUMER_MUTE = 0xe2,
        HID_CONSUMER_VOLUME_INCREMENT = 0xe9,
        HID_CONSUMER_VOLUME_DECREMENT = 0xea,
        HID_CONSUMER_RANDOM_PLAY = 0xb9,

        HID_CONSUMER_AC_ZOOM_IN = 0x22d,
        HID_CONSUMER_AC_ZOOM_OUT = 0x22e,

        HID_CONSUMER_AC_SEARCH = 0x221,
        HID_CONSUMER_AC_PROPERTIES = 0x209,
        HID_CONSUMER_AC_EXIT = 0x204,

        HID_CONSUMER_AL_TASK_MANAGER = 0x18f,
        HID_CONSUMER_AL_INTERNET_BROWSER = 0x196,
        HID_CONSUMER_AL_AUDIO_BROWSER = 0x1b7
    };
    Q_ENUM(ConsumerCode)

    enum KeyboardCode : quint32 {
        HID_KEYBOARD_A = 0x04,
        HID_KEYBOARD_ENTER = 0x28,
        HID_KEYBOARD_BACKSPACE = 0x2A,
        HID_KEYBOARD_TAB = 0x2B,
        HID_KEYBOARD_RIGHTARROW = 0x4F,
        HID_KEYBOARD_LEFTARROW = 0x50,
        HID_KEYBOARD_DOWNARROW = 0x51,
        HID_KEYBOARD_UPARROW = 0x52,
        HID_KEYBOARD_HOME = 0x4A,
        HID_KEYBOARD_POWER = 0x66,
        HID_KEYBOARD_F1 = 0x3A,
    };
    Q_ENUM(KeyboardCode)

    explicit FoilHid(ela_el *el, QObject *parent = nullptr);
    ~FoilHid() override;
    bool connectToHost(const QLatin1String &hostname, quint16 port);
    Q_INVOKABLE void send();
    foils_hid_state state() const
    {
        return m_state;
    }

    Q_INVOKABLE static QString stateString(foils_hid_state state);

    void send_unicode(uint32_t code);
    void send_kbd(uint32_t _code);
    void send_cons(uint32_t _code);
    void send_sysctl(uint32_t _code);
public slots:

    void sendKeyboard(KeyboardCode code);
    void sendConsumer(ConsumerCode code);

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
