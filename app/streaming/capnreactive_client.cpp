#ifdef CAPNREACTIVE_ENABLED

#include "capnreactive_client.h"
#include "sunshine_capnreactive.h"
#include <QDebug>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>

CapnReactiveClient::CapnReactiveClient(QObject *parent) 
    : QObject(parent), 
      m_CapnReactiveClient(nullptr),
      m_NetworkManager(new QNetworkAccessManager(this)),
      m_ConnectionTimer(new QTimer(this)),
      m_Connected(false),
      m_UseCapnReactive(false),
      m_VideoStreamActive(false),
      m_AudioStreamActive(false),
      m_InputHandlerActive(false)
{
    m_ConnectionTimer->setSingleShot(true);
    m_ConnectionTimer->setInterval(5000); // 5 second timeout
    connect(m_ConnectionTimer, &QTimer::timeout, this, &CapnReactiveClient::onConnectionTimeout);
}

CapnReactiveClient::~CapnReactiveClient() {
    closeConnection();
}

bool CapnReactiveClient::detectCapnReactiveSupport(const QString& serverAddr) {
    qDebug() << "Detecting CapnReactive support for server:" << serverAddr;
    
    CSunshineClient* testClient = sunshine_capnreactive_create();
    if (!testClient) {
        qDebug() << "Failed to create CapnReactive test client";
        return false;
    }
    
    QByteArray addrBytes = serverAddr.toUtf8();
    CSunshineErrorCode result = static_cast<CSunshineErrorCode>(
        sunshine_capnreactive_init_transport(testClient, addrBytes.constData())
    );
    
    sunshine_capnreactive_destroy(testClient);
    
    if (result == SUNSHINE_SUCCESS) {
        qDebug() << "CapnReactive transport initialization successful";
        return true;
    } else {
        qDebug() << "CapnReactive transport initialization failed:" << errorCodeToString(result);
        return false;
    }
}

void CapnReactiveClient::negotiateTransport(const QString& serverAddr) {
    m_ServerAddress = serverAddr;
    
    if (detectCapnReactiveSupport(serverAddr)) {
        qDebug() << "Server supports CapnReactive, using enhanced transport";
        m_UseCapnReactive = true;
        establishCapnReactiveConnection(serverAddr);
    } else {
        qDebug() << "Server doesn't support CapnReactive, using standard Limelight";
        m_UseCapnReactive = false;
        emit transportNegotiated(false);
    }
}

bool CapnReactiveClient::initializeConnection(const QString& serverAddr) {
    if (!m_UseCapnReactive) {
        qDebug() << "CapnReactive not enabled, skipping initialization";
        return false;
    }
    
    m_CapnReactiveClient = sunshine_capnreactive_create();
    if (!m_CapnReactiveClient) {
        emit connectionFailed("Failed to create CapnReactive client");
        return false;
    }
    
    QByteArray addrBytes = serverAddr.toUtf8();
    CSunshineErrorCode result = static_cast<CSunshineErrorCode>(
        sunshine_capnreactive_init_transport(m_CapnReactiveClient, addrBytes.constData())
    );
    
    if (result != SUNSHINE_SUCCESS) {
        QString error = QString("Transport initialization failed: %1").arg(errorCodeToString(result));
        emit connectionFailed(error);
        closeConnection();
        return false;
    }
    
    m_Connected = true;
    emit connectionEstablished();
    return true;
}

void CapnReactiveClient::closeConnection() {
    if (m_CapnReactiveClient) {
        sunshine_capnreactive_destroy(m_CapnReactiveClient);
        m_CapnReactiveClient = nullptr;
    }
    
    m_Connected = false;
    m_VideoStreamActive = false;
    m_AudioStreamActive = false;
    m_InputHandlerActive = false;
}

bool CapnReactiveClient::startVideoStream(unsigned int width, unsigned int height, unsigned int fps) {
    if (!m_Connected || !m_CapnReactiveClient) {
        qDebug() << "Cannot start video stream: not connected";
        return false;
    }
    
    CSunshineErrorCode result = static_cast<CSunshineErrorCode>(
        sunshine_capnreactive_start_video(m_CapnReactiveClient, width, height, fps)
    );
    
    if (result == SUNSHINE_SUCCESS) {
        m_VideoStreamActive = true;
        qDebug() << QString("Video stream started: %1x%2@%3fps").arg(width).arg(height).arg(fps);
        return true;
    } else {
        QString error = QString("Failed to start video stream: %1").arg(errorCodeToString(result));
        emit streamingError(error);
        return false;
    }
}

bool CapnReactiveClient::startAudioStream(unsigned int sampleRate, unsigned int channels) {
    if (!m_Connected || !m_CapnReactiveClient) {
        qDebug() << "Cannot start audio stream: not connected";
        return false;
    }
    
    CSunshineErrorCode result = static_cast<CSunshineErrorCode>(
        sunshine_capnreactive_start_audio(m_CapnReactiveClient, sampleRate, channels)
    );
    
    if (result == SUNSHINE_SUCCESS) {
        m_AudioStreamActive = true;
        qDebug() << QString("Audio stream started: %1Hz, %2 channels").arg(sampleRate).arg(channels);
        return true;
    } else {
        QString error = QString("Failed to start audio stream: %1").arg(errorCodeToString(result));
        emit streamingError(error);
        return false;
    }
}

bool CapnReactiveClient::initializeInputHandler() {
    if (!m_Connected || !m_CapnReactiveClient) {
        qDebug() << "Cannot initialize input handler: not connected";
        return false;
    }
    
    CSunshineErrorCode result = static_cast<CSunshineErrorCode>(
        sunshine_capnreactive_init_input(m_CapnReactiveClient)
    );
    
    if (result == SUNSHINE_SUCCESS) {
        m_InputHandlerActive = true;
        qDebug() << "Input handler initialized";
        return true;
    } else {
        QString error = QString("Failed to initialize input handler: %1").arg(errorCodeToString(result));
        emit streamingError(error);
        return false;
    }
}

bool CapnReactiveClient::sendVideoFrame(const void* frameData, unsigned int frameSize, unsigned int timestamp) {
    if (!m_VideoStreamActive || !m_CapnReactiveClient) {
        return false;
    }
    
    CSunshineErrorCode result = static_cast<CSunshineErrorCode>(
        sunshine_capnreactive_send_video_frame(m_CapnReactiveClient, frameData, frameSize, timestamp)
    );
    
    return result == SUNSHINE_SUCCESS;
}

bool CapnReactiveClient::sendAudioData(const void* audioData, unsigned int dataSize, unsigned int timestamp) {
    if (!m_AudioStreamActive || !m_CapnReactiveClient) {
        return false;
    }
    
    CSunshineErrorCode result = static_cast<CSunshineErrorCode>(
        sunshine_capnreactive_send_audio_data(m_CapnReactiveClient, audioData, dataSize, timestamp)
    );
    
    return result == SUNSHINE_SUCCESS;
}

bool CapnReactiveClient::sendMouseInput(int x, int y, unsigned int buttons) {
    if (!m_InputHandlerActive || !m_CapnReactiveClient) {
        return false;
    }
    
    CSunshineErrorCode result = static_cast<CSunshineErrorCode>(
        sunshine_capnreactive_send_mouse_input(m_CapnReactiveClient, x, y, buttons)
    );
    
    return result == SUNSHINE_SUCCESS;
}

bool CapnReactiveClient::sendKeyboardInput(unsigned int keyCode, bool pressed) {
    if (!m_InputHandlerActive || !m_CapnReactiveClient) {
        return false;
    }
    
    CSunshineErrorCode result = static_cast<CSunshineErrorCode>(
        sunshine_capnreactive_send_keyboard_input(m_CapnReactiveClient, keyCode, pressed ? 1 : 0)
    );
    
    return result == SUNSHINE_SUCCESS;
}

CapnReactiveClient::VideoStats CapnReactiveClient::getVideoStats() {
    VideoStats stats = {0, 0, 0, 0};
    
    if (m_VideoStreamActive && m_CapnReactiveClient) {
        CSunshineErrorCode result = static_cast<CSunshineErrorCode>(
            sunshine_capnreactive_get_video_stats(
                m_CapnReactiveClient, 
                &stats.framesSent, 
                &stats.width, 
                &stats.height, 
                &stats.fps
            )
        );
        
        if (result != SUNSHINE_SUCCESS) {
            qDebug() << "Failed to get video stats:" << errorCodeToString(result);
        }
    }
    
    return stats;
}

bool CapnReactiveClient::testQuicConnection(const QString& serverAddr) {
    Q_UNUSED(serverAddr)
    return true; // Assume QUIC is available for now
}

void CapnReactiveClient::establishCapnReactiveConnection(const QString& serverAddr) {
    m_ConnectionTimer->start();
    
    if (initializeConnection(serverAddr)) {
        m_ConnectionTimer->stop();
        emit transportNegotiated(true);
    } else {
        m_ConnectionTimer->stop();
        m_UseCapnReactive = false;
        emit transportNegotiated(false);
    }
}

void CapnReactiveClient::onDetectionFinished() {
    qDebug() << "CapnReactive detection finished";
}

void CapnReactiveClient::onConnectionTimeout() {
    qDebug() << "CapnReactive connection timeout, falling back to standard transport";
    closeConnection();
    m_UseCapnReactive = false;
    emit transportNegotiated(false);
}

QString CapnReactiveClient::errorCodeToString(CSunshineErrorCode code) {
    switch (code) {
        case SUNSHINE_SUCCESS: return "Success";
        case SUNSHINE_INVALID_PARAMETER: return "Invalid Parameter";
        case SUNSHINE_TRANSPORT_ERROR: return "Transport Error";
        case SUNSHINE_STREAM_ERROR: return "Stream Error";
        case SUNSHINE_RUNTIME_ERROR: return "Runtime Error";
        case SUNSHINE_UNKNOWN_ERROR: return "Unknown Error";
        default: return QString("Unknown Error Code: %1").arg(static_cast<int>(code));
    }
}

#endif // CAPNREACTIVE_ENABLED
