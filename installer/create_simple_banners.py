#!/usr/bin/env python3
"""
Create minimal BMP files for installer banners
"""

def create_bmp_file(filename, width, height, color=(64, 64, 128)):
    """Create a simple BMP file with solid color"""
    # Calculate padding for 4-byte alignment
    row_size = ((width * 3 + 3) // 4) * 4
    pixel_data_size = row_size * height
    file_size = 54 + pixel_data_size
    
    # BMP header (14 bytes)
    header = bytearray(14)
    header[0:2] = b'BM'  # Signature
    header[2:6] = file_size.to_bytes(4, 'little')  # File size
    header[6:10] = (0).to_bytes(4, 'little')  # Reserved
    header[10:14] = (54).to_bytes(4, 'little')  # Offset to pixel data
    
    # DIB header (40 bytes)
    dib_header = bytearray(40)
    dib_header[0:4] = (40).to_bytes(4, 'little')  # DIB header size
    dib_header[4:8] = width.to_bytes(4, 'little')  # Width
    dib_header[8:12] = height.to_bytes(4, 'little')  # Height
    dib_header[12:14] = (1).to_bytes(2, 'little')  # Planes
    dib_header[14:16] = (24).to_bytes(2, 'little')  # Bits per pixel
    dib_header[16:20] = (0).to_bytes(4, 'little')  # Compression
    dib_header[20:24] = pixel_data_size.to_bytes(4, 'little')  # Image size
    dib_header[24:28] = (2835).to_bytes(4, 'little')  # X pixels per meter
    dib_header[28:32] = (2835).to_bytes(4, 'little')  # Y pixels per meter
    dib_header[32:36] = (0).to_bytes(4, 'little')  # Colors used
    dib_header[36:40] = (0).to_bytes(4, 'little')  # Colors important
    
    # Create pixel data (BGR format, bottom-up)
    pixel_data = bytearray(pixel_data_size)
    bgr_color = bytes([color[2], color[1], color[0]])  # Convert RGB to BGR
    
    for y in range(height):
        row_start = y * row_size
        for x in range(width):
            pixel_start = row_start + (x * 3)
            pixel_data[pixel_start:pixel_start+3] = bgr_color
    
    # Write the file
    with open(filename, 'wb') as f:
        f.write(header)
        f.write(dib_header)
        f.write(pixel_data)

def main():
    print("Creating installer banner BMP files...")
    
    # Create installer dialog banner (493x312) - dark blue
    create_bmp_file('installer_banner.bmp', 493, 312, (44, 62, 80))
    print("✓ Created installer_banner.bmp (493x312)")
    
    # Create installer header banner (493x58) - slightly lighter blue
    create_bmp_file('installer_header.bmp', 493, 58, (52, 73, 94))
    print("✓ Created installer_header.bmp (493x58)")

if __name__ == "__main__":
    main()