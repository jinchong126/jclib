#!/bin/bash
  
# Check if correct number of arguments provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <p/put|g/get> <filename>"
    exit 1
fi

# FTP server details (replace with your server information)
FTP_SERVER="192.168.213.220"
FTP_USERNAME="admin"
FTP_PASSWORD="123456"

# Function to upload a file
upload_file() {
    ftp -n -i "$FTP_SERVER" <<END_SCRIPT
quote USER $FTP_USERNAME
quote PASS $FTP_PASSWORD
binary
put "$1"
quit
END_SCRIPT
}

# Function to download a file
download_file() {
    ftp -n -i "$FTP_SERVER" <<END_SCRIPT
quote USER $FTP_USERNAME
quote PASS $FTP_PASSWORD
binary
get "$1"
quit
END_SCRIPT
}

# Main script
case "$1" in
    p|put)
        upload_file "$2"
        ;;
    g|get)
        download_file "$2"
        ;;
    *)
        echo "Invalid option. Usage: $0 <p/put|g/get> <filename>"
        exit 1
        ;;
esac

exit 0
