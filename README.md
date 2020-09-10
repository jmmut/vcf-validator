# vcf-validator | [![Build Status](https://travis-ci.com/EBIvariation/vcf-validator.svg?branch=master)](https://travis-ci.com/EBIvariation/vcf-validator) [![Build status](https://ci.appveyor.com/api/projects/status/vt4ldlwhm91e3n49/branch/master?svg=true)](https://ci.appveyor.com/project/ebivariation-bot/vcf-validator/branch/master)

Validation suite for Variant Call Format (VCF) files. Please find below instructions on how to download and run, or read the wiki for more details about checks already implemented.

This suite includes all the checks from the vcftools suite as well as additional lexical, syntactic and semantic analysis of the VCF input. If any inconsistencies are found, they are classified in one of the following categories:

* Errors: Violations of the VCF specification
* Warnings: A recommendation is not followed or potentially wrong information appears in the file

These tools have been implemented using C++11.


## Download

We recommend using the [latest release](https://github.com/EBIvariation/vcf-validator/releases) for the most stable experience using vcf-validator. Along with the release notes, you will find the executables `vcf_validator`, `vcf_assembly_checker` and `vcf_debugulator` for Linux, macOS and Windows. For Windows, please also download the libraries in the [lib/windows_specific/](https://github.com/EBIvariation/vcf-validator/tree/master/lib/windows_specific/) folder.


## Run

### Validator

vcf-validator accepts both compressed and non-compressed input VCF files. Supported compression formats are .gz and .bz2. For other formats such as .zip, the `zcat` command and a pipe can be used (see below).

Reading uncompressed files:
* File path as argument: `vcf_validator -i /path/to/file.vcf`
* Standard input: `vcf_validator < /path/to/file.vcf`
* Standard input from pipe: `cat /path/to/file.vcf | vcf_validator`

Reading compressed files:
* File path as argument: `vcf_validator -i /path/to/compressed_file.vcf.gz`
* Standard input: `vcf_validator < /path/to/compressed_file.vcf.bz2`
* Standard input from pipe: `zcat /path/to/compressed_file.vcf.zip | vcf_validator`

The validation level can be configured using `-l` / `--level`. This parameter is optional and accepts 3 values:

* error: Display only syntax errors
* warning: Display both syntax and semantic, both errors and warnings (default)
* stop: Stop after the first syntax error is found

Different types of validation reports can be written with the `-r` / `--report` option. Several ones may be specified in the same execution, using commas to separate each type (without spaces, e.g.: `-r summary,database,text`).

* summary: Write a human-readable summary report to a file. This includes one line for each type of error and the number of occurrences, along with the first line that shows that type of error (default)
* text: Write a human-readable report to a file, with one description line for each VCF line that has an error.
* database: Write structured report to a database file. The database engine used is SQLite3, so the results can be inspected manually, but they are intended to be consumed by the debugulator.

Each report is written into its own file and it is named after the input file, followed by a timestamp. The default output directory is the same as the input file's if provided using `-i`, or the current directory if using the standard input; it can be changed with the `-o` / `--outdir` option.

A flag for validating evidence can be passed with `--require-evidence`. This will validate whether the VCF includes either Genotypes or Allele Frequencies. This flag will be **false** by default.

### Debugulator

There are some simple errors that can be automatically fixed. The most common error is the presence of duplicate variants. The needed parameters are the original VCF and the report generated by a previous run of the validator with the option `-r database`.

The fixed VCF will be written into the standard output, which you can redirect to a file, or use the `-o` / `--output` option and specify the desired file name.

The logs about what the debugulator is doing will be written into the error output. The logs may be redirected to a log file using `2>debugulator_log.txt` or completely discarded with `2>/dev/null`.

### Assembly Checker

vcf-assembly-checker assesses the correctness of a VCF file with respect to a FASTA file (that should contain the reference sequence). The assembly checker reads the CHR, POS and REF columns from the VCF, and for each line, looks into the FASTA file to see if the REF allele matches that region. A VCF file is considered valid if all the variants match the sequence in the FASTA file.

vcf-assembly-checker requires a FASTA-index file with extension `.fai` for efficient reading of the FASTA file.

If the VCF and FASTA files to compare don't use the same contig name nomenclature (GenBank, RefSeq, UCSC...) you may provide an assembly report file to map between them, using the `-a` option.

vcf-assembly-checker accepts both compressed and non-compressed input VCF files. Supported compression formats are .gz and .bz2. For other formats such as .zip, the `zcat` command and a pipe can be used (see below).

Reading uncompressed files:
* File path as argument: `vcf_assembly_checker -i /path/to/vcf-file -f /path/to/fasta-file -a /path/to/assembly-report`
* Standard input: `vcf_assembly_checker -f /path/to/fasta-file -a /path/to/assembly-report < /path/to/vcf-file`
* Standard input from pipe: `cat /path/to/vcf-file | vcf_assembly_checker -f /path/to/fasta-file -a /path/to/assembly-report`

Reading compressed files:
* File path as argument: `vcf_assembly_checker -i /path/to/compressed-vcf-file -f /path/to/fasta-file -a /path/to/assembly-report`
* Standard input: `vcf_assembly_checker -f /path/to/fasta-file -a /path/to/assembly-report < /path/to/compressed-vcf-file`
* Standard input from pipe: `zcat /path/to/compressed-vcf-file | vcf_assembly_checker -f /path/to/fasta-file -a /path/to/assembly-report`

Different types of validation reports can be written with the `-r` / `--report` option. Several ones may be specified in the same execution, using commas to separate each type (without spaces, e.g.: `-r summary,valid,text`).

* `summary` - Logs the number of matches, total variants checked and percentage of correctness of VCF file on the terminal.
* `valid` - Write valid lines (including meta-info lines) from the input VCF file into an output file.
* `text` - Write a human readable into file, containing errors and warnings generated while checking a VCF file.

Each report is written into its own file and it is named after the input file, followed by a timestamp. The default output directory is the same as the input file's if provided using `-i`, or the current directory if using the standard input; it can be changed with the `-o` / `--outdir` option.

### Examples

Simple example: `vcf_validator -i /path/to/file.vcf`

Full example: `vcf_validator -i /path/to/file.vcf -l stop -r database,stdout -o /path/to/output/folder/`

Debugulator example:

```
vcf_validator -i /path/to/file.vcf -r database -o /path/to/write/report/
vcf_debugulator -i /path/to/file.vcf -e /path/to/write/report/vcf.errors.timestamp.db -o /path/to/fixed.vcf 2>debugulator_log.txt
```

Assembly Checker example:

```
vcf_assembly_checker -i /path/to/file.vcf -f /path/to/fasta_file.fa
vcf_assembly_checker -f /path/to/fasta.fa -r text < /path/to/file.vcf
vcf_assembly_checker -i /path/to/file.vcf -f /path/to/fasta_file.fa -r valid -a /path/to/assembly_report
```


## Build

If you would like to use an unreleased version of vcf-validator, you can clone the repository and build it under 3 platforms: Linux, macOS and Windows. A statically linked executable will be generated, which means you won't need to install any dependencies in the machine that will run it; only install the dependencies in the machine that will build it.

If you plan to make changes, make sure to check the [docs/developer-guide.md](docs/developer-guide.md).

### Linux

The build has been tested on the following compilers:
* Clang 7
* GCC 5

NOTE: Currently the Linux compilation doesn't work in Ubuntu 18 or newer. We are working on fixing this. The Linux compilation is tested and working in Ubuntu 16.04.

#### Dependencies

Some dependencies have to be installed manually and others can be installed automatically. We recommend using the automatic install when possible.

Dependency | Version | Instalation method
:--------: | :-----: | :----:
Cmake | \>=2.8 | manual
Sqlite3 | \>=3 | manual
ODB compiler | 2.4.0 | manual or automatic
ODB common runtime | 2.4.0 | manual or automatic
ODB SQLite runtime | 2.4.0 | manual or automatic
bzip2 | 1.0.6 | manual or automatic
zlib | 1.2.11 | manual or automatic
Boost* | \>=1.65 | manual or automatic

*: See below the exact subset of Boost packages required.

If you are using Ubuntu 16, you can prepare all dependencies and compile the Validation Suite with these commands:
```
sudo apt-get install libsqlite3-0 libsqlite3-dev cmake wget build-essential
./install_dependencies.sh linux
mkdir build && cd build && cmake -G "Unix Makefiles" ..
make
```

The VCF Validation Suite binaries will be created in the `build/bin` subfolder. Optionally, read below for the explanation of the previous commands.

##### SQLite, CMake and automatic installation
The automatic install **requires** CMake, SQLite3 and wget to be installed before running the script (as ODB and zlib require them to be installed). Also, the script will compile some dependencies so a compilation environment is needed. If you are using Ubuntu, you can install all that with the command `sudo apt-get install libsqlite3-0 libsqlite3-dev cmake wget build-essential`. After installing that, use the command `./install_dependencies.sh linux`.

A subfolder named `linux_dependencies` will be created, with all the required libraries copied into it. 

##### Boost

**Note**: You can ignore this section if you are using the `install_dependencies.sh` script.

The dependencies are the Boost library core, and its submodules: Boost.filesystem, Boost.iostreams, Boost.program_options, Boost.regex, Boost.log and Boost.system.
If you are using Ubuntu, you can install them with the command `sudo apt-get install libboost-dev libboost-filesystem-dev libboost-iostreams-dev libboost-program-options-dev libboost-regex-dev libboost-log-dev`.

##### ODB

**Note**: You can ignore this section if you are using the `install_dependencies.sh` script.

You will need to download the ODB compiler, the ODB common runtime library, and the SQLite database runtime library from [this page](http://codesynthesis.com/products/odb/download.xhtml).

ODB requires SQLite3 to be installed. If you are using Ubuntu, the required packages' names will be `libsqlite3-0` and `libsqlite3-dev`.

To install the ODB compiler, the easiest way is to download the `.deb` or `.rpm` packages and install them automatically with `dpkg` or `rpm` respectively. Both the ODB common runtime and SQLite database runtime libraries can be installed manually running `./configure && make && sudo make install`. This will install the libraries in `/usr/local/lib`.

If you don't have root permissions, please run `./configure --prefix=/path/to/odb/libraries/folder` to specify which folder to install ODB in, then `make && make install`, without `sudo`. Also you will have to provide the path to ODB while configuring libodb-sqlite using `./configure --with-libodb=/path/to/odb/libraries`.

#### Compile

In order to create the build scripts, please run `cmake` with your preferred generator. For instance, `mkdir build && cd build && cmake -G "Unix Makefiles" ..` will create Makefiles, and to build the binaries, you will need to run `make`.

If the ODB libraries were not found during the build, please run `sudo updatedb && sudo ldconfig`. Also, if ODB has been installed in a non-default location, the option `-DEXT_LIB_PATH=/path/to/external/libraries/folder` must be also provided to the `cmake` command.

The VCF Validation Suite binaries will be created in the `build/bin` subfolder.


### macOS

On macOS the binaries obtained will only have system libraries dynamically linked. This means you will need to install dependencies to build vcf-validator but not to run it.

#### Dependencies

Some dependencies have to be installed manually and others can be installed automatically. We recommend using the automatic install when possible.

Dependency | Version | Instalation method
:--------: | :-----: | :----:
Cmake | \>=2.8 | manual
Sqlite3 | \>=3 | manual
ODB compiler | 2.4.0 | manual or automatic
ODB common runtime | 2.4.0 | manual or automatic
ODB SQLite runtime | 2.4.0 | manual or automatic
bzip2 | 1.0.6 | manual or automatic
zlib | 1.2.11 | manual or automatic
Boost | \>=1.65 | manual

You can prepare all dependencies and compile the Validation Suite with these commands:
```
brew install cmake ninja boost sqlite3
./install_dependencies.sh osx
mkdir build && cd build && cmake -G "Unix Makefiles" ..
make
```

The VCF Validation Suite binaries will be created in the `build/bin` subfolder. Optionally, read below for the explanation of the previous commands.

##### SQLite, CMake and automatic installation

The automatic install requires CMake, SQLite3 and wget to be installed before running the script (as ODB and zlib require them to be installed). In order to set up the environment to compile the dependencies, first you need to run `brew install cmake ninja boost sqlite3`.

Now you can easily install the ODB ORM and compression libraries just by running `./install_dependencies.sh osx`. You may run `./install_dependencies.sh --help` for usage instructions.

A subfolder named `osx_dependencies` will be created, with all the required libraries copied into it.

#### Compile

In order to create the build scripts, please run `cmake` with your preferred generator. For instance, `mkdir build && cd build && cmake -G "Unix Makefiles" ..` will create Makefiles, and to build the binaries, you will need to run `make`.

The VCF Validation Suite binaries will be created in the `bin` subfolder.

### Windows

On Windows the binaries obtained will only have ODB libraries dynamically linked. We provide a pre-compiled version of these within the repository because they are not easy to build; but if you would like to do it yourself, please check the *Miscellaneous* section below.

#### Dependencies

##### Compression libraries

You will need to download the bzip2 and zlib source code, from [here](http://www.bzip.org/downloads.html) and [here](https://zlib.net/zlib1211.zip) respectively.

##### Boost

The dependencies are the Boost library core, and its submodules: Boost.filesystem, Boost.iostreams, Boost.program_options, Boost.regex, Boost.log and Boost.system. You will need to compile them with zlib and bzip2 support and statically linking the runtime libraries.

* Download Boost from [here](https://www.boost.org/users/download/) and uncompress it
* From the directory where Boost was uncompressed, run these commands:

```
bootstrap
.\b2 --with-atomic --with-chrono --with-date_time --with-filesystem --with-log --with-program_options --with-regex --with-system --with-thread --with-iostreams -sBZIP2_SOURCE=path\to\bzip2-1.x.x -sZLIB_SOURCE=path\to\zlib-1.x.x runtime-link=static --build-type=complete
```

* Add boost_1_xx_x/stage/lib folder to the environment variable `LIB`
* Add boost_1_xx_x folder to the environment variable `INCLUDE`

##### ODB

Precompiled libraries of ODB and the ODB SQLite runtime are provided. In order to download headers, simply run the comand `install_dependencies.bat`, which will create a `windows_dependencies` folder in the root directory of the project.

#### Compile

In order to create the build scripts and compile vcf-validator, please run the following commands from the project root folder:

```
cmake -DCMAKE_BUILD_TYPE=Release -G "NMake Makefiles" /path/to/CMakeLists.txt
nmake
```

Binaries will be created in the `bin` subfolder.

In order to run those binaries, you will need to add the `lib/windows_specific` directory to the `PATH`. This will allow the dll files inside that directory to be found.


## Deliverables

The following binaries are be created after successful build:

* `vcf_validator`: VCF validation tool
* `vcf_debugulator`: automatic VCF fixing tool
* `vcf_assembly_checker`: variant checking tool against FASTA sequence
* `test_validation_suite` and derivatives: testing correct behaviour of the tools listed above


## Tests

Unit tests can be run using the binary `bin/test_validation_suite` or, if the generator supports it, a command like `make test`. The first option may provide a more detailed output in case of test failure.

**Note**: Tests that require input files will only work when executed with `make test` or running the binary from the project root folder (not the `bin` subfolder).

## Generate code from descriptors

Code generated from descriptors shall be always up-to-date in the GitHub repository. If changes to the source descriptors were necessary, please generate the Ragel machines C code from `.ragel` files using:

```
ragel -G2 src/vcf/vcf_v41.ragel -o inc/vcf/validator_detail_v41.hpp
ragel -G2 src/vcf/vcf_v42.ragel -o inc/vcf/validator_detail_v42.hpp
ragel -G2 src/vcf/vcf_v43.ragel -o inc/vcf/validator_detail_v43.hpp
```

And the full ODB-based code from the classes definitions using:

```
odb --include-prefix vcf --std c++11 -d sqlite --generate-query --generate-schema --hxx-suffix .hpp --ixx-suffix .ipp --cxx-suffix .cpp --output-dir inc/vcf/ inc/vcf/error.hpp
mv inc/vcf/error-odb.cpp src/vcf/error-odb.cpp
```

## Miscellaneous

### Build ODB Libraries for windows

This section describes how to build the ODB libraries for Windows. You may ignore it if you want to use pre-compiled libraries provided inside the repository. To build those libraries, first download the source code using `install_dependencies.bat`, which will create the following subfolder inside `windows_dependencies`:

* libodb-2.4.0
* libodb-sqlite-2.4.0
* sqlite
* odb (header files only)

You will have to compile libodb-2.4.0, sqlite and then libodb-sqlite-2.4.0. The build has been tested on Visual Studio 2017.

#### Build ODB runtime

1. Open libodb-2.4.0/libodb-vc12.sln in VS-Studio. File->Open->Project/Solution.
2. Retarget the solution file to latest version. Project->Retarget Solution.
3. Select Build type Release and configuration win32.
4. Select /MT in Project->Properties->Configuration Properties->C/C++->Code Generation->Runtime Library options.
5. Build the solution using Build->Build Solution.

#### Build SQLite for odb-sqlite

1. Open sqlite/sqlite3-vc12.sln in VS-Studio.
2. Retarget the solution file.
3. Select Build type Release and configuration win32.
4. Select /MT in Project->Properties->Configuration Properties->C/C++->Code Generation->Runtime Library options.
5. Build the solution using Build->Build Solution.

#### Build ODB-SQLite runtime

1. Open libodb-sqlite-2.4.0/libodb-sqlite-vc12.sln in VS-Studio.
2. Retarget the solution file.
3. Select Build type Release and configuration win32.
4. Select /MT in Project->Properties->Configuration Properties->C/C++->Code Generation->Runtime Library options.
5. Go to Project->Properties->Configuration Properties->VC++ Directories and append these paths to following variables.
    - Executable Directories => path/to/libodb-2.4.0/bin and path/to/sqlite/bin
    - Include Directories => path/to/libodb-2.4.0 and path/to/sqlite
    - Library Directories => path/to/libodb-2.4.0/lib and path/to/sqlite/lib
Note the paths should be absolute and the directories will be present within windows_dependencies folder.
6. Build the solution using Build->Build Solution.

Now you will obtain compiled LIB and DLL files, in the `lib` and `bin` folders respectively.
