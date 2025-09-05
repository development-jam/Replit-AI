#!/usr/bin/env python3
"""
Interactive demo for the Replit Programming Language
"""

import subprocess
import time

def run_replit_code(code):
    """Run Replit code and return output"""
    with open("temp_demo.rpl", "w") as f:
        f.write(code)
    
    try:
        result = subprocess.run(["./replit", "temp_demo.rpl"], 
                              capture_output=True, text=True)
        return result.stdout if result.returncode == 0 else result.stderr
    except Exception as e:
        return f"Error: {e}"

def main():
    print("🚀 Replit Programming Language Demo")
    print("=" * 40)
    
    # Demo 1: Basic arithmetic
    print("\n📊 Basic Arithmetic:")
    code1 = '''print "5 + 3 =", 5 + 3;
print "10 - 4 =", 10 - 4;
print "6 * 7 =", 6 * 7;
print "20 / 4 =", 20 / 4;'''
    
    with open("temp_demo.rpl", "w") as f:
        f.write('print "5 + 3 = " + (5 + 3);')
    result = subprocess.run(["./replit", "temp_demo.rpl"], capture_output=True, text=True)
    print(result.stdout if result.returncode == 0 else "Error in demo")
    
    # Demo 2: String operations
    print("\n🔤 String Operations:")
    with open("temp_demo.rpl", "w") as f:
        f.write('print "Hello" + " " + "World!";')
    result = subprocess.run(["./replit", "temp_demo.rpl"], capture_output=True, text=True)
    print(result.stdout if result.returncode == 0 else "Error in demo")
    
    # Demo 3: Boolean logic
    print("\n✅ Boolean Logic:")
    with open("temp_demo.rpl", "w") as f:
        f.write('print "true AND false = " + (true and false);')
    result = subprocess.run(["./replit", "temp_demo.rpl"], capture_output=True, text=True)
    print(result.stdout if result.returncode == 0 else "Error in demo")
    
    print("\n🎉 Replit Language is working!")
    print("Try running: ./replit examples/hello.rpl")

if __name__ == "__main__":
    main()