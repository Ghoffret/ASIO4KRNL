#!/usr/bin/env python3
"""
ASIO4KRNL Stub File Generator
Creates placeholder files for installer testing
"""

from pathlib import Path

def create_placeholder_file(path, content):
    """Create a placeholder file with specified content"""
    path.parent.mkdir(parents=True, exist_ok=True)
    with open(path, 'w') as f:
        f.write(content)
    print(f"Created placeholder: {path}")

def main():
    base_path = Path(__file__).parent
    
    # Create placeholder GUI executable
    gui_path = base_path / "src/gui/x64/Release/ASIO4KrnlGUI.exe"
    create_placeholder_file(gui_path, 
        "# ASIO4KRNL GUI Application Placeholder\n"
        "# This is a placeholder file for installer testing\n"
        "# In a real build, this would be the compiled GUI application\n")
    
    # Create placeholder driver
    driver_path = base_path / "src/driver/x64/Release/ASIOUSB.sys"
    create_placeholder_file(driver_path,
        "# ASIO4KRNL Driver Placeholder\n" 
        "# This is a placeholder file for installer testing\n"
        "# In a real build, this would be the compiled kernel driver\n")
    
    print("Placeholder files created successfully!")
    print("Note: These are placeholder files for installer testing only.")

if __name__ == "__main__":
    main()