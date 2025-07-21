#ifndef CAPNREACTIVE_CLIENT_H
#define CAPNREACTIVE_CLIENT_H

#ifdef CAPNREACTIVE_ENABLED

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <memory>
#include "sunshine_capnreactive.h"

struct CSunshineClient;

class CapnReactiveClient : public QObject
{
    Q_OBJECT

public:
    explicit CapnReactiveClient(QObject *parent = nullptr);
    ~CapnReactiveClient();

    bool detectCapnReactiveSupport(const QString& serverAddr);
    void negotiateTransport(const QString& serverAddr);
    bool isCapnReactiveEnabled() const { return m_UseCapnReactive; }
    
    bool initializeConnection(const QString& serverAddr);
    void closeConnection();
    
    bool startVideoStream(unsigned int width, unsigned int height, unsigned int fps);
    bool startAudioStream(unsigned int sampleRate, unsigned int channels);
    bool initializeInputHandler();
    
    bool sendVideoFrame(const void* frameData, unsigned int frameSize, unsigned int timestamp);
    bool sendAudioData(const void* audioData, unsigned int dataSize, unsigned int timestamp);
    bool sendMouseInput(int x, int y, unsigned int buttons);
    bool sendKeyboardInput(unsigned int keyCode, bool pressed);
    
    struct VideoStats {
        unsigned int framesSent;
        unsigned int width;
        unsigned int height;
        unsigned int fps;
    };
    VideoStats getVideoStats();

signals:
    void transportNegotiated(bool useCapnReactive);
    void connectionEstablished();
    void connectionFailed(const QString& error);
    void streamingStarted();
    void streamingError(const QString& error);

private slots:
    void onDetectionFinished();
    void onConnectionTimeout();

private:
    bool testQuicConnection(const QString& serverAddr);
    void establishCapnReactiveConnection(const QString& serverAddr);
    
    QString errorCodeToString(CSunshineErrorCode code);
    
    CSunshineClient* m_CapnReactiveClient;
    QNetworkAccessManager* m_NetworkManager;
    QTimer* m_ConnectionTimer;
    
    bool m_Connected;
    bool m_UseCapnReactive;
    QString m_ServerAddress;
    
    bool m_VideoStreamActive;
    bool m_AudioStreamActive;
    bool m_InputHandlerActive;
};

#endif // CAPNREACTIVE_ENABLED

#endif // CAPNREACTIVE_CLIENT_H
