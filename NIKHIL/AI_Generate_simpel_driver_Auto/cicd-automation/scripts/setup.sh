#!/bin/bash
# GitHub Actions Setup and Installation Script

set -e

echo "=========================================="
echo "CI/CD Automation Setup Script"
echo "=========================================="

# Check Python installation
echo "Checking Python installation..."
python3 --version
pip3 --version

# Install main dependencies
echo "Installing dependencies..."
pip3 install -r requirements.txt

# Create necessary directories
echo "Creating directory structure..."
mkdir -p logs output .github/workflows

# Copy workflow file
echo "Setting up GitHub Actions workflow..."
if [ ! -d ".github/workflows" ]; then
    mkdir -p .github/workflows
fi

if [ -f ".github/workflows/ai-cicd-pipeline.yml" ]; then
    cp .github/workflows/ai-cicd-pipeline.yml .github/workflows/
    echo "✓ Workflow file copied to .github/workflows/"
else
    echo "⚠ Warning: Workflow file not found"
fi

# Create .env file if not exists
if [ ! -f ".env" ]; then
    echo "Creating .env file..."
    cp config/.env.example .env
    echo "⚠ Please update .env with your OPENAI_API_KEY"
else
    echo "✓ .env file already exists"
fi

# Verify test setup
echo "Verifying test setup..."
if [ -d "tests" ] && [ -f "tests/test_example.py" ]; then
    echo "✓ Test files found"
else
    echo "⚠ Test files not found"
fi

echo ""
echo "=========================================="
echo "Setup Complete!"
echo "=========================================="
echo ""
echo "Next steps:"
echo "1. Update your OPENAI_API_KEY in .env file"
echo "2. Configure GitHub Secrets in your repository settings"
echo "3. Push the files to your repository"
echo "4. Test locally: pytest tests/"
echo "=========================================="
