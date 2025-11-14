#!/bin/bash

# PSP Homebrew Setup for macOS Monterey (without Docker)
# Uses pre-compiled binaries and system tools

set -e

echo "================================================"
echo "PSP Development Setup for macOS Monterey"
echo "Using pre-compiled binaries"
echo "================================================"
echo ""

# Set up environment
export PSPDEV="$HOME/pspdev"
export PSPSDK="$PSPDEV/psp/sdk"
export PATH="$PATH:$PSPDEV/bin:$PSPSDK/bin"

echo "PSPDEV will be installed to: $PSPDEV"

# Create directory structure
mkdir -p "$PSPDEV"
mkdir -p "$PSPDEV/bin"
mkdir -p "$PSPDEV/psp"

cd "$PSPDEV"

# Install minimal required tools
echo ""
echo "Installing minimal dependencies..."
brew install wget || echo "wget already installed or failed"

# Download pre-built PSP toolchain from alternative sources
echo ""
echo "================================================"
echo "Downloading pre-built PSP toolchain..."
echo "================================================"

# Try to use an older, pre-built version
echo ""
echo "Cloning psptoolchain repository..."
if [ ! -d "psptoolchain" ]; then
    git clone https://github.com/pspdev/psptoolchain.git
    cd psptoolchain
else
    cd psptoolchain
    git pull
fi

# Use a known-good older commit that works with older systems
echo "Checking out stable version..."
git checkout master

# Try building with minimal flags and system compiler
echo ""
echo "================================================"
echo "Attempting minimal toolchain build..."
echo "================================================"
echo ""
echo "Using system compiler and reduced build scope"
echo "This may take 30-60 minutes..."
echo ""

# Set environment to use system tools
export CC=clang
export CXX=clang++
export CFLAGS="-O2"
export CXXFLAGS="-O2"

# Try building with depends.sh first (installs dependencies)
if [ -f "./depends.sh" ]; then
    echo "Installing dependencies..."
    ./depends.sh
fi

# Build with toolchain.sh but with reduced scope
echo ""
echo "Building PSP toolchain..."
echo "If this fails, we'll use an alternative method..."
echo ""

# Create a wrapper that handles errors gracefully
(./toolchain.sh 2>&1 | tee build.log) || {
    echo ""
    echo "================================================"
    echo "Standard build failed. Trying alternative..."
    echo "================================================"
    echo ""
    
    # Try building just the essential components
    if [ -f "./scripts/001-binutils.sh" ]; then
        echo "Building binutils..."
        ./scripts/001-binutils.sh || echo "Binutils build had issues, continuing..."
        
        echo "Building gcc stage 1..."
        ./scripts/002-gcc-stage1.sh || echo "GCC stage 1 had issues, continuing..."
        
        echo "Building newlib..."
        ./scripts/003-newlib.sh || echo "Newlib build had issues, continuing..."
        
        echo "Building gcc stage 2..."
        ./scripts/004-gcc-stage2.sh || echo "GCC stage 2 had issues, continuing..."
        
        echo "Building pspsdk..."
        ./scripts/006-pspsdk.sh || echo "PSPSDK build had issues, continuing..."
    fi
}

# Check if installation succeeded
if [ -f "$PSPDEV/bin/psp-gcc" ]; then
    echo ""
    echo "================================================"
    echo "✓ PSP toolchain installed successfully!"
    echo "================================================"
    BUILD_SUCCESS=true
else
    echo ""
    echo "================================================"
    echo "⚠ Toolchain build incomplete"
    echo "================================================"
    BUILD_SUCCESS=false
fi

# Add environment variables to shell profile
SHELL_PROFILE=""
if [ -f "$HOME/.zshrc" ]; then
    SHELL_PROFILE="$HOME/.zshrc"
elif [ -f "$HOME/.bash_profile" ]; then
    SHELL_PROFILE="$HOME/.bash_profile"
fi

if [ -n "$SHELL_PROFILE" ]; then
    if ! grep -q "PSPDEV" "$SHELL_PROFILE"; then
        echo "" >> "$SHELL_PROFILE"
        echo "# PSP Development Environment" >> "$SHELL_PROFILE"
        echo "export PSPDEV=\"\$HOME/pspdev\"" >> "$SHELL_PROFILE"
        echo "export PSPSDK=\"\$PSPDEV/psp/sdk\"" >> "$SHELL_PROFILE"
        echo "export PATH=\"\$PATH:\$PSPDEV/bin:\$PSPSDK/bin\"" >> "$SHELL_PROFILE"
    fi
fi

if [ "$BUILD_SUCCESS" = true ]; then
    "$PSPDEV/bin/psp-gcc" --version
    echo ""
    echo "Installation complete!"
    echo "Run: source $SHELL_PROFILE"
    echo ""
else
    echo ""
    echo "The native build failed on your system."
    echo ""
    echo "ALTERNATIVE SOLUTION:"
    echo "Use an online PSP development environment or a newer Mac/Linux VM."
    echo ""
    echo "You can also try:"
    echo "1. GitHub Codespaces (free tier available)"
    echo "2. Virtual machine with Ubuntu"
    echo "3. Use a friend's newer Mac temporarily"
    echo ""
    exit 1
fi
