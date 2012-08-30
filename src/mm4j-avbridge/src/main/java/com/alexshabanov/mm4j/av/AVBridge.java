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

package com.alexshabanov.mm4j.av;

import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.Buffer;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;

/**
 * Bridge to avcodec functions.
 */
public final class AVBridge {
    private AVBridge() {}

    public static void loadDependencies() {
        try {
            System.loadLibrary("mm4j-avbridge-native");
        } catch (Throwable e) {
            e.printStackTrace();
            System.exit(1);
        }
    }

    public static final int CODEC_ID_MP3 = 100;
    //public static final int CODEC_ID_X264 = 200;
    //public static final int CODEC_ID_XVID = 201;

    public static native void initialize();

    // AVCodec * codec = avcodec_find_encoder(CODEC_ID_*), e.g. CODEC_ID_MP3
    public static native long findEncoder(int codecId);


    // avcodec_close(context)
    // av_free(context)
    public static native void closeAndFreeContext(long contextPtr);


    // AVCodecContext * context = avcodec_alloc_context();
    // context->bit_rate = 64000; - 64kb/s
    // context->sample_rate = 44100; - 44.1 kHz
    // context->channels = 2;
    public static native long allocAudioContext(long codecPtr, int bitRate, int sampleRate, int channels);


    // if (avcodec_open(context, codec) < 0) { return ... }
    public static native int codecOpen(long contextPtr, long codecPtr);


    public static final class CodecContextParameters {
        int frameSize = -1;
    }

    // copies relevant context->* fields into the given parameters instance.
    public static native void fetchCodecContextParameters(long contextPtr, CodecContextParameters outCodecParameters);


    // int encodedSize = avcodec_encode_audio(context, outbuf, outbuf_size, samples);
    public static native int encodeAudio(long contextPtr, Buffer outputBuffer, int outputBufferSize, Buffer samples);

    public static native int encodeAlt(Buffer input, Buffer output);


    //
    // non-native API
    //

    public static final int SIZEOF_SHORT = 2;

    private static void checkNotNull(long ptr, String message) {
        if (ptr == 0) {
            throw new RuntimeException(message);
        }
    }

    private static void checkResult(int result, String message) {
        if (result < 0) {
            throw new RuntimeException(message + "; result = " + result);
        }
    }


    public static void sampleUsage() {
        long contextPtr = 0;

        final int bitRate = 64000; // 64kb/s
        final int sampleRate = 44100; // 44.1 kHz
        final int channels = 2;

        try {
            final long codecPtr = findEncoder(CODEC_ID_MP3);
            checkNotNull(codecPtr, "Can't find mp3 encoder");

            contextPtr = allocAudioContext(codecPtr, bitRate, sampleRate, channels);

            checkResult(codecOpen(contextPtr, codecPtr), "Can't open codec in the given context");

            final CodecContextParameters parameters = new CodecContextParameters();
            fetchCodecContextParameters(contextPtr, parameters);

            final int samplesByteSize = parameters.frameSize * SIZEOF_SHORT * channels;
            final ByteBuffer samples = ByteBuffer.allocateDirect(samplesByteSize);

            final int outbufSize = 100000;
            final ByteBuffer outbuf = ByteBuffer.allocateDirect(outbufSize);

            // encode a single tone sound
            encodeSingleToneSound(new EncodingContext(channels, parameters.frameSize, sampleRate,
                    outbuf, samples, contextPtr));
        } finally {
            closeAndFreeContext(contextPtr);
        }
    }

    private static final class EncodingContext {
        final long contextPtr;
        final ByteBuffer input;
        final ByteBuffer output;
        final int sampleRate;
        final int frameSize;
        final int channels;

        private EncodingContext(int channels, int frameSize, int sampleRate, ByteBuffer output, ByteBuffer input, long contextPtr) {
            this.channels = channels;
            this.frameSize = frameSize;
            this.sampleRate = sampleRate;
            this.output = output;
            this.input = input;
            this.contextPtr = contextPtr;
        }
    }

    private static void encodeSingleToneSound(final EncodingContext context) {
        withFileChannel("/tmp/singletone.mp3", new ResourceCallback<FileChannel>() {
            @Override
            public void withResource(FileChannel channel) {
                try {
                    doEncodeTone(channel, context);
                } catch (IOException e) {
                    throw new RuntimeException(e);
                }
            }
        });
    }

    private static void doEncodeTone(FileChannel channel, EncodingContext context) throws IOException {
        double t = 0.0;
        final double tincr = 2.0 * Math.PI * 440.0 / context.sampleRate;

        for (int i = 0; i < 2000; ++i) {

            for (int j = 0; j < context.frameSize; ++j) {
                final short s = (short) (Math.sin(t) * 10000.0);
                context.input
                        .putShort(s)
                        .putShort(s);

                t += tincr;
            }

            // rewind input buffer to inital mark
            context.input.rewind();

            // encode input to output
            final int outSize = encodeAudio(context.contextPtr,
                    context.output, context.output.capacity(), context.input);
            if (outSize < 0) {
                System.out.println("!!!ERROR WHILE ENCODING!!!");
                return;
            }

            // outSize == 0 means that codec caches encoded data
            if (outSize > 0) {
                channel.write((ByteBuffer) context.output.slice().limit(outSize));
            }
        }
    }




    public interface ResourceCallback<T> {
        void withResource(T resource);
    }

    public static void withFileChannel(String path, ResourceCallback<FileChannel> resourceCallback) {
        try {
            final FileOutputStream fos = new FileOutputStream(path);
            try {
                final FileChannel channel = fos.getChannel();
                try {
                    resourceCallback.withResource(channel);
                } finally {
                    channel.close();
                }
            } finally {
                fos.close();
            }

        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
}
