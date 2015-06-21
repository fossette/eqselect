# eqselect

vbswitch is a tool to easily switch between different .vdi files
in the same directory.  A .vdi file is a VitualBox virtual disk.
Normally, a virtual disk should be cloned so both the original and
the clone can co-exist in the same VirtualBox host.  In this case,
the only difference is their internal Serial Disk ID which are
always distinct so VirtualBox can know which disk is which.  The
original and the clone could even be assigned to the same virtual
machine if needed.

However, there are cases where one just wishes to perform some tests
and discard the results (or keep them for good if they are positive).
A way to do that is to duplicate the .vdi file, rename it to a
significant name that perhaps include the date.  No need to change
the virtual machine's configuration options.  When the test is over,
either delete the duplicate .vdi file if the current state is wanted,
or delete the current .vdi file, and rename the duplicate file to the
original .vdi filename.

All this renaming can be a source of errors, and perhaps lost of data.
This is where vbswitch become handy.  It performs all the file
renaming automatically and quickly.

Getting started with eqselect

Suppose you have an Ubuntu 14.04 LTS guest virtual machine, and it's
virtual disk is ubuntu14.vdi.  Do the following steps:
1) Create an empty file called ubuntu14-original.vbswitch  This file
tells vbswitch that ubuntu14.vdi is now your original .vdi file.
2) Create as many .vdi duplicates as you require, and identify them
clearly.  For example, you can have ubuntu14-kerneltest.vdi and
ubuntu14-drivertest.vdi

The only thing to make sure is that all files must share the same
base name for vbswitch to work.  I used ubuntu14 as the base name
in the above example.

WARNING: The virtual machine of the disks you wishes to switch
MUST NOT RUN.  I never tried it, but no doubt in my mind that it's
very damaging.

To make the actual switch, you can run vbswitch directly in the
virtual machine folder.  You can also run vbswitch from anywhere,
but specify the virtual machine folder path as the first argument
of vbswitch.  I personally do this in a desktop menu shortcut.
It's quick and easy.

How to build and install eqselect

1) Download the source files and store them in a directory
2) Go to that directory in a terminal window
3) To built the executable file, type make
4) To install the executable file, type make install as a
superuser.  The Makefile will copy the executable file into the
/usr/bin directory.  If you want it elsewhere, feel free to copy
it by hand instead.

Troubleshooting

eqselect: Command not found.
eqselect is not found in your executables' path.  Try make install
as a superuser

ERROR: No .vbswitch reference file found.
Find out what is your .vdi file, and where it's located.  We'll
call it <base-name>.vdi where <base-name> is your file base name.
Create a new empty file called <base-name>-original.vbswitch
That's all there is to it.

ERROR: At least two .vdi files are required to move on.
Never easy, right?  Try cp <base-name>.vdi <base-name>-mytest.vdi

Version history

0.9       2015/06/20     Initial release

Compatibility

vbswitch has been developed and tested under FreeBSD 10.1.
It should be compatible with other systems because only standard
libraries have been used.

Disclaimer

THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Donations

Thanks for the support!
https://flattr.com/profile/fossette