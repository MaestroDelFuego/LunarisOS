#!/bin/bash
set -euo pipefail

echo "[*] Dumping file tree..."
if ! command -v tree &> /dev/null; then
    echo "Error: 'tree' command not found. Please install it (e.g., 'sudo apt install tree' on Debian/Ubuntu)."
    exit 1
fi
tree -a

echo -e "\n[*] Listing contents of files (excluding .iso, .o, .elf)..."
find . -type f ! -name "*.iso" ! -name "*.o" ! -name "*.elf" | while read -r file; do
    echo -e "\n=== Contents of $file ==="
    cat "$file"
done

echo "[✓] Done!"