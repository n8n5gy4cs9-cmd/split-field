#!/bin/bash

# Docker-based PSP Development Setup
# This is the recommended approach for older macOS systems

set -e

echo "================================================"
echo "Docker-based PSP Development Environment Setup"
echo "================================================"
echo ""

# Check if Docker is installed
if ! command -v docker &> /dev/null; then
    echo "Docker is not installed."
    echo ""
    echo "Please install Docker Desktop for Mac from:"
    echo "https://www.docker.com/products/docker-desktop"
    echo ""
    echo "Or install via Homebrew:"
    echo "  brew install --cask docker"
    echo ""
    exit 1
fi

echo "✓ Docker is installed"

# Check if Docker daemon is running
if ! docker info &> /dev/null; then
    echo ""
    echo "Docker daemon is not running."
    echo "Please start Docker Desktop and try again."
    exit 1
fi

echo "✓ Docker daemon is running"

# Pull the PSP development Docker image
echo ""
echo "Pulling PSP development Docker image..."
echo "This may take a few minutes..."
docker pull pspdev/pspdev:latest

# Create helper scripts
echo ""
echo "Creating helper scripts..."

# Create psp-make script
cat > /usr/local/bin/psp-make << 'EOF'
#!/bin/bash
docker run --rm -v "$PWD:/build" -w /build pspdev/pspdev:latest make "$@"
EOF

# Create psp-shell script for interactive shell
cat > /usr/local/bin/psp-shell << 'EOF'
#!/bin/bash
docker run --rm -it -v "$PWD:/build" -w /build pspdev/pspdev:latest /bin/bash
EOF

# Make them executable
chmod +x /usr/local/bin/psp-make 2>/dev/null || {
    echo "Note: Cannot create scripts in /usr/local/bin (permission denied)"
    echo "Creating scripts in your home directory instead..."
    
    cat > "$HOME/psp-make" << 'EOF'
#!/bin/bash
docker run --rm -v "$PWD:/build" -w /build pspdev/pspdev:latest make "$@"
EOF

    cat > "$HOME/psp-shell" << 'EOF'
#!/bin/bash
docker run --rm -it -v "$PWD:/build" -w /build pspdev/pspdev:latest /bin/bash
EOF

    chmod +x "$HOME/psp-make"
    chmod +x "$HOME/psp-shell"
    
    # Add to PATH if not already there
    SHELL_PROFILE=""
    if [ -f "$HOME/.zshrc" ]; then
        SHELL_PROFILE="$HOME/.zshrc"
    elif [ -f "$HOME/.bash_profile" ]; then
        SHELL_PROFILE="$HOME/.bash_profile"
    fi
    
    if [ -n "$SHELL_PROFILE" ]; then
        if ! grep -q "export PATH=\"\$HOME:\$PATH\"" "$SHELL_PROFILE"; then
            echo "" >> "$SHELL_PROFILE"
            echo "# PSP Development Scripts" >> "$SHELL_PROFILE"
            echo "export PATH=\"\$HOME:\$PATH\"" >> "$SHELL_PROFILE"
        fi
    fi
    
    echo "✓ Scripts created in $HOME/"
    SCRIPT_LOCATION="$HOME"
}

echo ""
echo "================================================"
echo "Docker Setup Complete!"
echo "================================================"
echo ""
echo "You can now build PSP homebrew using:"
echo ""
if [ "$SCRIPT_LOCATION" = "$HOME" ]; then
    echo "  ~/psp-make          # Build your project"
    echo "  ~/psp-make clean    # Clean build files"
    echo "  ~/psp-shell         # Open interactive shell"
    echo ""
    echo "Or use Docker directly:"
else
    echo "  psp-make            # Build your project"
    echo "  psp-make clean      # Clean build files"
    echo "  psp-shell           # Open interactive shell"
    echo ""
    echo "Or use Docker directly:"
fi
echo "  docker run --rm -v \"\$PWD:/build\" -w /build pspdev/pspdev:latest make"
echo ""
echo "Try building the Hello World example now:"
echo "  cd /Users/harriahola/Projects/PSP/helloworld"
if [ "$SCRIPT_LOCATION" = "$HOME" ]; then
    echo "  ~/psp-make"
else
    echo "  psp-make"
fi
echo ""
