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

package com.alexshabanov.mm4j.util;

import java.io.IOException;
import java.io.OutputStream;

/**
 * Non-threadsafe inplace byte array output stream.
 * Works with the provided buffer instance.
 */
public final class InplaceByteOutputStream extends OutputStream {
    private int offset;
    private final byte[] target;

    public InplaceByteOutputStream(byte[] target) {
        if (target == null) {
            throw new NullPointerException("target");
        }

        this.target = target;
    }

    @Override
    public void write(int b) throws IOException {
        target[offset] = (byte) b;
        ++offset;
    }

    public void reset() {
        seek(0);
    }

    public void seek(int offset) {
        this.offset = offset;
    }
}
