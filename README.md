cutframe
========
The cutframe program will read some frames from a video or audio data file, 
and then write to another file. There should be a file record the length
of all frames, so that the cutframe program can get the length of each frame
before read a frame from a video or audio data file. Take the file "LenFile.txt"
as a example of length file that record the length of all frames.

You can use the program like this:
$ ./cutframe.out src.dat src.txt outfile 2 120
# "src.dat" is a video or audio data file.
# "src.txt" is the file record the length of each frame.
# "outfile" is the output file name.
# "2" is the first frame to cut.
# "120" is the last frame to cut.
