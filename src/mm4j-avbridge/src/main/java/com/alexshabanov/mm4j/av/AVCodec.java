package com.alexshabanov.mm4j.av;

/**
 * Draft abstraction over the avcodec.
 */
public final class AVCodec {

    /**
     * Pointer to AVCodec structure
     */
    private long handle;

    private AVCodec(long handle) {
        this.handle = handle;
    }

    long getHandle() {
        return this.handle;
    }

}
