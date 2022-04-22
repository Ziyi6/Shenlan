#将src中所有的源文件都编译成一个叫做myfun的共享库
add_library(myfun SHARED
        baselib.cpp
        conv.cpp
)
