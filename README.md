## Project Description

I implemented the Huffman Encoding algorithm to compress/decompress either a file or a directory.

### To Run

**./fileCompressor \<flag> \<path or file> |codebook|**

### Flags

- \-b: indexes the given file/folder and outputs a huffman codebook in the same directory that fileCompressor was invoked in

- \-c: compress a given file/folder using the given codebook

- \-d: decompress a given file/folder using the given codebook

- \-R: runs any of the above operations in recursive mode, which means the file argument needs to be a path, and the fileCompressor will recursive descend into the given directory and all subdirectories, performing the given operation to all files found

## What I plan to change in the future

Being a rather novice programmer when I originally completed this project, I believe my code contains a littany of bugs. For instance, if memory serves, my shoddy use of delimeters causes the entire algorithm to break if it attempts to compress a file containing even a single dollar sign. I intend to go back and fix these bugs and, most importantly, make the code more modular and clean.
