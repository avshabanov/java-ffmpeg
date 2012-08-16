
#include <stdlib.h>

#include <AVNativeBridge.h>

struct BridgeContext {
    int foo;
    int bar;
    int baz;
};

/**
 * Implementation of AVNativeBridge.initInputFileContext
 */
jbyteArray JNICALL Java_com_alexshabanov_mm4j_av_AVNativeBridge_initInputFileContext
(JNIEnv * env, jobject ignored, jstring fileName) {
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
(JNIEnv * env, jobject ignored, jbyteArray jcontext) {
    struct BridgeContext context = { 0 };

    /* though the array can be garbage collected by JVM, here we'd take care about it's context */
    (*env)-> ReleaseByteArrayElements(env, jcontext, (jbyte *) &context, 0);

    /* now we can dispose context.* contents */
}
