#!/usr/bin/env python3
"""
ASIO4KRNL Installer Build Validation
Simulates the installer build process and validates all components
"""

import os
import xml.etree.ElementTree as ET
from pathlib import Path

def validate_wix_file():
    """Validate the WiX installer definition file"""
    print("🔍 Validating WiX installer definition...")
    
    wix_file = Path("installer/ASIO4KrnlInstaller.wxs")
    if not wix_file.exists():
        raise FileNotFoundError(f"WiX file not found: {wix_file}")
    
    try:
        # Parse XML
        tree = ET.parse(wix_file)
        root = tree.getroot()
        
        # Check namespace
        if not root.tag.endswith("}Wix"):
            raise ValueError("Not a valid WiX file - missing Wix namespace")
        
        # Find Product element
        product = root.find(".//{http://schemas.microsoft.com/wix/2006/wi}Product")
        if product is None:
            raise ValueError("No Product element found")
        
        # Validate required attributes
        required_attrs = ['Id', 'Name', 'Language', 'Version', 'Manufacturer', 'UpgradeCode']
        for attr in required_attrs:
            if attr not in product.attrib:
                raise ValueError(f"Missing required Product attribute: {attr}")
        
        # Count components
        components = root.findall(".//{http://schemas.microsoft.com/wix/2006/wi}Component")
        features = root.findall(".//{http://schemas.microsoft.com/wix/2006/wi}Feature")
        
        print(f"✓ WiX file is valid XML")
        print(f"✓ Product: {product.attrib.get('Name')}")
        print(f"✓ Version: {product.attrib.get('Version')}")
        print(f"✓ Found {len(components)} components")
        print(f"✓ Found {len(features)} features")
        
    except ET.ParseError as e:
        raise ValueError(f"XML parsing error: {e}")

def validate_source_files():
    """Validate that all required source files exist"""
    print("\n🔍 Validating source files...")
    
    required_files = [
        "src/driver/x64/Release/ASIOUSB.sys",
        "src/gui/x64/Release/ASIO4KrnlGUI.exe",
        "src/driver/ASIOUSB.inf",
        "installer/AppIcon.ico",
        "installer/installer_banner.bmp", 
        "installer/installer_header.bmp",
        "installer/README.txt"
    ]
    
    optional_files = [
        "LICENSE",
        "INSTALLATION_GUIDE.md",
        "USER_MANUAL.md", 
        "DEVICE_COMPATIBILITY.md"
    ]
    
    missing_required = []
    missing_optional = []
    
    for file_path in required_files:
        if not Path(file_path).exists():
            missing_required.append(file_path)
        else:
            print(f"✓ {file_path}")
    
    for file_path in optional_files:
        if not Path(file_path).exists():
            missing_optional.append(file_path)
        else:
            print(f"✓ {file_path}")
    
    if missing_required:
        print(f"\n❌ Missing required files:")
        for file_path in missing_required:
            print(f"  • {file_path}")
        raise FileNotFoundError("Required source files missing")
    
    if missing_optional:
        print(f"\n⚠️ Missing optional files:")
        for file_path in missing_optional:
            print(f"  • {file_path}")

def validate_build_scripts():
    """Validate build scripts exist and are properly structured"""
    print("\n🔍 Validating build scripts...")
    
    scripts = [
        ("installer/package.ps1", "PowerShell build script", True),
        ("installer/build.bat", "Batch build script", True),
        ("installer/test-installer.ps1", "Testing script", False)
    ]
    
    for script_path, description, needs_wix in scripts:
        if not Path(script_path).exists():
            raise FileNotFoundError(f"Missing {description}: {script_path}")
        
        # Basic content validation
        with open(script_path, 'r', encoding='utf-8') as f:
            content = f.read()
            
        if needs_wix:
            if script_path.endswith('.ps1'):
                if 'candle.exe' not in content or 'light.exe' not in content:
                    raise ValueError(f"PowerShell script missing WiX commands: {script_path}")
            elif script_path.endswith('.bat'):
                if 'candle.exe' not in content or 'light.exe' not in content:
                    raise ValueError(f"Batch script missing WiX commands: {script_path}")
        
        print(f"✓ {description}: {script_path}")

def validate_file_sizes():
    """Validate that files have reasonable sizes"""
    print("\n🔍 Validating file sizes...")
    
    size_checks = [
        ("installer/AppIcon.ico", 1000, 100000),  # 1KB - 100KB
        ("installer/installer_banner.bmp", 50000, 500000),  # 50KB - 500KB  
        ("installer/installer_header.bmp", 10000, 100000),  # 10KB - 100KB
    ]
    
    for file_path, min_size, max_size in size_checks:
        if Path(file_path).exists():
            size = Path(file_path).stat().st_size
            if size < min_size:
                print(f"⚠️ {file_path} is unusually small ({size} bytes)")
            elif size > max_size:
                print(f"⚠️ {file_path} is unusually large ({size} bytes)")
            else:
                print(f"✓ {file_path} size OK ({size} bytes)")

def simulate_build():
    """Simulate the installer build process"""
    print("\n🔧 Simulating build process...")
    
    # Create output directory
    output_dir = Path("installer/Output")
    output_dir.mkdir(exist_ok=True)
    print(f"✓ Created output directory: {output_dir}")
    
    # Simulate candle.exe (WiX compiler)
    print("✓ Simulated candle.exe compilation")
    
    # Simulate light.exe (WiX linker)  
    print("✓ Simulated light.exe linking")
    
    # Create a placeholder MSI file
    msi_path = output_dir / "ASIO4KRNL-Setup.msi"
    with open(msi_path, 'w') as f:
        f.write("# Placeholder MSI file\n")
        f.write("# This would be the actual installer in a real build\n")
    
    print(f"✓ Created placeholder MSI: {msi_path}")

def main():
    print("ASIO4KRNL Installer Build Validation")
    print("=" * 40)
    
    try:
        validate_wix_file()
        validate_source_files()
        validate_build_scripts()
        validate_file_sizes()
        simulate_build()
        
        print("\n" + "=" * 40)
        print("🎉 All validations passed!")
        print("The installer is ready to build with WiX Toolset.")
        print("")
        print("To build the actual installer:")
        print("  1. Install WiX Toolset v3.11+")
        print("  2. Run: cd installer && .\\package.ps1")
        print("  3. Test: .\\test-installer.ps1")
        
    except Exception as e:
        print(f"\n❌ Validation failed: {e}")
        return 1
    
    return 0

if __name__ == "__main__":
    import sys
    sys.exit(main())