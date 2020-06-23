#include "foilhid.h"

#include <QTimer>

#include <QDebug>

static quint8 remote_descriptor[] = {
    0x05, 0x0C,       /* Usage Page (Consumer),                    */
    0x09, 0x01,       /* Usage (Consumer Control),                 */
    0xA1, 0x01,       /* Collection (Application),                 */

    /* 1 "Consumer" input item */
    0x95, 0x01,       /*  Report Count (1),                        */
    0x75, 0x10,       /*  Report Size (16),                        */
    0x19, 0x00,       /*  Usage Minimum (Consumer Control),        */
    0x2A, 0x9C, 0x02, /*  Usage Maximum (AC Distribute Vertically),*/
    0x15, 0x00,       /*  Logical Minimum (1),                     */
    0x26, 0x8C, 0x02, /*  Logical Maximum (652),                   */
    0x80,             /*  Input,                                   */

    0xC0,             /* End Collection,                           */
};

struct remote_report {
    uint16_t consumer;
};


static
void feature_report(
    struct foils_hid *client,
    uint32_t device_id, uint8_t report_id,
    const void *data, size_t datalen)
{
    qDebug() << Q_FUNC_INFO << device_id << report_id;
}

static
void output_report(
    struct foils_hid *client,
    uint32_t device_id, uint8_t report_id,
    const void *data, size_t datalen)
{
    qDebug() << Q_FUNC_INFO << device_id << report_id;
}

static
void feature_report_sollicit(
    struct foils_hid *client,
    uint32_t device_id, uint8_t report_id)
{
    qDebug() << Q_FUNC_INFO << device_id << report_id;
}



struct unicode_state;

typedef void code_sender_t(struct unicode_state *ks, uint32_t code);

struct unicode_state {
    //struct term_input_state input_state;
    struct ela_el *el;
    struct foils_hid *client;
    struct ela_event_source *release;
    code_sender_t *release_sender;
    uint32_t current_code;
    uint32_t release_code;
};


static const uint8_t unicode_report_descriptor[] = {
    0x05, 0x01,         /*  Usage Page (Desktop),               */
    0x09, 0x06,         /*  Usage (Keyboard),                   */

    0xA1, 0x01,         /*  Collection (Application),           */
    0x85, 0x01,         /*      Report ID (1),                  */
    0x05, 0x10,         /*      Usage Page (Unicode),           */
    0x08,               /*      Usage (00h),                    */
    0x95, 0x01,         /*      Report Count (1),               */
    0x75, 0x20,         /*      Report Size (32),               */
    0x14,               /*      Logical Minimum (0),            */
    0x27, 0xFF, 0xFF, 0xFF,  /*      Logical Maximum (2**24-1), */
    0x81, 0x62,         /*      Input (Variable, No pref state, No Null Pos),  */
    0xC0,               /*  End Collection                      */

    0xA1, 0x01,         /*  Collection (Application),           */
    0x85, 0x02,         /*      Report ID (2),                  */
    0x95, 0x01,         /*      Report Count (1),               */
    0x75, 0x08,         /*      Report Size (8),                */
    0x15, 0x00,         /*      Logical Minimum (0),            */
    0x26, 0xFF, 0x00,   /*      Logical Maximum (255),          */
    0x05, 0x07,         /*      Usage Page (Keyboard),          */
    0x19, 0x00,         /*      Usage Minimum (None),           */
    0x2A, 0xFF, 0x00,   /*      Usage Maximum (FFh),            */
    0x80,               /*      Input,                          */
    0xC0,               /*  End Collection                      */

    0x05, 0x0C,         /* Usage Page (Consumer),               */
    0x09, 0x01,         /* Usage (Consumer Control),            */
    0xA1, 0x01,         /* Collection (Application),            */
    0x85, 0x03,         /*  Report ID (3),                      */
    0x95, 0x01,         /*  Report Count (1),                   */
    0x75, 0x10,         /*  Report Size (16),                   */
    0x19, 0x00,         /*  Usage Minimum (Consumer Control),   */
    0x2A, 0x8C, 0x02,   /*  Usage Maximum (AC Send),            */
    0x15, 0x00,         /*  Logical Minimum (0),                */
    0x26, 0x8C, 0x02,   /*  Logical Maximum (652),              */
    0x80,               /*  Input,                              */
    0xC0,               /* End Collection,                      */

    0x05, 0x01,         /* Usage Page (Desktop),                */
    0x0a, 0x80, 0x00,   /* Usage (System Control),              */
    0xA1, 0x01,         /* Collection (Application),            */
    0x85, 0x04,         /*  Report ID (4),                      */
    0x75, 0x01,         /*  Report Size (1),                    */
    0x95, 0x04,         /*  Report Count (4),                   */
    0x1a, 0x81, 0x00,   /*  Usage Minimum (System Power Down),  */
    0x2a, 0x84, 0x00,   /*  Usage Maximum (System Context menu),*/
    0x81, 0x02,         /*  Input (Variable),                   */
    0x75, 0x01,         /*  Report Size (1),                    */
    0x95, 0x04,         /*  Report Count (4),                   */
    0x81, 0x01,         /*  Input (Constant),                   */
    0xC0,               /* End Collection,                      */
};

static const
struct foils_hid_device_descriptor descriptors[] = {
    {
        "Unicode", 0x0100,
        (void *)unicode_report_descriptor, sizeof(unicode_report_descriptor),
        NULL, 0, NULL, 0
    },
};


QHash<foils_hid *, FoilHid *> FoilHid::s_instances;


FoilHid::FoilHid(ela_el *el, QObject *parent) : QObject(parent)
{
    static const int id = qRegisterMetaType<foils_hid_state>();
    static const struct foils_hid_handler handler = {
        .status = &FoilHid::onStatusChanged,
        .feature_report = feature_report,
        .output_report = output_report,
        .feature_report_sollicit = feature_report_sollicit,
    };
    s_instances.insert(&m_client, this);
    int err = foils_hid_init(&m_client, el, &handler, descriptors, 2);

    if (err) {
        qFatal("Error creating client: %s\n", strerror(err));
    }
}

FoilHid::~FoilHid()
{
    qDebug() << Q_FUNC_INFO;
    s_instances.remove(&m_client);
    foils_hid_deinit(&m_client);
}

bool FoilHid::connectToHost(const QLatin1String &hostname, quint16 port)
{
    const int err = foils_hid_client_connect_hostname(&m_client, hostname.data(), port, 0);

    if (err) {
        qCritical() << "Error:" << err;
    }

    foils_hid_device_enable(&m_client, 0);
    //foils_hid_device_enable(&m_client, 1);
    return err == 0;
}

void FoilHid::send()
{
    QTimer::singleShot(2000, this, [&]() {
        qDebug() << "SEND";


        uint8_t code = 0x66;
        foils_hid_input_report_send(&m_client, 0, 2, 1, &code, sizeof(code));
        //remote_report.consumer = 0xe2;
        //remote_report.consumer = 0x66;

        //foils_hid_input_report_send(&m_client, 0, 0, 0, &remote_report, sizeof(remote_report));
    });

}

QString FoilHid::stateString(foils_hid_state state)
{
    QString st;

    switch (state) {
    case FOILS_HID_IDLE:
        st = "idle";
        break;

    case FOILS_HID_CONNECTING:
        st = "connecting";
        break;

    case FOILS_HID_CONNECTED:
        st = "connected";
        break;

    case FOILS_HID_RESOLVE_FAILED:
        st = "resolve failed";
        break;

    case FOILS_HID_DROPPED:
        st = "dropped";
        break;
    }

    return st;
}

void FoilHid::onStatusChanged(foils_hid *client, foils_hid_state state)
{

    auto ptr = s_instances.value(client, nullptr);

    if (ptr) {
        ptr->setState(state);
    }
}

void FoilHid::send_unicode(uint32_t code)
{
    foils_hid_input_report_send(&m_client, 0, 1, 1, &code, sizeof(code));
}

void FoilHid::send_kbd(uint32_t _code)
{
    uint8_t code = _code;
    foils_hid_input_report_send(&m_client, 0, 2, 1, &code, sizeof(code));
}

void FoilHid::send_cons(uint32_t _code)
{
    uint16_t code = _code;
    foils_hid_input_report_send(&m_client, 0, 3, 1, &code, sizeof(code));
}

void FoilHid::send_sysctl(uint32_t _code)
{
    uint8_t code = _code;
    foils_hid_input_report_send(&m_client, 0, 4, 1, &code, sizeof(code));
}

void FoilHid::sendKeyboard(KeyboardCode code)
{
    send_kbd(code);
    QTimer::singleShot(1, this, [this]() {
        send_kbd(0);
    });
}

void FoilHid::sendConsumer(ConsumerCode code)
{
    send_cons(code);
    QTimer::singleShot(1, this, [this]() {
        send_cons(0);
    });
}

void FoilHid::send(FoilHid::SystemCode code)
{
    send_sysctl(code);
    QTimer::singleShot(1, this, [this]() {
        send_sysctl(0);
    });
}
