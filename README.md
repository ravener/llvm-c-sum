# LLVM C API Sum
A simple Sum function written in C using the [LLVM](https://llvm.org) C API.

The example uses [MCJIT](https://llvm.org/docs/MCJITDesignAndImplementation.html) to compile the function and load it in memory and execute it returning the value back to C.

The C equivalent of this is
```c
int sum(int x, int y) {
  return x + y;
}
```
Which is represented like the following in LLVM IR
```llvm
define i32 @sum(i32 %x, i32 %y) {
entry:
  %results = add i32 %x, %y
  ret i32 %results
}
```
These are further explained below.

An example with the newer OrcJIT is also planned once i get it to work.

## Building
This example should build for most recent LLVM Versions, I use `7.0.1` but anything `>= 5` should probably work.

You will need `llvm-config` in your `PATH`

- Clone the repository (`git clone https://github.com/pollen5/llvm-c-sum`)
- Change directory (`cd llvm-c-sum`)
- Build it (`make`)
- Execute it (`./sum <x> <y>`)

## Explanation
Here we quickly overview the code to help the beginners to LLVM understand the example.

### Modules
The first thing we do is make a module, a module holds functions, types and such.

We create a module with `LLVMModuleCreateWithName(name)`

It can be disposed using `LLVMDisposeModule(module)` but we don't do that here as freeing the ExecutionEngine also frees the module it was made for.

### Functions
Next up we create a function to add to the module using `LLVMAddFunction` adds the function and returns the Value reference to it.

To create a function we first need a type reference to the function information.

We do this using `LLVMFunctionType(ret, args, argc, varargs)`

The arguments are as follows:
- The return type that this function will return.
- Arguments this function takes.
- The Amount of arguments it takes.
- Whether this function takes variadic arguments.

We pass an array of two `LLVMInt32Type()` in args as that is what a sum function needs, 2 integers, ofcourse, and ofcourse returns back an integer so we use the type in there aswell.

### BasicBlocks
Next up after we have our function ready we can put some code in it, we start by making an entry BasicBlock or just block, BasicBlocks can later be used to do conditionals, loops, and so on as you can branch to them with the `br` instruction, however the first block is special which defines the entry point and cannot be branched to, we make a block called `entry` and position our builder at the end of it so anything we build is appended at end of the block.

### Builders
A builder (also known as `IRBuilder` in the C++ namespace) is the builder to build instructions, any instruction you build requires a builder.

Builders leaves the output value into an optionaly named [SSA Value](https://en.m.wikipedia.org/wiki/Static_single_assignment_form)

Builders also do constant folding behind the scenes when possible, that means if you tried to build a `5 + 2` the builder will fold it to a `7` rightaway preventing extra overhead at runtime, this however is not possible for dynamic values such as ones we pass through arguments like in our sum function in this case, but it's a nice thing to know.

In our code we get the function parameters using `LLVMGetParam(fn, index)` (index starts at 0) then use its value in the builder to build an `add` instruction that is stored in the local `%results` then the next build call builds a `ret` (return) to return the value to the caller.

### Verification & Dumping
Next up after our function is ready and added to the module we use `LLVMVerifyModule` to verify the module is well-formed if not it will abort for us and print the problem, this is very useful to find bugs early.

Then after we know it is valid module we dump it, (actually sometimes you may wanna dump before verify so you can really see where's the problem.) the dump prints the whole generated IR into stderr so we can see what it made.

The dump will look something like
```llvm
; ModuleID = 'main'
source_filename = "main"

define i32 @sum(i32 %x, i32 %y) {
entry:
  %results = add i32 %x, %y
  ret i32 %results
}
```
I've also pasted the dump output to the file [sum.ll](sum.ll) for reference.

### MCJIT & ExecutionEngine
Now that our module and everything is ready it's time to execute it.

We first initialize a few things
```c
LLVMLinkInMCJIT();
LLVMInitializeNativeTarget();
LLVMInitializeNativeAsmPrinter();
LLVMInitializeNativeAsmParser();
```
The names are pretty self explanatory on what they do.

Next up we fire up an execution engine for our module using `LLVMCreateExecutionEngineForModule(engine, module, err)`

The initialized engine will be written to the out parameter `engine` and if any error happened it will be written to `err`

> Note: Error Strings in LLVM has to be freed using `LLVMDisposeMessage(str)`

Now that we have our engine ready let's execute the function we made

```c
int (*sum)(int, int) = (int (*)(int, int)) LLVMGetFunctionAddress(engine, "sum");
```
This line will trigger compilation of the function `sum` and return its address

Then we cast that address to a C function pointer so we can execute it.

We execute it like any regular C function, `sum(x, y)` the `x`/`y` arguments was taken from the command line arguments.

Then after printing the output we do some cleanup, disposing the engine and the builder and returning with a classic `return 0;` from our C code and we are done!

## Contributing
Contributions to improving code quality or improving this README are welcome, if you have any questions please feel free to open an issue.

## License
[MIT](LICENSE)
