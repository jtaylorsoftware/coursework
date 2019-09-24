Name
  lss - list directory contents in sorted, long listing format

Syntax
  lss [OPTION] [FILE]
  
Description:
  lss by default lists information about the files in the current 
  directory, in long listing format sorted by decreasing byte count.
  Directories are not descended - files within directories are only
  listed if they are present in the arguments. Any valid options or 
  combinations of options that would work for the builtin LS command 
  are also valid for lss. Calling lss with options is equivalent to 
  calling LS with the specified options and the addition of -lS.
