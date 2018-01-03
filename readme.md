Notes:

 - setters return an integer (0 on success, > 0 on failure).
   - We can either keep these vague (i.e. "Your last command failed"), or get specific.
 - We should use namespaces excessively.
 - Get local versions of all the req libs; we want to be independent of build systems
 - Assume all incoming communication is big-endian, and send all outgoing communication as big-endian. 
   - Note that the teensy is little-endian.
