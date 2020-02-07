The program depends on ixwebsocket library to handling the websocket communication with upstream server to get the patient information. The easliest way to make ixwebsocket available is to use vcpkg.


The declaration and implementation can be combined into one .hxx file or saparated into .h and .cpp files. We recommend the previous one for its simplicity. In case of one .hxx file, one shall using following #def blocks to 
1. avoid multiple inclusion of either declaration or implementation
2. explicitly specify implemetation instantiation   

```c++
#ifndef CONSOLE_INCLUDE_UI_H 
#define CONSOLE_INCLUDE_UI_H

... declarations

#endif // CONSOLE_INCLUDE_UI_H

#ifdef CONSOLE_UI_IMPLEMENTATION
#ifndef CONSOLE_UI_IMPLEMENTED
#define CONSOLE_UI_IMPLEMENTED

... implementations

#endif // !CONSOLE_UI_IMPLEMENTED
#endif   // CONSOLE_UI_IMPLEMENTATION
```