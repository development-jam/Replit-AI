#!/usr/bin/env python3
"""
Python interface for the Replit Programming Language
Provides high-level tools and utilities for the Replit language
"""

import subprocess
import os
import sys
from pathlib import Path

class ReplitLanguage:
    def __init__(self):
        self.executable = "./replit"
        self.build_if_needed()
    
    def build_if_needed(self):
        """Build the Replit interpreter if it doesn't exist"""
        if not os.path.exists(self.executable):
            print("Building Replit interpreter...")
            try:
                # Compile the C++ source files
                compile_cmd = [
                    "g++", "-std=c++17", "-O2", "-o", "replit",
                    "main.cpp", "lexer.cpp", "parser.cpp", "vm.cpp", "replit_engine.cpp"
                ]
                subprocess.run(compile_cmd, check=True)
                print("✓ Replit interpreter built successfully!")
            except subprocess.CalledProcessError as e:
                print(f"✗ Build failed: {e}")
                raise
    
    def run_code(self, code):
        """Run Replit code and return the output"""
        try:
            # Create a temporary file
            with open("temp.rpl", "w") as f:
                f.write(code)
            
            # Run the interpreter
            result = subprocess.run([self.executable, "temp.rpl"], 
                                  capture_output=True, text=True)
            
            # Clean up
            os.remove("temp.rpl")
            
            if result.returncode == 0:
                return result.stdout
            else:
                return f"Error: {result.stderr}"
        except Exception as e:
            return f"Error running code: {e}"
    
    def run_file(self, filename):
        """Run a Replit source file"""
        try:
            result = subprocess.run([self.executable, filename], 
                                  capture_output=True, text=True)
            
            if result.returncode == 0:
                return result.stdout
            else:
                return f"Error: {result.stderr}"
        except Exception as e:
            return f"Error running file: {e}"
    
    def start_repl(self):
        """Start an interactive REPL session"""
        try:
            subprocess.run([self.executable])
        except KeyboardInterrupt:
            print("\nExiting Replit REPL")

def main():
    """Command line interface for Replit Python tools"""
    if len(sys.argv) < 2:
        print("Replit Language Python Interface")
        print("Commands:")
        print("  repl     - Start interactive REPL")
        print("  run <file> - Run a Replit source file")
        print("  build    - Build the interpreter")
        return
    
    rpl = ReplitLanguage()
    command = sys.argv[1]
    
    if command == "repl":
        rpl.start_repl()
    elif command == "run" and len(sys.argv) > 2:
        filename = sys.argv[2]
        output = rpl.run_file(filename)
        print(output)
    elif command == "build":
        rpl.build_if_needed()
    else:
        print(f"Unknown command: {command}")

if __name__ == "__main__":
    main()