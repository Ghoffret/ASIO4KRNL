#!/usr/bin/env python3
"""
Create installer banner images for ASIO4KRNL
"""

from PIL import Image, ImageDraw, ImageFont
import os

def create_installer_banner():
    """Create installer dialog banner (493x312 pixels)"""
    width, height = 493, 312
    img = Image.new('RGB', (width, height), color='#1a1a1a')
    draw = ImageDraw.Draw(img)
    
    # Try to use a system font, fall back to default
    try:
        font_large = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 32)
        font_medium = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 16)
    except:
        font_large = ImageFont.load_default()
        font_medium = ImageFont.load_default()
    
    # Draw gradient background
    for y in range(height):
        color_val = int(26 + (60 * y / height))
        color = (color_val, color_val, color_val + 10)
        draw.line([(0, y), (width, y)], fill=color)
    
    # Add title
    draw.text((50, 50), "ASIO4KRNL", fill='white', font=font_large)
    draw.text((50, 90), "Professional Audio Driver", fill='#cccccc', font=font_medium)
    
    # Add feature bullets
    features = [
        "• Low-latency audio performance",
        "• USB Audio Class compatibility", 
        "• Professional control panel",
        "• Windows 10/11 support"
    ]
    
    y_pos = 150
    for feature in features:
        draw.text((50, y_pos), feature, fill='#aaaaaa', font=font_medium)
        y_pos += 25
    
    return img

def create_installer_header():
    """Create installer header banner (493x58 pixels)"""
    width, height = 493, 58
    img = Image.new('RGB', (width, height), color='#2c3e50')
    draw = ImageDraw.Draw(img)
    
    try:
        font = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 18)
    except:
        font = ImageFont.load_default()
    
    # Draw gradient
    for y in range(height):
        blue_val = int(44 + (20 * y / height))
        color = (44, 62, blue_val + 30)
        draw.line([(0, y), (width, y)], fill=color)
    
    # Add title
    draw.text((20, 20), "ASIO4KRNL Professional Audio Driver Setup", fill='white', font=font)
    
    return img

def main():
    print("Creating installer banner images...")
    
    try:
        # Create banner images
        banner = create_installer_banner()
        header = create_installer_header()
        
        # Save images
        banner.save('installer_banner.bmp')
        header.save('installer_header.bmp')
        
        print("✓ Created installer_banner.bmp (493x312)")
        print("✓ Created installer_header.bmp (493x58)")
        
    except ImportError:
        print("PIL/Pillow not available, creating placeholder BMP files...")
        
        # Create minimal BMP headers for placeholders
        def create_placeholder_bmp(filename, width, height):
            # BMP header for 24-bit RGB
            file_size = 54 + (width * height * 3)
            bmp_header = b'BM'  # Signature
            bmp_header += file_size.to_bytes(4, 'little')  # File size
            bmp_header += b'\x00\x00\x00\x00'  # Reserved
            bmp_header += (54).to_bytes(4, 'little')  # Offset to pixel data
            bmp_header += (40).to_bytes(4, 'little')  # DIB header size
            bmp_header += width.to_bytes(4, 'little')  # Width
            bmp_header += height.to_bytes(4, 'little')  # Height
            bmp_header += (1).to_bytes(2, 'little')  # Planes
            bmp_header += (24).to_bytes(2, 'little')  # Bits per pixel
            bmp_header += b'\x00' * 24  # Compression and other fields
            
            # Simple dark blue pixels
            pixel_data = b'\x40\x40\x80' * (width * height)
            
            with open(filename, 'wb') as f:
                f.write(bmp_header)
                f.write(pixel_data)
        
        create_placeholder_bmp('installer_banner.bmp', 493, 312)
        create_placeholder_bmp('installer_header.bmp', 493, 58)
        
        print("✓ Created placeholder installer_banner.bmp")
        print("✓ Created placeholder installer_header.bmp")

if __name__ == "__main__":
    main()