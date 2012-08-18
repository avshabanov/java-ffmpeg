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

package com.alexshabanov.mm4j;

import com.alexshabanov.mm4j.av.AVBridge;

import java.nio.ByteBuffer;

/**
 * To run:
 * mvn clean compile exec:java -Dexec.mainClass=com.alexshabanov.mm4j.AVBridgeSampleApp
 *
 * In order to run from Intellij IDEA, set up environment variables
 * LD_LIBRARY_PATH=/home/alex/opt/lib
 */
public final class AVBridgeSampleApp {

    public static void main(String[] args) {
        System.out.println("Native API invoke sample #2");

        AVBridge.loadDependencies();

        AVBridge.initialize();
        AVBridge.sampleUsage();
    }
}
