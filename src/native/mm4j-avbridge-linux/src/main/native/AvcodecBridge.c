
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libavcodec/avcodec.h>

#include "AVNatives.h"


#define JAVA_CODEC_ID_MP3       (100)

static int toAvcodecCodecId(int javaCodecId) {
    switch (javaCodecId) {
        case JAVA_CODEC_ID_MP3: return CODEC_ID_MP3;

        default:
            return CODEC_ID_NONE;
    }
}


/*
 * AvcodecBridge.initialize
 */
void JNICALL Java_com_alexshabanov_mm4j_av_AvcodecBridge_initialize
(JNIEnv * env, jclass ignored) {
    avcodec_register_all();
}


/*
 * AvcodecBridge.findEncoder
 */
jlong JNICALL Java_com_alexshabanov_mm4j_av_AvcodecBridge_findEncoder
(JNIEnv * env, jclass ignored, jint javaCodecId) {
    int codecId = toAvcodecCodecId(javaCodecId);
    AVCodec * codec;

    if (codecId == CODEC_ID_NONE) {
        fprintf(stderr, "There is no registered java codec ID = %d\n", javaCodecId);
        return 0;
    }

    codec = avcodec_find_encoder(codecId);
    fprintf(stderr, "[AvcodecBridge] findEncoder: codec = %p\n", (void *) codec);

    return (jlong) codec;
}

/*
 * AvcodecBridge.closeAndFreeContext
 */
void JNICALL Java_com_alexshabanov_mm4j_av_AvcodecBridge_closeAndFreeContext
(JNIEnv * env, jclass ignored, jlong contextPtr) {
    AVCodecContext * context = (AVCodecContext *) contextPtr;
    avcodec_close(context);
    av_free(context);
}

/*
 * AvcodecBridge.allocAudioContext
 */
jlong JNICALL Java_com_alexshabanov_mm4j_av_AvcodecBridge_allocAudioContext
(JNIEnv * env, jclass ignored, jlong codecPtr, jint bitRate, jint sampleRate, jint channels) {
    AVCodec * codec = (AVCodec *) codecPtr;
    AVCodecContext * context = avcodec_alloc_context3(codec);

    fprintf(stderr, "[AvcodecBridge] allocAudioContext: codec = %p, context = %p\n", (void *) codec, (void *) context);

    context->bit_rate = bitRate;
    context->sample_rate = sampleRate;
    context->channels = channels;
    context->sample_fmt = AV_SAMPLE_FMT_S16;

    return (jlong) context;
}

/*
 * AvcodecBridge.codecOpen
 */
jint JNICALL Java_com_alexshabanov_mm4j_av_AvcodecBridge_codecOpen
(JNIEnv * env, jclass ignored, jlong contextPtr, jlong codecPtr) {
    AVCodecContext * context = (AVCodecContext *) contextPtr;
    AVCodec * codec = (AVCodec *) codecPtr;
    jint result;

    fprintf(stderr, "[AvcodecBridge] codecOpen: codec = %p, context = %p\n", (void *) codec, (void *) context);

    //result = avcodec_open2(context, codec, NULL);
    result = avcodec_open(context, codec);
    fprintf(stderr, "[AvcodecBridge] codecOpen: result = %d, context->frame_size = %d\n", result, context->frame_size);

    return result;
}

#define J_SET_INT_FIELD(env, clazz, obj, fieldName, fieldValue) \
    { \
        jfieldID fieldID = (*env)->GetFieldID(env, clazz, fieldName, "I"); \
        if (fieldID == NULL) { \
            fprintf(stderr, "There is no field '" fieldName "' in %p\n", (void *) obj); \
            return; \
        } \
        (* env)->SetIntField(env, obj, fieldID, fieldValue); \
    }

/*
 * AvcodecBridge.fetchCodecContextParameters
 */
void JNICALL Java_com_alexshabanov_mm4j_av_AvcodecBridge_fetchCodecContextParameters
(JNIEnv * env, jclass ignored, jlong contextPtr, jobject params) {
    AVCodecContext * context = (AVCodecContext *) contextPtr;
    jclass paramsClass = (*env)->GetObjectClass(env, params);

    fprintf(stderr, "[AvcodecBridge] fetchCodecContextParameters: context = %p\n", (void *) context);

    if (paramsClass == NULL) {
        fprintf(stderr, "Can't find params class of %p\n", (void *) params);
        return;
    }

    fprintf(stderr, "[AvcodecBridge] context->frame_size = %d\n",
        context->frame_size);

    J_SET_INT_FIELD(env, paramsClass, params, "frameSize", context->frame_size);
}

/*
 * AvcodecBridge.encodeAudio
 */
jint JNICALL Java_com_alexshabanov_mm4j_av_AvcodecBridge_encodeAudio
(JNIEnv * env, jclass ignored, jlong contextPtr, jobject outputBuffer, jint outputBufferSize, jobject inputBuffer) {
    jbyte * outbuf = (*env)->GetDirectBufferAddress(env, outputBuffer);
    jbyte * inbuf = (*env)->GetDirectBufferAddress(env, inputBuffer);
    AVCodecContext * context = (AVCodecContext *) contextPtr;
    int encodedBytes;

    fprintf(stderr, "[AvcodecBridge] encodeAudio: context = %p, outputBufferSize = %d, outbuf = %p, inbuf = %p\n",
        (void *) context,
        outputBufferSize,
        (void *) outbuf,
        (void *) inbuf);

    encodedBytes = avcodec_encode_audio(context, (uint8_t *) outbuf, outputBufferSize, (short *) inbuf);
    fprintf(stderr, "[AvcodecBridge] encodeAudio: context = %p, encodedBytes = %d\n", (void *) context, encodedBytes);

    return encodedBytes;
}
