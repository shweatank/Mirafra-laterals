#!/bin/bash
# Local Testing Script
# Run this to test the pipeline locally before pushing to GitHub

set -e

echo "=========================================="
echo "Local CI/CD Pipeline Test"
echo "=========================================="

# Create output directory
mkdir -p output logs

# Step 1: Run tests
echo ""
echo "Step 1: Running test suite..."
python3 -m pytest tests/ -v --tb=short > output/result.txt 2>&1 || true
echo "✓ Tests completed (output in output/result.txt)"

# Step 2: Check if OpenAI API key is set
echo ""
echo "Step 2: Checking OpenAI API key..."
if [ -z "$OPENAI_API_KEY" ]; then
    if [ -f ".env" ]; then
        export $(cat .env | xargs)
    fi
fi

if [ -z "$OPENAI_API_KEY" ]; then
    echo "⚠ OPENAI_API_KEY not set. Skipping AI analysis."
    echo "  Set OPENAI_API_KEY environment variable or create .env file"
else
    echo "✓ API key found"
    
    # Step 3: Run AI analysis
    echo ""
    echo "Step 3: Running AI analysis..."
    python3 scripts/ai_integration.py \
        --result-file output/result.txt \
        --output-dir output \
        --log-file logs/test_run.log \
        --source-dir src \
        --dry-run false
    
    echo "✓ AI analysis completed"
fi

# Display results
echo ""
echo "=========================================="
echo "Results:"
echo "=========================================="
echo ""
echo "Test Output:"
tail -20 output/result.txt
echo ""

if [ -f "output/fix.txt" ]; then
    echo "AI Suggestions:"
    head -20 output/fix.txt
    echo "..."
    echo ""
fi

echo "=========================================="
echo "✓ Local test completed!"
echo "=========================================="
