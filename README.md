# Introduction

Ganxo is an opensource API hooking framework. In Catalan, Ganxo means hook and is pronounced like you pronounce: "Gun show".
Ganxo uses [Capstone](https://github.com/aquynh/capstone) disassembler for its disassembly requirements.

# Features
* Ganxo is transactional. No hooking side effects take place until the transaction is committed.
* Written in C and aims to be light and portable. Although for now only Windows is supported.
* Ganxo has a rich api: simple data structures APIs, memory APIs, disassembly APIs.
* Heavily documented source code which is ideal for learning how to build API hooking frameworks.

# Building
* Clone Ganxo from: http://github.com/0xeb/Ganxo
* Clone the thirdparty dependencies:

```
git submodule update --init --recursive
```

This is a template for Windows x86 builds:

```
mkdir build
cd build

cmake .. -G "Visual Studio 15" -DCAPSTONE_BUILD_STATIC_RUNTIME=OFF -DCAPSTONE_BUILD_STATIC=ON -DCAPSTONE_BUILD_SHARED=OFF -DCAPSTONE_BUILD_DIET=OFF -DCAPSTONE_BUILD_TESTS=OFF -DCAPSTONE_USE_DEFAULT_ALLOC=ON -DCAPSTONE_ARM_SUPPORT=OFF -DCAPSTONE_ARM64_SUPPORT=OFF -DCAPSTONE_MIPS_SUPPORT=OFF -DCAPSTONE_PPC_SUPPORT=OFF -DCAPSTONE_SPARC_SUPPORT=OFF -DCAPSTONE_SYSZ_SUPPORT=OFF -DCAPSTONE_XCORE_SUPPORT=OFF -DCAPSTONE_X86_SUPPORT=ON -DCAPSTONE_X86_REDUCE=OFF -DCAPSTONE_X86_ATT_DISABLE=ON -DCAPSTONE_OSXKERNEL_SUPPORT=OFF -DGANXO_PLATFORM_WINDOWS=ON -DGANXO_ARCH_X86=ON
```

To build for x64, pass the `-A x64` argument and change `-DGANXO_ARCH_X86=ON` to `-DGANXO_ARCH_X64=ON`.
Feel free to tweak the capstone defines.

Now you can open the VS solution `Ganxo.sln` or build the project from the command:
```
cmake --build . --config Release
```

# Usage examples

## Simple

This is an exmaple program that hooks Sleep() and MessageBoxA()

### Step 1 - Define the prototypes and the hook functions
```c++
#include <Ganxo.h>

typedef VOID(WINAPI *Sleep_proto)(DWORD);
typedef int (WINAPI *MessageBoxA_proto)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);

Sleep_proto orig_Sleep = Sleep;
VOID WINAPI My_Sleep(DWORD dwMilliSecs)
{
    printf("MySleep() triggered! calling original sleep...");
    orig_Sleep(dwMilliSecs);
    printf("done!\n");
}

MessageBoxA_proto orig_MessageBoxA = MessageBoxA;
int WINAPI my_MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
    printf("MessageBoxA(hWnd=%08p, lpText='%s', lpCaption='%s', uType=%04X)\n",
        hWnd, lpText, lpCaption, uType);
    return IDOK;
}
```

### Step 2 - Initialize Ganxo

```c++
gnx_err_t err = gnx_init();
if (err != GNX_ERR_OK)
{
    printf("Failed to initialize!\n");
    return -1;
}

// Create a new workspace instance
gnx_handle_t gnx;
if (gnx_open(&gnx) != GNX_ERR_OK)
{
    printf("Failed to create workspace\n");
    return -1;
}
```

### Step 3 - Start a hooking transaction

```c++
gnx_handle_t transaction;
gnx_transaction_begin(gnx, &transaction);
```
### Step 4 - Add the function to the transaction

```c++
// Hook
err = gnx_transaction_add_hook(
    transaction, 
    (void **)&orig_Sleep, 
    My_Sleep);
err = gnx_transaction_add_hook(
    transaction, 
    (void **)&orig_MessageBoxA, 
    my_MessageBoxA);
```

### Step 5 - Commit the transaction
```
err = gnx_transaction_commit(transaction);
```
### Step 6 - Functions are now hooked!
```c++
// Call hooked functions
printf("main(): calling Sleep()\n");
::Sleep(10);

MessageBoxA(0, "Hello", "Info", MB_OK);
```

### Step 7 - When done, remove the hooks
Start a new transaction for the sake of removing the hooks, remove the hooks and commit the transaction:
```c++
// Unhook
gnx_transaction_begin(gnx, &transaction);
gnx_transaction_remove_hook(
    transaction, 
    (void **)&orig_Sleep);
gnx_transaction_remove_hook(
    transaction, 
    (void **)&orig_MessageBoxA);
gnx_transaction_commit(transaction);
gnx_close(gnx);
```
