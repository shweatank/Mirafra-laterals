#!/usr/bin/env python3
"""
CI/CD Pipeline - AI Integration Script
Main entry point for AI analysis and fix generation
"""

import sys
import os

# Add parent directory to path for imports
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

from src.ai_integration import main

if __name__ == "__main__":
    sys.exit(main())
