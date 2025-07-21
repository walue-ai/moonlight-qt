#pragma once

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <memory>

// Forward declarations for CapnReactive FFI
struct CSunshineClient;
enum CSunshineErrorCode : int;

class CapnReactiveClient : public QObject
{
    Q_OBJECT

public:
    explicit CapnReactiveClient(QObject *parent = nullptr);
    ~CapnReactiveClient();

    // Transport detection and negotiation
    bool detectCapnReactiveSupport(const QString& serverAddr);
    void negotiateTransport(const QString& serverAddr);
    bool isCapnReactiveEnabled() const { return m_UseCapnReactive; }
    
    // Connection management
    bool initializeConnection(const QString& serverAddr);
    void closeConnection();
    
    // Streaming setup
    bool startVideoStream(unsigned int width, unsigned int height, unsigned int fps);
    bool startAudioStream(unsigned int sampleRate, unsigned int channels);
    bool initializeInputHandler();
    
    // Data transmission
    bool sendVideoFrame(const void* frameData, unsigned int frameSize, unsigned int timestamp);
    bool sendAudioData(const void* audioData, unsigned int dataSize, unsigned int timestamp);
    bool sendMouseInput(int x, int y, unsigned int buttons);
    bool sendKeyboardInput(unsigned int keyCode, bool pressed);
    
    // Statistics
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
    // Transport detection
    bool testQuicConnection(const QString& serverAddr);
    void establishCapnReactiveConnection(const QString& serverAddr);
    
    // Error handling
    QString errorCodeToString(CSunshineErrorCode code);
    
    // FFI client management
    CSunshineClient* m_CapnReactiveClient;
    QNetworkAccessManager* m_NetworkManager;
    QTimer* m_ConnectionTimer;
    
    // State management
    bool m_Connected;
    bool m_UseCapnReactive;
    QString m_ServerAddress;
    
    // Streaming state
    bool m_VideoStreamActive;
    bool m_AudioStreamActive;
    bool m_InputHandlerActive;
};
