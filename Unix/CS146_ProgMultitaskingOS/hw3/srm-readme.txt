Name
  srm - safely remove files

Syntax
  srm [OPTION] [FILE]
  
Description:
  srm is a safe version of rm that moves files to a trash directory 
  rather than deleting them outright. srm depends on an environment
  variable TRASH being set to the path of an existing directory to run
  safe removal (if options are present, rm will run regardless of TRASH
  being set). srm optionally takes command line options - if they are present, 
  the builtin rm is ran with those options (and files may be lost). If given no 
  options, srm will just safely remove files. Safely-removed files are not
  versioned - old removed files will be overwritten by newly removed files.
----------------------------------------------------------------------------------
Name
  unrm - restore safe-removed (srm) files

Syntax
  unrm [FILE]
  
Description:
  unrm restores files that were safely removed by the srm command. unrm 
  requires an environment variable TRASH being set to the path of an existing
  directory to run. Files will be restored from whatever path TRASH refers to
  at runtime. unrm will inform the user if the TRASH directory is empty. 
  unrm takes no command line options. 
----------------------------------------------------------------------------------
Name
  lsrm - list files that have been safe-removed

Syntax
  lsrm 
  
Description:
  lsrm performs a long listing of all files currently in the TRASH directory. 
  An environment variable TRASH must be set to the path of an existing directory
  in order to run lsrm. 
---------------------------------------------------------------------------------- 
Name
  durm - show file sizes of safe-removed files

Syntax
  durm 
  
Description:
  durm runs the builtin du command on the TRASH directory. 
  An environment variable TRASH must be set to the path of an existing directory
  in order to run durm. 
----------------------------------------------------------------------------------
Name
  trash - remove all files that are currently safe-removed

Syntax
  trash 
  
Description:
  trash runs the rm command with options -rf on the TRASH directory, effectively
  removing all previously safe-removed files from disk. trash requires that the 
  environment variable TRASH is set to the path of an existing directory. 
----------------------------------------------------------------------------------