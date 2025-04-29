# μvc

A CLI VCS (command line interface version control system) app tailored towards musicians.

## Features
- Staging of music files
- Creating snapshots of staged files at a certain timestamp
- Listing all snapshots
- Restoring previously snapped files to working directory (not yet implemented)

## Usage
Usage is fairly similar to Git.

To install, please build from source.

Run `muvc {command} {argument(s)}`

### Commands
- `about`: show information about app
- `init`: initialize an empty muvc repository in the current working directory.
- `stage file`: stage a file
- `snap -m "message"`: create a snapshot of the currently staged files
- `list`: list all snaps
- `check hash`: restore files from a previous snap to working directory (not yet implemented)
- `--help`: print help message