#!/usr/bin/env python3
"""
Logger and Utility Module
Provides logging and utility functions for CI/CD pipeline
"""

import logging
import os
import sys
from datetime import datetime
from pathlib import Path
from typing import Optional


class PipelineLogger:
    """Centralized logging for the pipeline"""
    
    def __init__(self, log_dir: str = "logs", log_level: str = "INFO"):
        """Initialize logger"""
        self.log_dir = Path(log_dir)
        self.log_dir.mkdir(parents=True, exist_ok=True)
        
        # Create timestamp-based log file
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        self.log_file = self.log_dir / f"pipeline_{timestamp}.log"
        
        self.logger = self._setup_logger(log_level)
    
    def _setup_logger(self, log_level: str) -> logging.Logger:
        """Setup logger with console and file handlers"""
        logger = logging.getLogger("Pipeline")
        logger.setLevel(getattr(logging, log_level))
        
        # Clear existing handlers
        logger.handlers.clear()
        
        # Console handler
        console_handler = logging.StreamHandler(sys.stdout)
        console_handler.setLevel(logging.INFO)
        console_format = logging.Formatter(
            '%(asctime)s | %(levelname)-8s | %(message)s',
            datefmt='%Y-%m-%d %H:%M:%S'
        )
        console_handler.setFormatter(console_format)
        logger.addHandler(console_handler)
        
        # File handler
        file_handler = logging.FileHandler(self.log_file)
        file_handler.setLevel(logging.DEBUG)
        file_format = logging.Formatter(
            '%(asctime)s | %(levelname)-8s | %(name)-12s | %(funcName)-15s | %(message)s',
            datefmt='%Y-%m-%d %H:%M:%S'
        )
        file_handler.setFormatter(file_format)
        logger.addHandler(file_handler)
        
        return logger
    
    def get_logger(self) -> logging.Logger:
        """Get logger instance"""
        return self.logger
    
    def info(self, message: str):
        """Log info message"""
        self.logger.info(message)
    
    def warning(self, message: str):
        """Log warning message"""
        self.logger.warning(message)
    
    def error(self, message: str):
        """Log error message"""
        self.logger.error(message)
    
    def debug(self, message: str):
        """Log debug message"""
        self.logger.debug(message)
    
    def section(self, title: str):
        """Log section header"""
        line = "=" * 80
        self.logger.info(line)
        self.logger.info(f"  {title}")
        self.logger.info(line)


class FileUtils:
    """File utility functions"""
    
    @staticmethod
    def read_file(filepath: str) -> str:
        """Read file contents"""
        try:
            with open(filepath, 'r', encoding='utf-8') as f:
                return f.read()
        except FileNotFoundError:
            raise FileNotFoundError(f"File not found: {filepath}")
        except Exception as e:
            raise Exception(f"Error reading file {filepath}: {str(e)}")
    
    @staticmethod
    def write_file(filepath: str, content: str, append: bool = False) -> None:
        """Write content to file"""
        try:
            os.makedirs(os.path.dirname(filepath), exist_ok=True)
            mode = 'a' if append else 'w'
            with open(filepath, mode, encoding='utf-8') as f:
                f.write(content)
        except Exception as e:
            raise Exception(f"Error writing to file {filepath}: {str(e)}")
    
    @staticmethod
    def file_exists(filepath: str) -> bool:
        """Check if file exists"""
        return os.path.isfile(filepath)
    
    @staticmethod
    def get_file_size(filepath: str) -> int:
        """Get file size in bytes"""
        try:
            return os.path.getsize(filepath)
        except FileNotFoundError:
            return 0
    
    @staticmethod
    def list_files(directory: str, extension: str = "") -> list:
        """List files in directory"""
        try:
            files = []
            for filename in os.listdir(directory):
                if extension and not filename.endswith(extension):
                    continue
                files.append(os.path.join(directory, filename))
            return files
        except FileNotFoundError:
            return []


class ProcessingStats:
    """Track processing statistics"""
    
    def __init__(self):
        """Initialize stats"""
        self.start_time = datetime.now()
        self.stats = {
            "tests_run": 0,
            "tests_passed": 0,
            "tests_failed": 0,
            "tests_skipped": 0,
            "ai_analysis_time": 0,
            "fixes_generated": 0,
            "files_modified": 0,
            "errors": 0
        }
    
    def update(self, key: str, value: int or float):
        """Update statistic"""
        if key in self.stats:
            self.stats[key] = value
    
    def increment(self, key: str, amount: int = 1):
        """Increment statistic"""
        if key in self.stats:
            self.stats[key] += amount
    
    def get_duration(self) -> str:
        """Get elapsed time"""
        duration = datetime.now() - self.start_time
        seconds = int(duration.total_seconds())
        hours = seconds // 3600
        minutes = (seconds % 3600) // 60
        secs = seconds % 60
        
        if hours > 0:
            return f"{hours}h {minutes}m {secs}s"
        elif minutes > 0:
            return f"{minutes}m {secs}s"
        else:
            return f"{secs}s"
    
    def summary(self) -> str:
        """Get summary of statistics"""
        return f"""
╔════════════════════════════════════════╗
║      Pipeline Execution Summary        ║
╠════════════════════════════════════════╣
║ Tests Run:       {self.stats['tests_run']:>24} ║
║ Tests Passed:    {self.stats['tests_passed']:>24} ║
║ Tests Failed:    {self.stats['tests_failed']:>24} ║
║ Tests Skipped:   {self.stats['tests_skipped']:>24} ║
║ Fixes Generated: {self.stats['fixes_generated']:>24} ║
║ Files Modified:  {self.stats['files_modified']:>24} ║
║ Duration:        {self.get_duration():>24} ║
║ Errors:          {self.stats['errors']:>24} ║
╚════════════════════════════════════════╝
"""


if __name__ == "__main__":
    # Test logger
    logger = PipelineLogger()
    logger.section("Testing Pipeline Logger")
    logger.info("This is an info message")
    logger.warning("This is a warning message")
    logger.error("This is an error message")
    logger.debug("This is a debug message")
    
    # Test stats
    stats = ProcessingStats()
    stats.update("tests_run", 42)
    stats.update("tests_passed", 40)
    stats.update("tests_failed", 2)
    print(stats.summary())
