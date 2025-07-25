# μvc

A CLI VCS (command line interface version control system) app tailored towards musicians.

## Features
- Staging of music files
- Creating snapshots of staged files at a certain timestamp
- Listing all snapshots
- Restoring previously snapped files to working directory
- Restoring shelved files after a `check` command

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
- `check hash`: restore files from a previous snap to working directory
- `restore`: restore prior files after a `check` command
- `--help`: print help message

### Example usage

1. In working directory, run `muvc init` to initialize an empty repository
2. Run `muvc stage example.wav` to stage `example.wav` for snapping
3. Run `muvc snap -m "first revision without backing vocals"` to create a snap
4. Edit `example.wav` and save it by overriding the file
5. Run `muvc stage example.wav` to stage the modified `example.wav` for a newer snap
6. Run `muvc snap -m "added backing vocals"` to snap the newer version
7. Run `muvc list` to check all snaps and ids
8. Run `muvc check 9d490476` to check the snap with the id `9d490476` (for example our first snap)
9. You can now listen to the previous version of `example.wav`
10. Run `muvc restore` to restore the file to the newer (shelved) version

### License

This project is licensed under the GPL-3.0 license, see [LICENSE](LICENSE) file for more details.