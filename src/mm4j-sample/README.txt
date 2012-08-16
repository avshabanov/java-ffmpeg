In order to launch, do the following:


[Optional - Create Your Common LD dir]
    ln -s ~/.m2/repository/com/alexshabanov/mm4j/mm4j-avbridge-linux/1.0.0-SNAPSHOT/mm4j-avbridge-linux-1.0.0-SNAPSHOT.so mm4j-avbridge-linux.so

In your .bashrc:
    export LD_LIBRARY_PATH=$HOME/opt/lib:$LD_LIBRARY_PATH

Now the library should be accessible by its libmm4j-avbridge-linux.so alias

