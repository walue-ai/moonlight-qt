#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CSunshineClient CSunshineClient;

typedef enum {
    SUNSHINE_SUCCESS = 0,
    SUNSHINE_INVALID_PARAMETER = -1,
    SUNSHINE_TRANSPORT_ERROR = -2,
    SUNSHINE_STREAM_ERROR = -3,
    SUNSHINE_RUNTIME_ERROR = -4,
    SUNSHINE_UNKNOWN_ERROR = -99
} SunshineErrorCode;

CSunshineClient* sunshine_capnreactive_create(void);
int sunshine_capnreactive_init_transport(CSunshineClient* client, const char* server_addr);
void sunshine_capnreactive_destroy(CSunshineClient* client);

int sunshine_capnreactive_start_video(CSunshineClient* client, unsigned int width, unsigned int height, unsigned int fps);
int sunshine_capnreactive_start_audio(CSunshineClient* client, unsigned int sample_rate, unsigned int channels);
int sunshine_capnreactive_init_input(CSunshineClient* client);

int sunshine_capnreactive_send_video_frame(CSunshineClient* client, const void* frame_data, unsigned int frame_size, unsigned int timestamp);
int sunshine_capnreactive_send_audio_data(CSunshineClient* client, const void* audio_data, unsigned int data_size, unsigned int timestamp);

int sunshine_capnreactive_send_mouse_input(CSunshineClient* client, int x, int y, unsigned char buttons);
int sunshine_capnreactive_send_keyboard_input(CSunshineClient* client, unsigned int key_code, unsigned char pressed);
int sunshine_capnreactive_send_control_command(CSunshineClient* client, unsigned char command_type, const void* data, unsigned int data_size, void* response_buffer, unsigned int* response_size);

int sunshine_capnreactive_get_video_stats(CSunshineClient* client, unsigned long long* frames_sent, unsigned int* width, unsigned int* height, unsigned int* fps);
int sunshine_capnreactive_get_audio_stats(CSunshineClient* client, unsigned long long* packets_sent, unsigned int* sample_rate, unsigned int* channels);
int sunshine_capnreactive_get_input_stats(CSunshineClient* client, unsigned long long* commands_sent);

#ifdef __cplusplus
}
#endif
