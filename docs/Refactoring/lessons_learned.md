# refactoring - lessons learned

spot singletons
manager classes are usually the once


# classes

pull out classes as a cpp file
    see which dependencies are missing.
    include the file at the place where it was, to make it work

## opengl

### change projection

get calculation for 