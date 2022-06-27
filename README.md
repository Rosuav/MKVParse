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

Fetch the contents of https://github.com/QBobWatson/python-ebml as ebml/
Will need branch rosuav/patch-1 so probably get from my fork

Copy.mkv has no problem; Copy2.mkv exhibits the issue.

When the issue is present, the symptom is that the video does not begin
playing until some time after the audio has begun. VLC renders this by
having the video player collapse until the right point for it to start,
at which time it fully opens. The cause appears to be copying from the
first keyframe after the chosen seek point, as per the info on FFMPEG's
wiki at https://trac.ffmpeg.org/wiki/Seeking :

"""Basically, if you specify "second 157" and there is no key frame
until second 159, it will include two seconds of audio (with no video)
at the start, then will start from the first key frame."""

HOWEVER! The wiki says that this will happen when -ss is an *input option*,
meaning when "ffmpeg -ss TIME -i FILE" is used, and that it will NOT happen
when -ss is an output option ie "ffmpeg -i FILE -ss TIME". But my experience
is that the problem occurs when -ss is an output option, even if it is also
an input option (see Copy3.mkv in the Makefile).

What I want to do here, ideally, would be:

1. Seek rapidly to the position specified (2:46)
2. Seek backwards to the most recent key frame
3. Parse frames until the original position is re-reached
4. Render frames until the next key frame
5. Copy frames thereafter.

This is complicated, but as a next-best, I would accept this:

1. Seek rapidly to the position specified
2. Render black frames at full resolution until the next key frame
3. Copy frames thereafter.

Alternatively:

1. Seek rapidly to a position somewhere approximately before the chosen one
   (user-specified - see Copy3.mkv with "fast seek 2:30" followed by "render
   until 2:46")
2. Parse frames until the original position is re-reached
3. Render frames to key, then copy frames, as per ideal.

If this were done as a dedicated "take clip from file" program, using libavutil
for the underlying work, how would it be done?

1. Take file name, start pos, end pos, output file
2. Read file, enumerate keyframes? Follow one of the above algos.
3. Figure out how to do a "-c copy" using libavutil and friends
4. Ensure that all unknown metadata is copied faithfully.

See: https://ffmpeg.org/doxygen/trunk/index.html
