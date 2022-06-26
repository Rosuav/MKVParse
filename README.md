MKV format parsing
==================

This has no purpose as a project. It exists so that I can explore the MKV
file format and get a better understanding of its capabilities.

The original file "Monody.mkv" (not included in repository) was downloaded
from The Fat Rat's Youtube channel. For the analysis being done here, these
reduced versions of it are used, all clipped to the lyric section only:

1) Transcoded.mkv - transcoded at original video resolution
2) Transcoded2.mkv - same thing but with a different FFMPEG algorithm
3) Copy.mkv, Copy2.mkv - avoiding transcoding by copying
4) Small.mkv, Small2.mkv - reduced video resolution

Commands to produce these are in the Makefile for reference.
