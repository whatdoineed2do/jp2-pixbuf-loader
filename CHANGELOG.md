# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [Unreleased] -
### Added
- Thumbnailer file to support displaying JPEG2000 thumbnails in file managers
- Implemented image_save and added tests for saving

### Fixed
- Fix installing to a different prefix
- Fix SYCC444 bug
- Fix image object not being destroyed on successful load into pixbuf

## [0.0.2] - 2020-09-25
### Added
- Support for CMYK colorspace

### Fixed
- Fix SIGSEGV happening when cleaning up after a failed load.

## [0.0.1] - 2020-04-01
### Added
- Support for .j2c, .j2k, .jp2, .jpc, .jpf, .jpm, .jpx file extensions
- Support for RGB, GRAY, GRAY12, sYCC420, sYCC422, sYCC444 colorspaces
- Multiple tests

