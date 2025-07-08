使用dl_iterate_phdr，实测AndroidSecurityGuard的FridaDetect可以检测到 /memfd:frida-agent-64.so (deleted)

### `dl_iterate_phdr`

`dl_iterate_phdr` is a function provided by the GNU C Library that allows you to iterate over all shared objects (shared libraries) currently loaded into the process's address space. 

It is commonly used for inspecting loaded libraries, analyzing memory regions, or detecting specific libraries.

#### Function Signature:
```c
int dl_iterate_phdr(int (*callback)(struct dl_phdr_info *info, size_t size, void *data), void *data);
```

#### Parameters:
1. **`callback`**: A function pointer that is called for each shared object. It receives information about the shared object in the form of a `struct dl_phdr_info`.
2. **`data`**: A user-defined pointer passed to the callback function, which can be used to store or pass additional information.

#### `struct dl_phdr_info`:
This structure provides information about a shared object:
- **`dlpi_name`**: The name (path) of the shared object.
- **`dlpi_addr`**: The base address where the shared object is loaded.
- **`dlpi_phdr`**: A pointer to the program headers of the shared object.
- **`dlpi_phnum`**: The number of program headers.

#### Return Value:
- Returns `0` if all shared objects are processed.
- If the callback function returns a non-zero value, `dl_iterate_phdr` stops and returns that value.

#### Example Use Case:
The following example demonstrates how to use `dl_iterate_phdr` to print the names of all shared libraries loaded into the process.

#### Example Code:
```cpp
#include <link.h>
#include <iostream>

int callback(struct dl_phdr_info *info, size_t size, void *data) {
    std::cout << "Library: " << info->dlpi_name << std::endl;
    return 0; // Continue iteration
}

int main() {
    dl_iterate_phdr(callback, nullptr);
    return 0;
}
```

#### Output:
This code prints the names of all shared libraries currently loaded into the process:
```plaintext
Library: /path/to/libc.so
Library: /path/to/libm.so
...
```