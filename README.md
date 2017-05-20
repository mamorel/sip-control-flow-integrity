# Task 2
## Caller authentication

1. Get the sensitive function list as input (manually specified by the user)
2. Use LLVM built-in CFG analysis features to explore all possible paths to each sensitive function
3. Construct a whitelist of callers chain (stack trace) to sensitive functions
4. Find an efficient way to verify stack traces at runtime
  a) Hash the trace?
  b) Sorted Merkle trees?
5. Redirect the control flow to a response mechanism if the stack trace is malicious

## Evaluation 
* Performance and security evaluation for your tool. 
  * Measure protection time, runtime overhead and binary size differences.
  * For the security evaluation you may use ADT to report on limitation of your protection and list possible attacks.

We highly encourage the usage of reverse engineering tools to implement automated attacks against your protection scheme.