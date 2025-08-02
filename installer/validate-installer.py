#!/usr/bin/env python3
"""
ASIO4KRNL MSI Installer Validation Script
=========================================

This script validates the MSI installer infrastructure without requiring
WiX Toolset to be installed. It checks:

1. File structure and dependencies
2. XML syntax validation
3. Build script syntax
4. Path references
5. Component configurations

Usage: python3 validate-installer.py
"""

import os
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

def check_file_exists(filepath, description):
    """Check if a file exists and report the result."""
    if os.path.exists(filepath):
        print(f"✓ {description}: {filepath}")
        return True
    else:
        print(f"✗ {description} MISSING: {filepath}")
        return False

def validate_xml_file(filepath, description):
    """Validate XML file syntax."""
    try:
        tree = ET.parse(filepath)
        print(f"✓ {description} XML is valid")
        return tree
    except ET.ParseError as e:
        print(f"✗ {description} XML ERROR: {e}")
        return None
    except Exception as e:
        print(f"✗ {description} ERROR: {e}")
        return None

def main():
    print("ASIO4KRNL MSI Installer Validation")
    print("=" * 40)
    
    # Change to installer directory
    installer_dir = Path(__file__).parent
    os.chdir(installer_dir)
    print(f"Working directory: {os.getcwd()}")
    print()
    
    validation_passed = True
    
    # Check core installer files
    print("1. Checking core installer files...")
    files_to_check = [
        ("ASIO4KrnlInstaller.wxs", "WiX source file"),
        ("ASIO4KrnlInstaller.wixproj", "WiX project file"),
        ("build-msi.bat", "Batch build script"),
        ("build-msi.ps1", "PowerShell build script"),
        ("AppIcon.ico", "Application icon"),
        ("README.txt", "Original installer readme"),
        ("BUILD_INSTRUCTIONS.md", "Build instructions")
    ]
    
    for filepath, description in files_to_check:
        if not check_file_exists(filepath, description):
            validation_passed = False
    print()
    
    # Check referenced files
    print("2. Checking referenced files...")
    referenced_files = [
        ("../README.md", "Main readme file"),
        ("../LICENSE", "License file"),
        ("../src/driver/ASIOUSB.inf", "Driver INF file"),
        ("../src/common/Version.h", "Version header")
    ]
    
    for filepath, description in referenced_files:
        if not check_file_exists(filepath, description):
            validation_passed = False
    print()
    
    # Validate XML files
    print("3. Validating XML files...")
    wix_tree = validate_xml_file("ASIO4KrnlInstaller.wxs", "WiX source")
    wixproj_tree = validate_xml_file("ASIO4KrnlInstaller.wixproj", "WiX project")
    print()
    
    # Check WiX configuration
    if wix_tree:
        print("4. Checking WiX configuration...")
        try:
            ns = {'wix': 'http://schemas.microsoft.com/wix/2006/wi'}
            
            # Check product info
            product = wix_tree.find('.//wix:Product', ns)
            if product is not None:
                print(f"✓ Product: {product.get('Name')} v{product.get('Version')}")
                print(f"✓ Manufacturer: {product.get('Manufacturer')}")
                print(f"✓ Upgrade Code: {product.get('UpgradeCode')}")
            else:
                print("✗ Product element not found")
                validation_passed = False
            
            # Check components
            components = wix_tree.findall('.//wix:Component', ns)
            print(f"✓ Found {len(components)} components:")
            for comp in components:
                comp_id = comp.get('Id')
                files = comp.findall('.//wix:File', ns)
                print(f"  - {comp_id}: {len(files)} files")
            
            # Check features
            features = wix_tree.findall('.//wix:Feature', ns)
            print(f"✓ Found {len(features)} features")
            
        except Exception as e:
            print(f"✗ WiX configuration error: {e}")
            validation_passed = False
        print()
    
    # Check project configuration
    if wixproj_tree:
        print("5. Checking project configuration...")
        try:
            ns = {'msbuild': 'http://schemas.microsoft.com/developer/msbuild/2003'}
            
            # Check output settings
            output_name = wixproj_tree.find('.//msbuild:OutputName', ns)
            if output_name is not None:
                print(f"✓ Output name: {output_name.text}")
            
            # Check compile items
            compile_items = wixproj_tree.findall('.//msbuild:Compile', ns)
            print(f"✓ Compile items: {len(compile_items)}")
            for item in compile_items:
                print(f"  - {item.get('Include')}")
            
            # Check extensions
            extensions = wixproj_tree.findall('.//msbuild:WixExtension', ns)
            print(f"✓ WiX extensions: {len(extensions)}")
            
        except Exception as e:
            print(f"✗ Project configuration error: {e}")
            validation_passed = False
        print()
    
    # Check build script syntax
    print("6. Checking build scripts...")
    try:
        with open("build-msi.bat", "r") as f:
            bat_content = f.read()
            if "msbuild" in bat_content and "ASIO4KrnlInstaller.wixproj" in bat_content:
                print("✓ Batch script references correct files")
            else:
                print("✗ Batch script missing required references")
                validation_passed = False
    except Exception as e:
        print(f"✗ Batch script error: {e}")
        validation_passed = False
    
    try:
        with open("build-msi.ps1", "r") as f:
            ps_content = f.read()
            if "msbuild" in ps_content and "ASIO4KrnlInstaller.wixproj" in ps_content:
                print("✓ PowerShell script references correct files")
            else:
                print("✗ PowerShell script missing required references")
                validation_passed = False
    except Exception as e:
        print(f"✗ PowerShell script error: {e}")
        validation_passed = False
    print()
    
    # Final result
    print("=" * 40)
    if validation_passed:
        print("✓ VALIDATION PASSED")
        print("The MSI installer infrastructure appears to be correctly configured.")
        print("\nTo build the MSI installer:")
        print("1. Install Visual Studio with Windows Driver Kit")
        print("2. Install WiX Toolset v3.11+")
        print("3. Run build-msi.bat or build-msi.ps1 as Administrator")
        return 0
    else:
        print("✗ VALIDATION FAILED")
        print("Please fix the issues above before building the installer.")
        return 1

if __name__ == "__main__":
    sys.exit(main())