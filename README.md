The declaration and implementation can be combined into one .hxx file or saparated into .h and .cpp files. We recommend the previous one for its simplicity. In case of one .hxx file, one shall using following #def blocks to 
1. avoid multiple inclusion of either declaration or implementation
2. explicitly specify implemetation instantiation   