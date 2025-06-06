
The Crazy Hacker's Crazy Guide to Bup Craziness
===============================================

Despite what you might have heard, bup is not that crazy, and neither are
you if you're trying to figure out how it works.  But it's also (as of this
writing) rather new and the source code doesn't have a lot of comments, so
it can be a little confusing at first glance.  This document is designed to
make it easier for you to get started if you want to add a new feature, fix
a bug, or just understand how it all works.


Bup Source Code Layout
----------------------

As you're reading this, you might want to look at different parts of the bup
source code to follow along and see what we're talking about.  bup's code is
written primarily in python with a bit of C code in speed-sensitive places. 
Here are the most important things to know:

 - The main program is a fairly small C program that mostly just
   initializes the correct Python interpreter and then runs
   bup.main.main().  This arrangement was chosen in order to give us
   more flexibility.  For example:

     - It allows us to avoid
       [crashing on some Unicode-unfriendly command line arguments](https://bugs.python.org/issue35883)
       which is critical, given that paths can be arbitrary byte
       sequences.

     - It allows more flexibility in dealing with upstream changes
       like the breakage of our ability to manipulate the
       processes arguement list on platforms that support it during
       the Python 3.9 series.

     - It means that we'll no longer be affected by any changes to the
       `#!/...` path, i.e. if `/usr/bin/python`, or
       `/usr/bin/python3`, or whatever we'd previously selected during
       `./configure` were to change from 2 to 3, or 3.5 to 3.20.

   The version of python bup uses is determined by the `python-config`
   program selected by `./configure`.  It tries to find a suitable
   default unless `BUP_PYTHON_CONFIG` is set in the environment.

 - bup supports both internal and external subcommands.  The former
   are the most common, and are all located in lib/bup/cmd/.  They
   must be python modules named lib/bup/cmd/COMMAND.py, and must
   contain a `main(argv)` function that will be passed the *binary*
   command line arguments (bytes, not strings).  The filename must
   have underscores for any dashes in the subcommand name.  The
   external subcommands are in lib/cmd/.

 - The python code is all in lib/bup.

 - lib/bup/\*.py contains the python code (modules) that bup depends on.
   That directory name seems a little silly (and worse, redundant) but there
   seemed to be no better way to let programs write "from bup import
   index" and have it work.  Putting bup in the top level conflicted with
   the 'bup' command; calling it anything other than 'bup' was fundamentally
   wrong, and doesn't work when you install bup on your system in /usr/lib
   somewhere.  So we get the annoyingly long paths.


Repository Structure
====================

Before you can talk about how bup works, we need to first address what it
does.  The purpose of bup is essentially to let you "replicate" data between
two main data structures:

1. Your computer's filesystem;

2. A bup repository. (Yes, we know, that part also resides in your
   filesystem.  Stop trying to confuse yourself.  Don't worry, we'll be
   plenty confusing enough as it is.)

Essentially, copying data from the filesystem to your repository is called
"backing stuff up," which is what bup specializes in.  Normally you initiate
a backup using the 'bup save' command, but that's getting ahead of
ourselves.

For the inverse operation, ie. copying from the repository to your
filesystem, you have several choices; the main ones are 'bup restore', 'bup
ftp', 'bup fuse', and 'bup web'.

Now, those are the basics of backups.  In other words, we just spent about
half a page telling you that bup backs up and restores data.  Are we having
fun yet?

The next thing you'll want to know is the format of the bup repository,
because hacking on bup is rather impossible unless you understand that part. 
In short, a bup repository is a git repository.  If you don't know about
git, you'll want to read about it now.  A really good article to read is
"Git for Computer Scientists" - you can find it in Google.  Go read it now. 
We'll wait.

Got it?  Okay, so now you're an expert in blobs, trees, commits, and refs,
the four building blocks of a git repository.  bup uses these four things,
and they're formatted in exactly the same way as git does it, so you can use
git to manipulate the bup repository if you want, and you probably won't
break anything.  It's also a comfort to know you can squeeze data out using
git, just in case bup fails you, and as a developer, git offers some nice
tools (like 'git rev-list' and 'git log' and 'git diff' and 'git show' and
so on) that allow you to explore your repository and help debug when things
go wrong.

Now, bup does use these tools a little bit differently than plain git.  We
need to do this in order to address two deficiencies in git when used for
large backups, namely a) git bogs down and crashes if you give it really
large files; b) git is too slow when you give it too many files; and c) git
doesn't store detailed filesystem metadata.

Let's talk about each of those problems in turn.


Handling large files (cmd/split, hashsplit.split_to_blob_or_tree)
--------------------

The primary reason git can't handle huge files is that it runs them through
xdelta, which generally means it tries to load the entire contents of a file
into memory at once.  If it didn't do this, it would have to store the
entire contents of every single revision of every single file, even if you
only changed a few bytes of that file.  That would be a terribly inefficient
use of disk space, and git is well known for its amazingly efficient
repository format.

Unfortunately, xdelta works great for small files and gets amazingly slow
and memory-hungry for large files.  For git's main purpose, ie. managing
your source code, this isn't a problem.  But when backing up your
filesystem, you're going to have at least a few large files, and so it's a
non-starter.  bup has to do something totally different.

What bup does instead of xdelta is what we call "hashsplitting."  We wanted
a general-purpose way to efficiently back up *any* large file that might
change in small ways, without storing the entire file every time.  In fact,
the original versions of bup could only store a single file at a time;
surprisingly enough, this was enough to give us a large part of bup's
functionality.  If you just take your entire filesystem and put it in a
giant tarball each day, then send that tarball to bup, bup will be able to
efficiently store only the changes to that tarball from one day to the next. 
For small files, bup's compression won't be as good as xdelta's, but for
anything over a few megabytes in size, bup's compression will actually
*work*, which is a big advantage over xdelta.

How does hashsplitting work?  It's deceptively simple.  We read through the
file one byte at a time, calculating a rolling checksum of the last 64
bytes.  (Why 64?  No reason.  Literally.  We picked it out of the air.
Probably some other number is better.  Feel free to join the mailing list
and tell us which one and why.)  (The rolling checksum idea is actually
stolen from rsync and xdelta, although we use it differently.  And they use
some kind of variable window size based on a formula we don't totally
understand.)

The original rolling checksum algorithm we used was called "stupidsum,"
because it was based on the only checksum Avery remembered how to calculate at
the time.  He also remembered that it was the introductory checksum
algorithm in a whole article about how to make good checksums that he read
about 15 years ago, and it was thoroughly discredited in that article for
being very stupid.  But, as so often happens, Avery couldn't remember any
better algorithms from the article.  So what we got is stupidsum.

Since then, we have replaced the stupidsum algorithm with what we call
"rollsum," based on code in librsync.  It's essentially the same as what
rsync does, except we use a fixed window size.

(If you're a computer scientist and can demonstrate that some other rolling
checksum would be faster and/or better and/or have fewer screwy edge cases,
we need your help!  Avery's out of control!  Join our mailing list!  Please! 
Save us! ...  oh boy, I sure hope he doesn't read this)

In any case, rollsum seems to do pretty well at its job.  You can find
it in bupsplit.c.  Basically, it converts the last 64 bytes read into
a 32-bit integer.  What we then do is take the lowest 13 bits of the
rollsum, and if they're all 1's, we consider that to be the end of a
chunk.  This happens on average once every 2^13 = 8192 bytes, so the
average chunk size is 8192 bytes.  We also cap the maximum chunk size
at four times the average chunk size, i.e. 4 * 2^13, to avoid having
any arbitrarily large chunks.

(Why 13 bits?  Well, we picked the number at random and... eugh.  You're
getting the idea, right?  Join the mailing list and tell us why we're
wrong.)

(Incidentally, even though the average chunk size is 8192 bytes, the actual
probability distribution of block sizes ends up being non-uniform; if we
remember our stats classes correctly, which we probably don't, it's probably
an "exponential distribution."  The idea is that for each byte in the block,
the probability that it's the last block is one in 8192.  Thus, the
block sizes end up being skewed toward the smaller end.  That's not
necessarily for the best, but maybe it is.  Computer science to the rescue? 
You know the drill.)

Anyway, so we're dividing up those files into chunks based on the rolling
checksum.  Then we store each chunk separately (indexed by its sha1sum) as a
git blob.  Why do we split this way?  Well, because the results are actually
really nice.  Let's imagine you have a big mysql database dump (produced by
mysqldump) and it's basically 100 megs of SQL text.  Tomorrow's database
dump adds 100 rows to the middle of the file somewhere, soo it's 100.01 megs
of text.

A naive block splitting algorithm - for example, just dividing the file into
8192-byte blocks - would be a disaster.  After the first bit of text has
changed, every block after that would have a different boundary, so most of
the blocks in the new backup would be different from the previous ones, and
you'd have to store the same data all over again.  But with hashsplitting,
no matter how much data you add, modify, or remove in the middle of the
file, all the chunks *before* and *after* the affected chunk are absolutely
the same.  All that matters to the hashsplitting algorithm is the
"separator" sequence, and a single change can only affect, at most, one
separator sequence or the bytes between two separator sequences.  And
because of rollsum, about one in 8192 possible 64-byte sequences is a
separator sequence.  Like magic, the hashsplit chunking algorithm will chunk
your file the same way every time, even without knowing how it had chunked
it previously.

The next problem is less obvious: after you store your series of chunks as
git blobs, how do you store their sequence?  Each blob has a 20-byte sha1
identifier, which means the simple list of blobs is going to be 20/8192 =
0.25% of the file length.  For a 200GB file, that's 488 megs of just
sequence data.

As an overhead percentage, 0.25% basically doesn't matter.  488 megs sounds
like a lot, but compared to the 200GB you have to store anyway, it's
irrelevant.  What *is* relevant is that 488 megs is a lot of memory you have
to use in order to keep track of the list.  Worse, if you back up an
almost-identical file tomorrow, you'll have *another* 488 meg blob to keep
track of, and it'll be almost but not quite the same as last time.

Hmm, big files, each one almost the same as the last... you know where this
is going, right?

Actually no!  Ha!  We didn't split this list in the same way.  We could
have, in fact, but it wouldn't have been very "git-like", since we'd like to
store the list as a git 'tree' object in order to make sure git's
refcounting and reachability analysis doesn't get confused.  Never mind the
fact that we want you to be able to 'git checkout' your data without any
special tools.

What we do instead is we extend the hashsplit algorithm a little further
using what we call "fanout." Instead of checking just the last 13 bits of
the checksum, we use additional checksum bits to produce additional splits.
Note that (most likely due to an implementation bug), the next higher bit
after the 13 bits (marked 'x'):

  ...... '..x1'1111'1111'1111

is actually ignored (so the fanout starts just to the left of the x).
Now, let's say we use a 4-bit fanout. That means we'll break a series
of chunks into its own tree object whenever the next 4 bits of the
rolling checksum are 1, in addition to the 13 lowest ones.  Since the
13 lowest bits already have to be 1, the boundary of a group of chunks
is necessarily also always the boundary of a particular chunk.

And so on.  Eventually you'll have too many chunk groups, but you can group
them into supergroups by using another 4 bits, and continue from
there.

What you end up with is an actual tree of blobs - which git 'tree'
objects are ideal to represent.  For example, with a fanout of 4,

  ...... '1011'1111'1111' 'x1'1111'1111'1111

would produce a tree two levels deep because there are 8 (two blocks
of 4) contiguous 1 bits to the left of the ignored 14th bit.

And if you think about it, just like the original list of chunks, the
tree itself is pretty stable across file modifications.  Any one
modification will only affect the chunks actually containing the
modifications, thus only the groups containing those chunks, and so on
up the tree.  Essentially, the number of changed git objects is O(log
n) where n is the number of chunks.  Since log 200 GB, using a base of
16 or so, is not a very big number, this is pretty awesome.  Remember,
any git object we *don't* change in a new backup is one we can reuse
from last time, so the deduplication effect is pretty awesome.

Better still, the hashsplit-tree format is good for a) random instead of
sequential access to data (which you can see in action with 'bup fuse'); and
b) quickly showing the differences between huge files (which we haven't
really implemented because we don't need it, but you can try 'git diff -M -C
-C backup1 backup2 -- filename' for a good start).

So now we've split out 200 GB file into about 24 million pieces.  That
brings us to git limitation number 2.


Handling huge numbers of files (git.PackWriter)
------------------------------

git is designed for handling reasonably-sized repositories that change
relatively infrequently.  (You might think you change your source code
"frequently" and that git handles much more frequent changes than, say, svn
can handle.  But that's not the same kind of "frequently" we're talking
about.  Imagine you're backing up all the files on your disk, and one of
those files is a 100 GB database file with hundreds of daily users.  Your
disk changes so frequently you can't even back up all the revisions even if
you were backing stuff up 24 hours a day.  That's "frequently.")

git's way of doing things works really nicely for the way software
developers write software, but it doesn't really work so well for everything
else.  The #1 killer is the way it adds new objects to the repository: it
creates one file per blob.  Then you later run 'git gc' and combine those
files into a single file (using highly efficient xdelta compression, and
ignoring any files that are no longer relevant).

'git gc' is slow, but for source code repositories, the resulting
super-efficient storage (and associated really fast access to the stored
files) is worth it.  For backups, it's not; you almost never access your
backed-up data, so storage time is paramount, and retrieval time is mostly
unimportant.

To back up that 200 GB file with git and hashsplitting, you'd have to create
24 million little 8k files, then copy them into a 200 GB packfile, then
delete the 24 million files again.  That would take about 400 GB of disk
space to run, require lots of random disk seeks, and require you to go
through your data twice.

So bup doesn't do that.  It just writes packfiles directly.  Luckily, these
packfiles are still git-formatted, so git can happily access them once
they're written.

But that leads us to our next problem.


Huge numbers of huge packfiles (midx.py, bloom.py, cmd/midx, cmd/bloom)
------------------------------

Git isn't actually designed to handle super-huge repositories.  Most git
repositories are small enough that it's reasonable to merge them all into a
single packfile, which 'git gc' usually does eventually.

The problematic part of large packfiles isn't the packfiles themselves - git
is designed to expect the total size of all packs to be larger than
available memory, and once it can handle that, it can handle virtually any
amount of data about equally efficiently.  The problem is the packfile
indexes (.idx) files.  In bup we call these idx (pronounced "idix") files
instead of using the word "index," because the word index is already used
for something totally different in git (and thus bup) and we'll become
hopelessly confused otherwise.

Anyway, each packfile (*.pack) in git has an associated idx (*.idx) that's a
sorted list of git object hashes and file offsets.  If you're looking for a
particular object based on its sha1, you open the idx, binary search it to
find the right hash, then take the associated file offset, seek to that
offset in the packfile, and read the object contents.

The performance of the binary search is about O(log n) with the number of
hashes in the pack, with an optimized first step (you can read about it
elsewhere) that somewhat improves it to O(log(n)-7).

Unfortunately, this breaks down a bit when you have *lots* of packs.  Say
you have 24 million objects (containing around 200 GB of data) spread across
200 packfiles of 1GB each.  To look for an object requires you search
through about 122000 objects per pack; ceil(log2(122000)-7) = 10, so you'll
have to search 10 times.  About 7 of those searches will be confined to a
single 4k memory page, so you'll probably have to page in about 3-4 pages
per file, times 200 files, which makes 600-800 4k pages (2.4-3.6 megs)...
every single time you want to look for an object.

This brings us to another difference between git's and bup's normal use
case.  With git, there's a simple optimization possible here: when looking
for an object, always search the packfiles in MRU (most recently used)
order.  Related objects are usually clusted together in a single pack, so
you'll usually end up searching around 3 pages instead of 600, which is a
tremendous improvement.  (And since you'll quickly end up swapping in all
the pages in a particular idx file this way, it isn't long before searching
for a nearby object doesn't involve any swapping at all.)

bup isn't so lucky.  git users spend most of their time examining existing
objects (looking at logs, generating diffs, checking out branches), which
lends itself to the above optimization.  bup, on the other hand, spends most
of its time looking for *nonexistent* objects in the repository so that it
can back them up.  When you're looking for objects that aren't in the
repository, there's no good way to optimize; you have to exhaustively check
all the packs, one by one, to ensure that none of them contain the data you
want.

To improve performance of this sort of operation, bup introduces midx
(pronounced "midix" and short for "multi-idx") files.  As the name implies,
they index multiple packs at a time.

Imagine you had a midx file for your 200 packs.  midx files are a lot like
idx files; they have a lookup table at the beginning that narrows down the
initial search, followed by a binary search.  Then unlike idx files (which
have a fixed-size 256-entry lookup table) midx tables have a variably-sized
table that makes sure the entire binary search can be contained to a single
page of the midx file.  Basically, the lookup table tells you which page to
load, and then you binary search inside that page.  A typical search thus
only requires the kernel to swap in two pages, which is better than results
with even a single large idx file.  And if you have lots of RAM, eventually
the midx lookup table (at least) will end up cached in memory, so only a
single page should be needed for each lookup.

You generate midx files with 'bup midx'.  The downside of midx files is that
generating one takes a while, and you have to regenerate it every time you
add a few packs.

UPDATE: Brandon Low contributed an implementation of "bloom filters", which
have even better characteristics than midx for certain uses.  Look it up in
Wikipedia.  He also massively sped up both midx and bloom by rewriting the
key parts in C.  The nicest thing about bloom filters is we can update them
incrementally every time we get a new idx, without regenerating from
scratch.  That makes the update phase much faster, and means we can also get
away with generating midxes less often.

midx files are a bup-specific optimization and git doesn't know what to do
with them.  However, since they're stored as separate files, they don't
interfere with git's ability to read the repository.


Handling large directories (tree splitting)
-------------------------------------------

Similarly to large files, git doesn't handle frequently changing large
directories well either, since they're stored in a single tree object
using up 28 bytes plus the length of the filename for each entry
(record). If there are a lot of files, these tree objects can become
very large, and every minor change to a directory such as creating a
single new file in it requires storing an entirely new tree object.
Imagine an active Maildir containing tens or hundreds of thousands of
files.

If tree splitting is permitted (via the `bup.split.trees` config
option), then instead of writing a tree as one potentially large git
tree object, bup may split it into a subtree whose "leaves" are git
tree objects, each containing part of the original tree.  Note that
for `bup on HOST ...`  `bup.split.trees` must be set on the `HOST`.

The trees are split in a manner similar to the file hashsplitting
described above, but with the restriction that splits may only occur
between (not within) tree entries.

When a tree is split, a large directory like

	dir/{.bupm,aa,bb,...,zz}

might become

	dir/.b/{.bupm,aa,...,ii}
	dir/.bupd.1.bupd
	dir/j/{jj,...,rr}
	dir/s/{ss,...,zz}

where the ".bupd.1.bupd" inside dir/ indicates that the tree for dir/
was split, and the number (here "1") describes the number of levels
that were created (just one in this case).  The names in an
intermediate level (inside dir/, but not the leaves -- in this
example, ".b", "j", "s", etc.) are derived from the first filename
contained within each subtree, abbreviated to the shortest valid
unique prefix.  At any level, the names contained in a subtree will
always be greater than or equal to the name of the subtree itself and
less than the name of the next subtree at that level.  This makes it
possible to know which split subtree to read at every level when
looking for a given filename.

When parsing the split tree depth info file name, i.e. `.bupd.1.bupd`
in the example above, any extra bytes in the name after the
`.bupd.DEPTH` prefix, and before the final `.bupd` suffix must be
ignored.  The `DEPTH` will always be a sequence of `[0-9]+`, and any
extra bytes will begin with `.`, e.g.  `.bupd.3.SOMETHING.NEW.bupd`.
This allows future extensions.

Detailed Metadata
-----------------

So that's the basic structure of a bup repository, which is also a git
repository.  There's just one more thing we have to deal with:
filesystem metadata.  Git repositories are really only intended to
store file contents with a small bit of extra information, like
symlink targets and executable bits, so we have to store the rest
some other way.

Bup stores more complete metadata in the VFS in a file named .bupm in
each tree.  This file contains one entry for each file in the tree
object, sorted in the same order as the tree.  The first .bupm entry
is for the directory itself, i.e. ".", and its name is the empty
string, "".

Each .bupm entry contains a variable length sequence of records
containing the metadata for the corresponding path.  Each record
records one type of metadata.  Current types include a common record
type (containing the normal stat information), a symlink target type,
a hardlink target type, a POSIX1e ACL type, etc.  See metadata.py for
the complete list.

The .bupm file is optional, and when it's missing, bup will behave as
it did before the addition of metadata, and restore files using the
tree information.

The nice thing about this design is that you can walk through each
file in a tree just by opening the tree and the .bupm contents, and
iterating through both at the same time.

Since the contents of any .bupm file should match the state of the
filesystem when it was *indexed*, bup must record the detailed
metadata in the index.  To do this, bup records four values in the
index, the atime, mtime, and ctime (as timespecs), and an integer
offset into a secondary "metadata store" which has the same name as
the index, but with ".meta" appended.  This secondary store contains
the encoded Metadata object corresponding to each path in the index.

Currently, in order to decrease the storage required for the metadata
store, bup only writes unique values there, reusing offsets when
appropriate across the index.  The effectiveness of this approach
relies on the expectation that there will be many duplicate metadata
records.  Storing the full timestamps in the index is intended to make
that more likely, because it makes it unnecessary to record those
values in the secondary store.  So bup clears them before encoding the
Metadata objects destined for the index, and timestamp differences
don't contribute to the uniqueness of the metadata.

Bup supports recording and restoring hardlinks, and it does so by
tracking sets of paths that correspond to the same dev/inode pair when
indexing.  This information is stored in an optional file with the
same name as the index, but ending with ".hlink".

If there are multiple index runs, and the hardlinks change, bup will
notice this (within whatever subtree it is asked to reindex) and
update the .hlink information accordingly.

The current hardlink implementation will refuse to link to any file
that resides outside the restore tree, and if the restore tree spans a
different set of filesystems than the save tree, complete sets of
hardlinks may not be restored.


Filesystem Interaction
======================

Storing data is just half of the problem of making a backup; figuring out
what to store is the other half.

At the most basic level, piping the output of 'tar' into 'bup split' is an
easy way to offload that decision; just let tar do all the hard stuff.  And
if you like tar files, that's a perfectly acceptable way to do it.  But we
can do better.

Backing up with tarballs would totally be the way to go, except for two
serious problems:

1. The result isn't easily "seekable."  Tar files have no index, so if (as
   commonly happens) you only want to restore one file in a 200 GB backup,
   you'll have to read up to 200 GB before you can get to the beginning of
   that file.  tar is short for "tape archive"; on a tape, there was no
   better way to do it anyway, so they didn't try.  But on a disk, random
   file access is much, much better when you can figure out how.
   
2. tar doesn't remember which files it backed up last time, so it has to
   read through the entire file contents again in order to generate the
   tarball, large parts of which will then be skipped by bup since they've
   already been stored.  This is much slower than necessary.

(The second point isn't entirely true for all versions of tar. For example,
GNU tar has an "incremental" mode that can somewhat mitigate this problem,
if you're smart enough to know how to use it without hurting yourself.  But
you still have to decide which backups are "incremental" and which ones will
be "full" and so on, so even when it works, it's more error-prone than bup.)

bup divides the backup process into two major steps: a) indexing the
filesystem, and b) saving file contents into the repository.  Let's look at
those steps in detail.


Indexing the filesystem (cmd/drecurse, cmd/index, index.py)
-----------------------

Splitting the filesystem indexing phase into its own program is
nontraditional, but it gives us several advantages.

The first advantage is trivial, but might be the most important: you can
index files a lot faster than you can back them up.  That means we can
generate the index (.bup/bupindex) first, then have a nice, reliable,
non-lying completion bar that tells you how much of your filesystem remains
to be backed up.  The alternative would be annoying failures like counting
the number of *files* remaining (as rsync does), even though one of the
files is a virtual machine image of 80 GB, and the 1000 other files are each
under 10k.  With bup, the percentage complete is the *real* percentage
complete, which is very pleasant.

Secondly, it makes it easier to debug and test; you can play with the index
without actually backing up any files.

Thirdly, you can replace the 'bup index' command with something else and not
have to change anything about the 'bup save' command.  The current 'bup
index' implementation just blindly walks the whole filesystem looking for
files that have changed since the last time it was indexed; this works fine,
but something using inotify instead would be orders of magnitude faster. 
Windows and MacOS both have inotify-like services too, but they're totally
different; if we want to support them, we can simply write new bup commands
that do the job, and they'll never interfere with each other.

And fourthly, git does it that way, and git is awesome, so who are we to
argue?

So let's look at how the index file works.

First of all, note that the ".bup/bupindex" file is not the same as git's
".git/index" file.  The latter isn't used in bup; as far as git is
concerned, your bup repository is a "bare" git repository and doesn't have a
working tree, and thus it doesn't have an index either.

However, the bupindex file actually serves exactly the same purpose as git's
index file, which is why we still call it "the index." We just had to
redesign it for the usual bup-vs-git reasons, mostly that git just isn't
designed to handle millions of files in a single repository.  (The only way
to find a file in git's index is to search it linearly; that's very fast in
git-sized repositories, but very slow in bup-sized ones.)

Let's not worry about the exact format of the bupindex file; it's still not
optimal, and will probably change again.  The most important things to know
about bupindex are:

 - You can iterate through it much faster than you can iterate through the
   "real" filesystem (using something like the 'find' command).
   
 - If you delete it, you can get it back just by reindexing your filesystem
   (although that can be annoying to wait for); it's not critical to the
   repository itself.
   
 - You can iterate through only particular subtrees if you want.
 
 - There is no need to have more than one index for a particular filesystem,
   since it doesn't store anything about backups; it just stores file
   metadata.  It's really just a cache (or 'index') of your filesystem's
   existing metadata.  You could share the bupindex between repositories, or
   between multiple users on the same computer.  If you back up your
   filesystem to multiple remote repositories to be extra safe, you can
   still use the same bupindex file across all of them, because it's the
   same filesystem every time.
   
 - Filenames in the bupindex are absolute paths, because that's the best way
   to ensure that you only need one bupindex file and that they're
   interchangeable.
   

A note on file "dirtiness"
--------------------------

The concept on which 'bup save' operates is simple enough; it reads through
the index and backs up any file that is "dirty," that is, doesn't already
exist in the repository.

Determination of dirtiness is a little more complicated than it sounds.  The
most dirtiness-relevant flag in the bupindex is IX_HASHVALID; if
this flag is reset, the file *definitely* is dirty and needs to be backed
up.  But a file may be dirty even if IX_HASHVALID is set, and that's the
confusing part.

The index stores a listing of files, their attributes, and
their git object ids (sha1 hashes), if known.  The "if known" is what
IX_HASHVALID is about.  When 'bup save' backs up a file, it sets
the sha1 and sets IX_HASHVALID; when 'bup index' sees that a file has
changed, it leaves the sha1 alone and resets IX_HASHVALID.

Remember that the index can be shared between users, repositories, and
backups.  So IX_HASHVALID doesn't mean your repository *has* that sha1 in
it; it only means that if you *do* have it, that you don't need to back up
the file.  Thus, 'bup save' needs to check every file in the index to make
sure its hash exists, not just that it's valid.

There's an optimization possible, however: if you know a particular tree's
hash is valid and exists (say /usr), then you don't need to check the
validity of all its children; because of the way git trees and blobs work,
if your repository is valid and you have a tree object, then you have all
the blobs it points to.  You won't back up a tree object without backing up
its blobs first, so you don't need to double check it next time.  (If you
really want to double check this, it belongs in a tool like 'bup fsck' or
'git fsck'.) So in short, 'bup save' on a "clean" index (all files are
marked IX_HASHVALID) can be very fast; we just check our repository and see
if the top level IX_HASHVALID sha1 exists.  If it does, then we're done.

Similarly, if not the entire index is valid, you can still avoid recursing
into subtrees if those particular subtrees are IX_HASHVALID and their sha1s
are in the repository.  The net result is that, as long as you never lose
your index, 'bup save' can always run very fast.

Another interesting trick is that you can skip backing up files even if
IX_HASHVALID *isn't* set, as long as you have that file's sha1 in the
repository.  What that means is you've chosen not to backup the latest
version of that file; instead, your new backup set just contains the
most-recently-known valid version of that file.  This is a good trick if you
want to do frequent backups of smallish files and infrequent backups of
large ones.  Each of your backups will be "complete," in that they contain
all the small files and the large ones, but intermediate ones will just
contain out-of-date copies of the large files. Note that this isn't done
right now, and 'bup save --smaller' doesn't store bigger files _at all_.

A final game we can play with the bupindex involves restoring: when you
restore a directory from a previous backup, you can update the bupindex
right away.  Then, if you want to restore a different backup on top, you can
compare the files in the index against the ones in the backup set, and
update only the ones that have changed.  (Even more interesting things
happen if people are using the files on the restored system and you haven't
updated the index yet; the net result would be an automated merge of all
non-conflicting files.) This would be a poor man's distributed filesystem. 
The only catch is that nobody has written this feature for 'bup restore'
yet.  Someday!


How 'bup save' works (cmd/save)
--------------------

This section is too boring and has been omitted.  Once you understand the
index, there's nothing special about bup save.


Retrieving backups: the bup vfs layer (vfs.py, cmd/ls, cmd/ftp, cmd/fuse)
=====================================

One of the neat things about bup's storage format, at least compared to most
backup tools, is it's easy to read a particular file, or even part of a
file.  That means a read-only virtual filesystem is easy to generate and
it'll have good performance characteristics.  Because of git's commit
structure, you could even use branching and merging to make a transactional
read-write filesystem... but that's probably getting a little out of bup's
scope.  Who knows what the future might bring, though?

Read-only filesystems are well within our reach today, however.  The 'bup
ls', 'bup ftp', and 'bup fuse' commands all use a "VFS" (virtual filesystem)
layer to let you access your repositories.  Feel free to explore the source
code for these tools and vfs.py - they're pretty straightforward.  Some
things to note:

 - None of these use the bupindex for anything.
 
 - For user-friendliness, they present your refs/commits/trees as a single
   hierarchy (ie.  a filesystem), which isn't really how git repositories
   are formatted.  So don't get confused!


Handling Python 3's insistence on strings
=========================================

In Python 2 strings were bytes, and bup used them for all kinds of
data.  Python 3 made a pervasive backward-incompatible change to treat
all strings as Unicode, i.e. in Python 2 'foo' and b'foo' were the
same thing, while u'foo' was a Unicode string.  In Python 3 'foo'
became synonymous with u'foo', completely changing the type and
potential content, depending on the locale.

In addition, and particularly bad for bup, Python 3 also (initially)
insisted that all kinds of things were strings that just aren't (at
least not on many platforms), i.e. user names, groups, filesystem
paths, etc.  There's no guarantee that any of those are always
representable in Unicode.

Over the years, Python 3 has gradually backed away from that initial
position, adding alternate interfaces like os.environb or allowing
bytes arguments to many functions like open(b'foo'...), so that in
those cases it's at least possible to accurately retrieve the system
data.

After a while, they devised the concept of
[bytesmuggling](https://www.python.org/dev/peps/pep-0383/) as a more
comprehensive solution.  In theory, this might be sufficient, but our
initial randomized testing discovered that some binary arguments would
crash Python during startup[1].  Eventually Johannes Berg tracked down
the [cause](https://sourceware.org/bugzilla/show_bug.cgi?id=26034),
and we hope that the problem will be fixed eventually in glibc or
worked around by Python, but in either case, it will be a long time
before any fix is widely available.

Before we tracked down that bug we were pursuing an approach that
would let us side step the issue entirely by manipulating the
LC_CTYPE, but that approach was somewhat complicated, and once we
understood what was causing the crashes, we decided to just let Python
3 operate "normally", and work around the issues.

Consequently, we've had to wrap a number of things ourselves that
incorrectly return Unicode strings (libacl, libreadline, hostname,
etc.)  and we've had to come up with a way to avoid the fatal crashes
caused by some command line arguments (sys.argv) described above.  To
fix the latter, for the time being, we just use a trivial sh wrapper
to redirect all of the command line arguments through the environment
in BUP_ARGV_{0,1,2,...} variables, since the variables are unaffected,
and we can access them directly in Python 3 via environb.

[1] Our randomized argv testing found that the byte smuggling approach
    was not working correctly for some values (initially discovered in
    Python 3.7, and observed in other versions).  The interpreter
    would just crash while starting up like this:

    Fatal Python error: _PyMainInterpreterConfig_Read: memory allocation failed
    ValueError: character U+134bd2 is not in range [U+0000; U+10ffff]

    Current thread 0x00007f2f0e1d8740 (most recent call first):
    Traceback (most recent call last):
      File "t/test-argv", line 28, in <module>
        out = check_output(cmd)
      File "/usr/lib/python3.7/subprocess.py", line 395, in check_output
        **kwargs).stdout
      File "/usr/lib/python3.7/subprocess.py", line 487, in run
        output=stdout, stderr=stderr)

We hope you'll enjoy bup.  Looking forward to your patches!

-- apenwarr and the rest of the bup team

<!--
Local Variables:
mode: markdown
End:
-->
