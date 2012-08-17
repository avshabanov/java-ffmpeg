
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AVNatives.h"

struct BridgeContext {
    int foo;
    int bar;
    int baz;
};

/**
 * Implementation of AVNativeBridge.initInputFileContext
 */
jbyteArray JNICALL Java_com_alexshabanov_mm4j_av_AVNativeBridge_initInputFileContext
(JNIEnv * env, jclass ignored, jstring fileName) {
    jbyteArray result = (*env)->NewByteArray(env, sizeof(struct BridgeContext));
    struct BridgeContext context;

    context.foo = 0x11112222;
    context.bar = 0x33334444;
    context.baz = 0x55556666;

    (*env)->SetByteArrayRegion(env, result, 0, sizeof(struct BridgeContext), (jbyte *) &context);
    return result;
}

/**
 * Implementation of AVNativeBridge.disposeFileContext
 */
void JNICALL Java_com_alexshabanov_mm4j_av_AVNativeBridge_disposeFileContext
(JNIEnv * env, jclass ignored, jbyteArray jcontext) {
    struct BridgeContext * context;

    /* The array can be garbage collected by JVM, here we'd take care about it's context */
    context = (struct BridgeContext *) (*env)->GetByteArrayElements(env, jcontext, 0);

    /* now we can dispose context.* contents */
    fprintf(stderr, "C -> Disposing context{%X, %X, %X}\n", context->foo, context->bar, context->baz);
}

void JNICALL Java_com_alexshabanov_mm4j_av_AVNativeBridge_writeSomething
(JNIEnv * env, jclass ignored, jobject buffer) {
    jbyte * address = (*env)->GetDirectBufferAddress(env, buffer);
    jlong capacity = (*env)->GetDirectBufferCapacity(env, buffer);
    jlong i;

    if (address == NULL) {
        fprintf(stderr, "Can't get direct buffer address, buffer = %p\n", (void *) buffer);
        return;
    }

    /* writing data */
    for (i = 0; i < capacity; ++i) {
        address[i] = (jbyte) i;
    }
}
