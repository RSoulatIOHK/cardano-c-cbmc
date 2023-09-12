# Cardano-C
![Post-Integration](https://github.com/Biglup/cardano-c/actions/workflows/unit-test.yml/badge.svg)

"cardano-c" is a C library aiming to be a robust, commercial-grade, full-featured Toolkit for building transaction and interacting with the Cardano blockchain. Compliant with MISRA standards, 
it ensures consistent reliability and safety. With its binding-friendly architecture, "cardano-c" aims for broad compatibility, enabling easy integrations across various programming languages. 

A practical toolkit for developers working with Cardano.

### Features

- **Address Parsing & Generation:** Mnemonic creation/restoration and address derivation functionalities.
- **Ed25519 Cryptography:** Support for the Ed25519 signature scheme is based on [libsodium](https://github.com/jedisct1/libsodium) a powerful library for encryption, decryption, signatures, password hashing and more..
- **Transaction Serialization & Deserialization:** Convert transactions to and from CBOR format for transmission to the blockchain.
- **Powerful Transaction Builder:** A versatile tool with comprehensive protocol support that allows you to easily create Cardano transactions.

### Getting Started

TBD

### Prerequisites

TBD

### Installation

TBD


### Set up the Git hooks custom directory

After cloning the repository run the following command in the
repository root:

```shell
git config core.hooksPath .githooks
```

### Clang format integration

Repository comes with always-up-to-date `.clang-format` file, an input configuration
for `clang-format` tool (version 15.x is a minimum). 

- https://apt.llvm.org/

### Unit Tests

This project uses Google test framework for writing unit tests.

Start by installing the gtest development package:

```bash
sudo apt-get install libgtest-dev
```

Note that this package only install source files. You have to compile the code yourself to create the necessary
library files.

These source files should be located at /usr/src/googletest.

```bash
cd $(mktemp -d)
cmake /usr/src/googletest
make
sudo make install
```

### MISRA Compliance 

This project C source files use MISRA C 2012 guidelines as underlying coding standard.

For MISRA validation, the project uses cppcheck. Cppcheck covers almost all the MISRA C 2012 rules.
Including the amendments. Together with a C compiler flags `-std=c99 -pedantic-errors -Wall -Wextra -Werror` we get full coverage.

#### Deviations

The source code has the following deviations from MISRA C 2012:

| Rule                 | Category | Rationale for Skipping                                                                                     |
|----------------------|----------|-------------------------------------------------------------------------------------------------------------|
| [15.5]: A function should have a single point of exit at the end | Advisory | Early returns can reduce the need for nested conditional statements, making the code more straightforward and easier to read. The intent behind using an early return is to handle edge cases or preconditions at the beginning of the function, allowing the main function logic to remain unindented and clear. |

### Contributing

We welcome contributions from the community. Please read our CONTRIBUTING.md for guidelines.


### License 

[APACHE LICENSE, VERSION 2.0](https://apache.org/licenses/LICENSE-2.0)
