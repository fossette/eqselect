# eqselect

**eqselect** is a tool that selects a random media file from an implied or a
specified directory, and executes it.  All media files will eventually be
executed once before an already executed media is executed a second time,
an so on, thus the name EQualSELECT.  VLC is used as the default player.

The following command line options are available:  
  _**-c**_    Continuous selection instead of the default random selection.  
  _**-l**_    Consider the current directory as a leaf, i.e. sub-directories are ignored.  
  _**-r**_    Repeat the last executed file.  
  _**-z**_    Reset the executed files list.

  Optional parameter: the directory to use.  If not specified, the current directory is used.

## How to build and install **eqselect**

1. Download the source files and store them in a directory
2. Go to that directory in a terminal window
3. To built the executable file, type `make`
4. To install the executable file, type `make install` as a
superuser.  The Makefile will copy the executable file into the
`/usr/bin` directory.  If you want it elsewhere, feel free to copy
it by hand instead.

## Troubleshooting

_**eqselect: Command not found.**_  
`eqselect` is not found in your executables' path.  Try `make install` as a superuser.

_**ERROR: No file to execute in the current directory.**_  
**eqselect** looks for media files in the current directory and
its sub-directories.  You may specify a relevant path as a
command line option.

## Version history

0.9 - 2015/06/20 - Initial release

## TO DO List

Configuration files for more flexibility, and to avoid code rebuilds.

## Compatibility

**eqselect** has been developed and tested under FreeBSD 10.1.
It should be compatible with other systems because only standard
libraries have been used.

## Disclaimer

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

## Donations

Thanks for the support!  
https://flattr.com/profile/fossette
