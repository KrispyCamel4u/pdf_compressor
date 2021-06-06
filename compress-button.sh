#!/bin/bash

#Compressing action

#Extract the file path
file=$2

#Remove path and extension for outut file name
filename=$(basename "${file}")

#Prepare the output file name
if [ $3 == 0 ] 
then
    output_file="$(dirname "${file}")/${filename%.pdf}_compressed.pdf"
else
    output_file="$(dirname "${file}")/${3%}.pdf"
fi
#Keep backslashes as ghost script doesn't handle it properly
file=${file//\\}
output_file=${output_file//\\}

# echo $file
echo $output_file

#Default compression is high resolution
level=prepress

#script arguments
if [ $1 == 3 ]
   then
       level=prepress
elif [ $1 == 2 ]
   then
       level=ebook
elif [ $1 == 1 ]
   then
       level=screen
fi

# echo $level

#Compress File

gs -sDEVICE=pdfwrite -dCompatibilityLevel=1.4 \
-dPDFSETTINGS=/$level -dNOPAUSE -dPrinted=false -dQUIET -dBATCH \
-sOutputFile="${output_file}" "${file}"

#Success Message
# if [ $?==0 ]
# then
#   python3 success.py
# fi
