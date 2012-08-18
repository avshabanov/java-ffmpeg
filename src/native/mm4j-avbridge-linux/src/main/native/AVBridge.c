/*
 * Copyright 2012 Alexander Shabanov - http://alexshabanov.com.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libavcodec/avcodec.h>

#include "AVBridge.h"

/**
 * Matches AVBridge.CODEC_ID_MP3
 */
#define JAVA_CODEC_ID_MP3       (100)


/**
 * Helper macro for finding and setting certain integer property of the given java object.
 */
#define J_SET_INT_FIELD(env, clazz, obj, fieldName, fieldValue) \
    { \
        jfieldID fieldID = (*env)->GetFieldID(env, clazz, fieldName, "I"); \
        if (fieldID == NULL) { \
            fprintf(stderr, "There is no field '" fieldName "' in %p\n", (void *) obj); \
            return; \
        } \
        (* env)->SetIntField(env, obj, fieldID, fieldValue); \
    }


/**
 *
 */
static int toAvcodecCodecId(int javaCodecId) {
    switch (javaCodecId) {
        case JAVA_CODEC_ID_MP3: return CODEC_ID_MP3;

        default:
            return CODEC_ID_NONE;
    }
}


/*
 * AVBridge.initialize
 */
void JNICALL Java_com_alexshabanov_mm4j_av_AVBridge_initialize
(JNIEnv * env, jclass ignored) {
    avcodec_register_all();
}


/*
 * AVBridge.findEncoder
 */
jlong JNICALL Java_com_alexshabanov_mm4j_av_AVBridge_findEncoder
(JNIEnv * env, jclass ignored, jint javaCodecId) {
    int codecId = toAvcodecCodecId(javaCodecId);
    AVCodec * codec;

    if (codecId == CODEC_ID_NONE) {
        fprintf(stderr, "There is no registered java codec ID = %d\n", javaCodecId);
        return 0;
    }

    codec = avcodec_find_encoder(codecId);
    fprintf(stderr, "[AVBridge] findEncoder: codec = %p\n", (void *) codec);

    return (jlong) codec;
}

/*
 * AVBridge.closeAndFreeContext
 */
void JNICALL Java_com_alexshabanov_mm4j_av_AVBridge_closeAndFreeContext
(JNIEnv * env, jclass ignored, jlong contextPtr) {
    AVCodecContext * context = (AVCodecContext *) contextPtr;
    avcodec_close(context);
    av_free(context);
}

/*
 * AVBridge.allocAudioContext
 */
jlong JNICALL Java_com_alexshabanov_mm4j_av_AVBridge_allocAudioContext
(JNIEnv * env, jclass ignored, jlong codecPtr, jint bitRate, jint sampleRate, jint channels) {
    AVCodec * codec = (AVCodec *) codecPtr;
    AVCodecContext * context = avcodec_alloc_context3(codec);

    fprintf(stderr, "[AVBridge] allocAudioContext: "
        "codec = %p, context = %p, bitRate = %d, sampleRate = %d, channels = %d\n",
        (void *) codec,
        (void *) context,
        bitRate,
        sampleRate,
        channels);

    context->bit_rate = bitRate;
    context->sample_rate = sampleRate;
    context->channels = channels;
    context->sample_fmt = AV_SAMPLE_FMT_S16;
    if (context->channels == 1) {
        context->channel_layout = AV_CH_LAYOUT_MONO;
    } else {
        context->channel_layout = AV_CH_LAYOUT_STEREO;
    }

    return (jlong) context;
}

/*
 * AVBridge.codecOpen
 */
jint JNICALL Java_com_alexshabanov_mm4j_av_AVBridge_codecOpen
(JNIEnv * env, jclass ignored, jlong contextPtr, jlong codecPtr) {
    AVCodecContext * context = (AVCodecContext *) contextPtr;
    AVCodec * codec = (AVCodec *) codecPtr;
    jint result;

    fprintf(stderr, "[AVBridge] codecOpen: codec = %p, context = %p\n", (void *) codec, (void *) context);

    result = avcodec_open2(context, codec, NULL);
    fprintf(stderr, "[AVBridge] codecOpen: result = %d, context->frame_size = %d\n", result, context->frame_size);

    return result;
}

/*
 * AVBridge.fetchCodecContextParameters
 */
void JNICALL Java_com_alexshabanov_mm4j_av_AVBridge_fetchCodecContextParameters
(JNIEnv * env, jclass ignored, jlong contextPtr, jobject params) {
    AVCodecContext * context = (AVCodecContext *) contextPtr;
    jclass paramsClass = (*env)->GetObjectClass(env, params);

    fprintf(stderr, "[AVBridge] fetchCodecContextParameters: context = %p\n", (void *) context);

    if (paramsClass == NULL) {
        fprintf(stderr, "Can't find params class of %p\n", (void *) params);
        return;
    }

    fprintf(stderr, "[AVBridge] context->frame_size = %d\n",
        context->frame_size);

    J_SET_INT_FIELD(env, paramsClass, params, "frameSize", context->frame_size);
}

#if 0
static void dump_hex(const char * name, jbyte * buffer, int size) {
    fprintf(stderr, "Contents of %s: [ ", name);
    for (int i = 0; i < size; ++i) {
        fprintf(stderr, "%02x ", (unsigned char) buffer[i]);
    }
    fputs("]\n", stderr);
}
#endif

/*
 * AVBridge.encodeAudio
 */
jint JNICALL Java_com_alexshabanov_mm4j_av_AVBridge_encodeAudio
(JNIEnv * env, jclass ignored, jlong contextPtr, jobject outputBuffer, jint outputBufferSize, jobject inputBuffer) {
    jbyte * outbuf = (*env)->GetDirectBufferAddress(env, outputBuffer);
    jbyte * inbuf = (*env)->GetDirectBufferAddress(env, inputBuffer);
    AVCodecContext * context = (AVCodecContext *) contextPtr;
    int encodedBytes;

#if 0
    fprintf(stderr, "[AVBridge] encodeAudio: context = %p, outputBufferSize = %d, outbuf = %p, inbuf = %p\n",
        (void *) context,
        outputBufferSize,
        (void *) outbuf,
        (void *) inbuf);

    dump_hex("inbuf", inbuf, 18);
    dump_hex("outbuf", outbuf, 18);
#endif

    encodedBytes = avcodec_encode_audio(context, (uint8_t *) outbuf, outputBufferSize, (short *) inbuf);

#if 0
    fprintf(stderr, "[AVBridge] encodeAudio: context = %p, encodedBytes = %d\n", (void *) context, encodedBytes);
#endif

    return encodedBytes;
}

jint JNICALL Java_com_alexshabanov_mm4j_av_AVBridge_encodeAlt
(JNIEnv * env, jclass ignored, jobject inputBuffer, jobject outputBuffer) {
    jbyte * outbuf = (*env)->GetDirectBufferAddress(env, outputBuffer);
    jbyte * inbuf = (*env)->GetDirectBufferAddress(env, inputBuffer);

    AVCodec * codec = avcodec_find_encoder(CODEC_ID_MP3);
    AVCodecContext * context = avcodec_alloc_context3(codec);

    context->bit_rate = 64000;
    context->sample_rate = 44100;
    context->channels = 2;
    context->sample_fmt = AV_SAMPLE_FMT_S16; // samples as short[]
    context->channel_layout = AV_CH_LAYOUT_STEREO;

    fputs("Before opening codec\n", stderr);

    if (avcodec_open2(context, codec, NULL) < 0) {
        fputs("Can't open codec\n", stderr);
        return;
    }

    int totalSamplesByteSize = context->frame_size * sizeof(short) * context->channels;
    fprintf(stderr, "totalSamplesByteSize = %d\n", totalSamplesByteSize);

    for (int j = 0; j < 15; ++j) {
        for (int i = 0; i < totalSamplesByteSize; ++i) {
            inbuf[i] = 0x51;
        }

        int encoded = avcodec_encode_audio(context, (uint8_t *) outbuf, 10000, (short *) inbuf);
        fprintf(stderr, "encodedBytes = %d\n", encoded);
    }

    avcodec_close(context);
    av_free(context);
}
