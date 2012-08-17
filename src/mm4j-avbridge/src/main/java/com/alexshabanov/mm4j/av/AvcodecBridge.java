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

/**
 * Bridge to avcodec functions.
 * TODO: direct mapping of byte array!
 */
public final class AvcodecBridge {
    private AvcodecBridge() {}

    private static native long findEncoder(String name);

    private static native long allocAudioContext(int bitRate, int sampleRate, int channels);

    private static native int codecOpen(long contextPtr, long codecPtr);

    private static native byte[] getContextBuffer(long contextPtr);
}
