#ifndef SUNSHINE_CAPNREACTIVE_H
#define SUNSHINE_CAPNREACTIVE_H

#ifdef CAPNREACTIVE_ENABLED

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CSunshineClient CSunshineClient;

typedef enum CSunshineErrorCode {
    SUNSHINE_SUCCESS = 0,
    SUNSHINE_INVALID_PARAMETER = -1,
    SUNSHINE_TRANSPORT_ERROR = -2,
    SUNSHINE_STREAM_ERROR = -3,
    SUNSHINE_RUNTIME_ERROR = -4,
    SUNSHINE_UNKNOWN_ERROR = -99
} CSunshineErrorCode;

CSunshineClient* sunshine_capnreactive_create(void);
int sunshine_capnreactive_init_transport(CSunshineClient* client, const char* server_addr);
void sunshine_capnreactive_destroy(CSunshineClient* client);

int sunshine_capnreactive_start_video(CSunshineClient* client, unsigned int width, unsigned int height, unsigned int fps);
int sunshine_capnreactive_start_audio(CSunshineClient* client, unsigned int sample_rate, unsigned int channels);
int sunshine_capnreactive_init_input(CSunshineClient* client);

int sunshine_capnreactive_send_video_frame(CSunshineClient* client, const void* frame_data, unsigned int frame_size, unsigned int timestamp);
int sunshine_capnreactive_send_audio_data(CSunshineClient* client, const void* audio_data, unsigned int data_size, unsigned int timestamp);
int sunshine_capnreactive_send_mouse_input(CSunshineClient* client, int x, int y, unsigned int buttons);
int sunshine_capnreactive_send_keyboard_input(CSunshineClient* client, unsigned int key_code, int pressed);

int sunshine_capnreactive_get_video_stats(CSunshineClient* client, unsigned int* frames_sent, unsigned int* width, unsigned int* height, unsigned int* fps);

#ifdef __cplusplus
}
#endif

#endif // CAPNREACTIVE_ENABLED

#endif // SUNSHINE_CAPNREACTIVE_H
