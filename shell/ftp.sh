#!/bin/bash
  
# Check if correct number of arguments provided
if [ "$#" -ne 2 ]; then
    usage
    exit 1
fi

# FTP server details (replace with your server information)
FTP_SERVER="192.168.213.220"
FTP_USERNAME="admin"
FTP_PASSWORD="123456"
METHOD=$1
X_FILE=$2

# Function usage
usage() {
    echo "Usage: $0 <p/put|g/get> <filename>"
    exit 1
}

# Function to upload a file
upload_file() {
    FILE=$1
    ftp -n -i "$FTP_SERVER" <<END_SCRIPT
quote USER $FTP_USERNAME
quote PASS $FTP_PASSWORD
binary
put "$FILE"
quit
END_SCRIPT
}

# Function to download a file
download_file() {
    FILE=$1
    ftp -n -i "$FTP_SERVER" <<END_SCRIPT
quote USER $FTP_USERNAME
quote PASS $FTP_PASSWORD
binary
get "$FILE"
quit
END_SCRIPT
}

# Main script
case "$METHOD" in
    p|put)
        upload_file "$X_FILE"
        ;;
    g|get)
        download_file "$X_FILE"
        ;;
    *)
        usage
        exit 1
        ;;
esac

exit 0
