In order to launch, do the following:


[Optional - Create Your Common LD dir]
    ln -s ~/.m2/repository/com/alexshabanov/mm4j/mm4j-avbridge-linux/1.0.0-SNAPSHOT/mm4j-avbridge-linux-1.0.0-SNAPSHOT.so libmm4j-avbridge-native.so

NOTICE, the name should be libmm4j-avbridge-native.so, it must be loaded as System.loadLibrary("mm4j-avbridge-native");

In your .bashrc:
    export LD_LIBRARY_PATH=$HOME/opt/lib:$LD_LIBRARY_PATH

Now the library should be accessible by its libmm4j-avbridge-native.so alias (don't forget to do . ~/.bashrc in your shell)




Launch application:
mvn clean compile exec:java -Dexec.mainClass=com.alexshabanov.mm4j.App
