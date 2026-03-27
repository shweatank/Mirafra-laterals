#!/usr/bin/env python3
"""
AI Integration Module for CI/CD Pipeline
Analyzes test failures and generates fixes using OpenAI API
"""

import os
import sys
import json
import logging
import argparse
from datetime import datetime
from pathlib import Path
from typing import Optional, Dict, List, Tuple
import re

try:
    from openai import OpenAI
except ImportError:
    print("Error: openai package not found. Install with: pip install openai")
    sys.exit(1)


class AIAnalyzer:
    """Analyzes test results and generates fixes using AI"""
    
    def __init__(self, api_key: Optional[str] = None, log_file: Optional[str] = None):
        """Initialize AI analyzer with OpenAI API"""
        self.api_key = api_key or os.environ.get("OPENAI_API_KEY")
        if not self.api_key:
            raise ValueError("OPENAI_API_KEY not found in environment variables")
        
        self.client = OpenAI(api_key=self.api_key)
        self.logger = self._setup_logger(log_file)
        self.model = "gpt-4-turbo-preview"
        self.logger.info("AIAnalyzer initialized")
    
    def _setup_logger(self, log_file: Optional[str] = None) -> logging.Logger:
        """Setup logging configuration"""
        logger = logging.getLogger("AIAnalyzer")
        logger.setLevel(logging.DEBUG)
        
        # Console handler
        console_handler = logging.StreamHandler()
        console_handler.setLevel(logging.INFO)
        console_format = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        console_handler.setFormatter(console_format)
        logger.addHandler(console_handler)
        
        # File handler
        if log_file:
            file_handler = logging.FileHandler(log_file, mode='a')
            file_handler.setLevel(logging.DEBUG)
            file_format = logging.Formatter(
                '%(asctime)s - %(name)s - %(levelname)s - [%(filename)s:%(lineno)d] - %(message)s'
            )
            file_handler.setFormatter(file_format)
            logger.addHandler(file_handler)
        
        return logger
    
    def read_test_results(self, result_file: str) -> str:
        """Read test results from file"""
        try:
            with open(result_file, 'r', encoding='utf-8') as f:
                content = f.read()
            self.logger.info(f"Read test results from {result_file}")
            return content
        except FileNotFoundError:
            self.logger.error(f"Test result file not found: {result_file}")
            raise
        except Exception as e:
            self.logger.error(f"Error reading test results: {str(e)}")
            raise
    
    def analyze_failures(self, test_results: str) -> Dict[str, any]:
        """Analyze test failures using AI"""
        self.logger.info("Analyzing test failures with AI...")
        
        prompt = f"""You are a Python testing expert. Analyze the following test results and provide:
1. Summary of failures and errors
2. Root causes of failures
3. Specific recommendations for fixes
4. Code examples for fixes (if applicable)

Test Results:
{test_results}

Please structure your response as JSON with these keys:
- summary (string): brief summary
- failures (list): list of failing tests
- root_causes (dict): map of test to root cause
- recommendations (list): list of fix recommendations
- code_fixes (dict): map of file to code fix (if applicable)
- severity (string): "critical", "high", "medium", or "low"

Respond ONLY with valid JSON, no markdown formatting."""

        try:
            response = self.client.chat.completions.create(
                model=self.model,
                messages=[
                    {
                        "role": "system",
                        "content": "You are an expert Python developer and test analyst. Provide analysis in JSON format only."
                    },
                    {
                        "role": "user",
                        "content": prompt
                    }
                ],
                temperature=0.7,
                max_tokens=2000,
                timeout=30
            )
            
            analysis_text = response.choices[0].message.content.strip()
            
            # Parse JSON response
            try:
                analysis = json.loads(analysis_text)
                self.logger.info("AI analysis completed successfully")
                return analysis
            except json.JSONDecodeError:
                self.logger.warning("Failed to parse JSON response, extracting text instead")
                return {
                    "summary": analysis_text,
                    "failures": [],
                    "root_causes": {},
                    "recommendations": [],
                    "code_fixes": {}
                }
                
        except Exception as e:
            self.logger.error(f"Error during AI analysis: {str(e)}")
            raise
    
    def generate_fixes(self, source_dir: str, analysis: Dict) -> str:
        """Generate code fixes based on analysis"""
        self.logger.info("Generating code fixes...")
        
        # Collect Python files from source directory
        source_files = self._collect_source_files(source_dir)
        
        prompt = f"""Based on the following test analysis, generate improved code fixes:

Analysis:
{json.dumps(analysis, indent=2)}

Source Files in project:
{chr(10).join(source_files[:5])}

Provide:
1. File-by-file improvements
2. Specific code changes (use clear diff-like format)
3. Explanation of changes
4. Testing recommendations

Focus on fixing the root causes identified in the analysis."""

        try:
            response = self.client.chat.completions.create(
                model=self.model,
                messages=[
                    {
                        "role": "system",
                        "content": "You are an expert Python code reviewer and fixer. Provide clear, actionable code improvements."
                    },
                    {
                        "role": "user",
                        "content": prompt
                    }
                ],
                temperature=0.5,
                max_tokens=3000,
                timeout=30
            )
            
            fixes = response.choices[0].message.content
            self.logger.info("Code fixes generated successfully")
            return fixes
            
        except Exception as e:
            self.logger.error(f"Error generating fixes: {str(e)}")
            raise
    
    def _collect_source_files(self, source_dir: str) -> List[str]:
        """Collect Python source files from directory"""
        files = []
        try:
            for root, dirs, filenames in os.walk(source_dir):
                # Skip common directories
                dirs[:] = [d for d in dirs if d not in ['.git', '__pycache__', '.pytest_cache', 'venv', 'env']]
                
                for filename in filenames:
                    if filename.endswith('.py'):
                        file_path = os.path.join(root, filename)
                        files.append(file_path)
                        if len(files) >= 10:
                            return files
        except Exception as e:
            self.logger.warning(f"Error collecting source files: {str(e)}")
        
        return files
    
    def save_analysis(self, analysis: Dict, output_file: str) -> None:
        """Save analysis results to file"""
        try:
            with open(output_file, 'w', encoding='utf-8') as f:
                json.dump(analysis, f, indent=2)
            self.logger.info(f"Analysis saved to {output_file}")
        except Exception as e:
            self.logger.error(f"Error saving analysis: {str(e)}")
            raise
    
    def save_fixes(self, fixes: str, output_file: str) -> None:
        """Save generated fixes to file"""
        try:
            with open(output_file, 'w', encoding='utf-8') as f:
                f.write("# AI-Generated Fixes and Improvements\n\n")
                f.write(f"Generated: {datetime.now().isoformat()}\n")
                f.write("-" * 80 + "\n\n")
                f.write(fixes)
            self.logger.info(f"Fixes saved to {output_file}")
        except Exception as e:
            self.logger.error(f"Error saving fixes: {str(e)}")
            raise


def main():
    """Main entry point"""
    parser = argparse.ArgumentParser(
        description="AI-powered test analysis and code fix generation"
    )
    parser.add_argument(
        "--result-file",
        required=True,
        help="Path to test result file"
    )
    parser.add_argument(
        "--output-dir",
        default="output",
        help="Output directory for analysis results"
    )
    parser.add_argument(
        "--log-file",
        default=None,
        help="Log file path"
    )
    parser.add_argument(
        "--source-dir",
        default="src",
        help="Source directory for code analysis"
    )
    parser.add_argument(
        "--dry-run",
        default="false",
        choices=["true", "false"],
        help="Dry run mode (no file modifications)"
    )
    
    args = parser.parse_args()
    
    # Setup
    dry_run = args.dry_run.lower() == "true"
    os.makedirs(args.output_dir, exist_ok=True)
    
    # Setup logging
    log_file = args.log_file or os.path.join(args.output_dir, "ai_analysis.log")
    os.makedirs(os.path.dirname(log_file), exist_ok=True)
    
    try:
        # Initialize analyzer
        analyzer = AIAnalyzer(log_file=log_file)
        
        # Read test results
        test_results = analyzer.read_test_results(args.result_file)
        
        # Check if there are actual failures
        if "passed" in test_results and "failed" not in test_results:
            analyzer.logger.info("All tests passed, no analysis needed")
            return 0
        
        # Analyze failures
        analysis = analyzer.analyze_failures(test_results)
        
        # Save analysis
        analysis_file = os.path.join(args.output_dir, "analysis.json")
        analyzer.save_analysis(analysis, analysis_file)
        
        # Generate fixes
        fixes = analyzer.generate_fixes(args.source_dir, analysis)
        
        # Save fixes
        fixes_file = os.path.join(args.output_dir, "fix.txt")
        analyzer.save_fixes(fixes, fixes_file)
        
        # Success message
        analyzer.logger.info("AI analysis and fix generation completed successfully")
        print(f"\n✓ Analysis saved to: {analysis_file}")
        print(f"✓ Fixes saved to: {fixes_file}")
        
        return 0
        
    except KeyError as e:
        logging.error(f"Missing API key: {str(e)}")
        print("Error: OPENAI_API_KEY environment variable not set")
        return 1
    except Exception as e:
        logging.error(f"Fatal error: {str(e)}", exc_info=True)
        print(f"Error: {str(e)}")
        return 1


if __name__ == "__main__":
    sys.exit(main())
