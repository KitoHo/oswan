PATH c:\cygwin1\bin;c:\cygwin1\mipseltools\bin
rem 
make -f makefile clean
make -f makefile 2>&1 |tee err.txt
