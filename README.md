# ALT Linux Package Comparator

A C++ utility for comparing package repositories between different ALT Linux branches. The tool automatically downloads up-to-date package data and provides detailed comparison results.

## Features

- **Compare branches**: Compare package sets between any two ALT Linux branches
- **Architecture-specific analysis**: Filter comparisons by specific architectures
- **Version tracking**: Identify packages with newer versions in either branch
- **JSON output**: Structured results in JSON format for easy processing
- **CLI interface**: Simple command-line interface for all operations

## Supported Branches

### Main Branches
- `sisyphus`
- `sisyphus_e2k`
- `sisyphus_riscv64` 
- `sisyphus_loongarch64`
- `p11`
- `p10`
- `p10_e2k`
- `p9`
- `c10f2`
- `c9f2`

### Common Architectures
- `srpm`
- `noarch`
- `x86_64`
- `i586`
- `aarch64`
- `x86_64-i586`

## Installation

### From Source
```bash
git clone <repository-url>
cd altlinux-comparator
mkdir build && cd build
cmake ..
make
```

### Docker
```bash
docker pull allowowl/altlinux-comparator
docker run -it allowowl/altlinux-comparator
```

## Usage

### Basic Commands

```bash
# Compare two branches
altlinux-compare compare p10 sisyphus

# Compare with specific architecture
altlinux-compare compare p10 sisyphus x86_64

# Show branch information
altlinux-compare info p10
altlinux-compare info p10 x86_64

# List available branches and architectures
altlinux-compare list-branches

# Show help
altlinux-compare help
```

### Examples

```bash
# Compare p10 vs p9 for x86_64
altlinux-compare compare p10 p9 x86_64

# Compare p10 vs Sisyphus (all architectures)
altlinux-compare compare p10 sisyphus

# Get package count for p10 branch
altlinux-compare info p10

# Check x86_64 packages in p10
altlinux-compare info p10 x86_64
```

## Workflow Logic

1. **Determines data paths** - Organizes package data by branch and architecture
2. **Downloads JSON files** - Fetches up-to-date package data via cURL from ALT Linux API
3. **Processes and compares** - Analyzes package sets across branches and architectures
4. **Saves results** - Outputs structured comparison data to JSON files

## File Organization

```
data/
├── comparison_p10_vs_sisyphus.json
├── comparison_p10_vs_p9_x86_64.json
└── ... (other comparison results)
```

## Output Structure

Output after execution:
```
Comparing p10 vs sisyphus
Loading packages...
Comparison completed!

summary of comparison:
  - pkg only in p10: 58437
  - pkg only in sisyphus: 43771
  - pkg newer in p10: 6248
  - all pkg in p10: 158375
  - all pkg in sisyphus: 143709

Full results saved to: data/comparison_p10_vs_sisyphus.json
Examples in file:
  - 58437 packages only in p10
  - 43771 packages only in sisyphus
  - 6248 packages newer in p10
```
Comparison results are saved in JSON format with the following structure:

```json
{
    "metadata": {
        "branch_1": "p10",
        "branch_2": "sisyphus",
        "compared_architectures": [
            "aarch64",
            "armh",
            "i586",
            "noarch",
            "x86_64",
            "x86_64-i586"
        ],
        "comparison_date": 1762186945
    },
    "packages_newer_in_p10": [
        {
            "arch": "aarch64",
            "branch_1_version": "1.1.2-alt1.1",
            "branch_2_version": "1.1.14-alt1",
            "name": "zmusic-devel"
        },
        ...
    ],
    "packages_only_in_p10": [
        {
            "arch": "aarch64",
            "disttag": "sisyphus+225100.100.1.1",
            "epoch": "0",
            "name": "zzuf",
            "release": "alt1_10",
            "version": "0.15"
        },
        ...
      ],
    "packages_only_in_sisyphus": [
        {
            "arch": "aarch64",
            "disttag": "sisyphus+357971.200.1.1",
            "epoch": "0",
            "name": "zyn-fusion",
            "release": "alt4",
            "version": "3.0.6"
        },
        ...
      ],
    "summary": {
        "size_branch_1_only_packages": 58437,
        "size_branch_1_packages": 158375,
        "size_branch_2_only_packages": 43771,
        "size_branch_2_packages": 143709,
        "size_packages_newer_in_branch_1": 6248
    }
}
```

## Dependencies

- C++17 compatible compiler
- libcurl
- nlohmann-json
- CMake 3.10+

## Platform Support

**ONLY LINUX** - Specifically designed for ALT Linux environments

## Important Notes

- Before comparing branches by architecture, ensure the required packages are present using `info <branch> [arch]`
- Package data is fetched directly from ALT Linux repositories and is always up-to-date
- Results are saved in the `build/data/` directory automatically created by the tool

## Building from Source

```bash
# Install dependencies (ALT Linux)
apt-get update && apt-get install -y \
    cmake gcc-c++ libcurl-devel nlohmann-json-devel \
    git make pkg-config

# Build
mkdir build && cd build
cmake ..
make

# Install (optional)
cp build/altlinux-compare /usr/local/bin/
```
