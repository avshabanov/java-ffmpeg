package com.alexshabanov.mm4j;

import com.alexshabanov.mm4j.av.AVNativeBridge;

import java.nio.Buffer;
import java.nio.ByteBuffer;
import java.util.Arrays;

/**
 * Entry point - run as follows:
 * mvn clean compile exec:java -Dexec.mainClass=com.alexshabanov.mm4j.App
 *
 * In order to run from Intellij IDEA, set up environment variables
 * LD_LIBRARY_PATH=/home/alex/opt/lib
 */
public final class App {
    public static void main(String[] args) {
        System.out.println("Native API invoke sample #2");

        AVNativeBridge.loadDependencies();

        final byte[] context = AVNativeBridge.initInputFileContext("deus");
        for (final byte b : context) {
            System.out.println("b = " + Integer.toHexString(b & 0x000000ff));
        }

        // this line crushes the application
        AVNativeBridge.disposeFileContext(context);

        System.out.println("EOF init/dispose test");

        final ByteBuffer buffer = ByteBuffer.allocateDirect(18);
        AVNativeBridge.writeSomething(buffer);

        System.out.print("Buffer =");
        for (int i = 0; i < buffer.capacity(); ++i) {
            System.out.print(" " + buffer.get(i));
        }
        System.out.println(";");
    }
}
